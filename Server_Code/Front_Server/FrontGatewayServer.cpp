#include "FrontGatewayServer.h"

#include <chrono>
#include <cstring>
#include <iostream>

FrontGatewayServer::FrontGatewayServer(int port)
    : port(port), listener(new FrontListener(port)), running(false), next_client_id(1) {}

FrontGatewayServer::~FrontGatewayServer() {
    running = false;
    if (sender_thread.joinable()) {
        sender_thread.join();
    }

    std::lock_guard<std::mutex> lock(client_mutex);
    for (auto& kv : client_socket_map) {
        closesocket(kv.second);
    }
    delete listener;
}

int FrontGatewayServer::recv_all(SOCKET sock, char* buffer, int len) {
    int received = 0;
    while (received < len) {
        int res = recv(sock, buffer + received, len - received, 0);
        if (res <= 0) {
            return -1;
        }
        received += res;
    }
    return received;
}

int FrontGatewayServer::send_all(SOCKET sock, const char* buffer, int len) {
    int sent = 0;
    while (sent < len) {
        int res = send(sock, buffer + sent, len - sent, 0);
        if (res <= 0) {
            return -1;
        }
        sent += res;
    }
    return sent;
}

void FrontGatewayServer::remove_client(SOCKET sock) {
    std::lock_guard<std::mutex> lock(client_mutex);
    auto it = socket_client_map.find(sock);
    if (it != socket_client_map.end()) {
        int client_id = it->second;
        client_socket_map.erase(client_id);
        socket_client_map.erase(it);
        closesocket(sock);
        std::cout << "client disconnected, id=" << client_id << std::endl;
    }
}

void FrontGatewayServer::send_loop() {
    while (running) {
        ReadResult readresult = BackendBridgeReceiver::getInstance()->readFromShm();
        HEAD head = { 0 };
        memcpy(&head, readresult.data, sizeof(HEAD));
        int total_len = sizeof(HEAD) + head.data_len;

        std::cout << "[FrontGateway] Received response from backend: service_type=" << head.service_type
                  << " data_len=" << head.data_len << " client_fd=" << readresult.client_fd << std::endl;

        SOCKET target = INVALID_SOCKET;
        {
            std::lock_guard<std::mutex> lock(client_mutex);
            auto it = client_socket_map.find(readresult.client_fd);
            if (it != client_socket_map.end()) {
                target = it->second;
                std::cout << "[FrontGateway] Found client socket for fd=" << readresult.client_fd << std::endl;
            } else {
                std::cout << "[FrontGateway] WARNING: Client fd=" << readresult.client_fd << " not found in map!" << std::endl;
            }
        }

        if (target == INVALID_SOCKET) {
            std::cout << "[FrontGateway] Skipping send - no valid target socket" << std::endl;
            continue;
        }

        std::cout << "[FrontGateway] Sending " << total_len << " bytes to client..." << std::endl;
        if (send_all(target, readresult.data, total_len) < 0) {
            std::cerr << "[FrontGateway] ERROR: send to client failed, id=" << readresult.client_fd << std::endl;
            remove_client(target);
        } else {
            std::cout << "[FrontGateway] Successfully sent response to client fd=" << readresult.client_fd << std::endl;
        }
    }
}

void FrontGatewayServer::epoll_event_start() {
    running = true;
    sender_thread = std::thread(&FrontGatewayServer::send_loop, this);

    ThreadPoolStd pool(100);
    SOCKET listen_fd = listener->getServerfd();

    while (running) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(listen_fd, &readfds);

        std::vector<SOCKET> clients_copy;
        {
            std::lock_guard<std::mutex> lock(client_mutex);
            for (auto& kv : client_socket_map) {
                FD_SET(kv.second, &readfds);
                clients_copy.push_back(kv.second);
            }
        }

        int ready = select(0, &readfds, nullptr, nullptr, nullptr);
        if (ready == SOCKET_ERROR) {
            std::cerr << "select error: " << WSAGetLastError() << std::endl;
            continue;
        }

        if (FD_ISSET(listen_fd, &readfds)) {
            SOCKET client = accept(listen_fd, nullptr, nullptr);
            if (client != INVALID_SOCKET) {
                int client_id = next_client_id++;
                {
                    std::lock_guard<std::mutex> lock(client_mutex);
                    client_socket_map[client_id] = client;
                    socket_client_map[client] = client_id;
                }
                std::cout << "new client connected, id=" << client_id << std::endl;
            }
        }

        for (SOCKET client_sock : clients_copy) {
            if (!FD_ISSET(client_sock, &readfds)) {
                continue;
            }

            HEAD msg_head = { 0 };
            if (recv_all(client_sock, reinterpret_cast<char*>(&msg_head), sizeof(HEAD)) != sizeof(HEAD)) {
                remove_client(client_sock);
                continue;
            }

            if (msg_head.data_len < 0 || msg_head.data_len > (PACKAGESIZE - static_cast<int>(sizeof(HEAD)))) {
                std::cerr << "invalid data_len from client" << std::endl;
                remove_client(client_sock);
                continue;
            }

            int total_len = sizeof(HEAD) + msg_head.data_len;
            std::vector<char> package(total_len);
            memcpy(package.data(), &msg_head, sizeof(HEAD));

            if (msg_head.data_len > 0) {
                if (recv_all(client_sock, package.data() + sizeof(HEAD), msg_head.data_len) != msg_head.data_len) {
                    remove_client(client_sock);
                    continue;
                }
            }

            int client_id = 0;
            {
                std::lock_guard<std::mutex> lock(client_mutex);
                auto it = socket_client_map.find(client_sock);
                if (it != socket_client_map.end()) {
                    client_id = it->second;
                }
            }

            DispatchToBackendTask* task = new DispatchToBackendTask(package.data(), total_len, client_id);
            task->setDatalen(total_len);
            task->setClientfd(client_id);
            pool.add_task(task);
        }
    }
}
