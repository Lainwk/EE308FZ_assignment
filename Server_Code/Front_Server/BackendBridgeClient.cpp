#include "BackendBridgeClient.h"
#include "WinSockInit.h"

#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>

BackendBridgeClient* BackendBridgeClient::instance = nullptr;

BackendBridgeClient::BackendBridgeClient() : backend_socket(INVALID_SOCKET), stop_flag(false) {
    ensure_winsock_initialized();
    ensureConnected();
    receiver_thread = std::thread(&BackendBridgeClient::receiverLoop, this);
}

BackendBridgeClient::~BackendBridgeClient() {
    stop_flag = true;
    queue_cv.notify_all();

    if (receiver_thread.joinable()) {
        receiver_thread.join();
    }

    if (backend_socket != INVALID_SOCKET) {
        closesocket(backend_socket);
        backend_socket = INVALID_SOCKET;
    }
}

BackendBridgeClient* BackendBridgeClient::getInstance() {
    if (instance == nullptr) {
        instance = new BackendBridgeClient();
    }
    return instance;
}

bool BackendBridgeClient::ensureConnected() {
    std::lock_guard<std::mutex> lock(send_mutex);
    if (backend_socket != INVALID_SOCKET) {
        return true;
    }

    backend_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (backend_socket == INVALID_SOCKET) {
        std::cerr << "create backend socket failed: " << WSAGetLastError() << std::endl;
        return false;
    }

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(BACK_SERVER_PORT);
    inet_pton(AF_INET, BACK_SERVER_HOST, &addr.sin_addr);

    if (connect(backend_socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "connect backend failed: " << WSAGetLastError() << std::endl;
        closesocket(backend_socket);
        backend_socket = INVALID_SOCKET;
        return false;
    }

    return true;
}

int BackendBridgeClient::sendAll(const char* data, int len) {
    int sent = 0;
    while (sent < len) {
        int res = send(backend_socket, data + sent, len - sent, 0);
        if (res <= 0) {
            return -1;
        }
        sent += res;
    }
    return sent;
}

int BackendBridgeClient::recvAll(char* buffer, int len) {
    int received = 0;
    while (received < len) {
        int res = recv(backend_socket, buffer + received, len - received, 0);
        if (res <= 0) {
            return -1;
        }
        received += res;
    }
    return received;
}

int BackendBridgeClient::writeToShm(const char* data, int len, int client_fd) {
    if (len <= 0 || len > PACKAGESIZE) {
        return -1;
    }

    if (!ensureConnected()) {
        return -1;
    }

    int payload_len = len;
    if (len >= static_cast<int>(sizeof(HEAD))) {
        HEAD head = { 0 };
        memcpy(&head, data, sizeof(HEAD));
        payload_len = static_cast<int>(sizeof(HEAD) + head.data_len);
        if (payload_len > PACKAGESIZE) {
            payload_len = PACKAGESIZE;
        }
    }

    InterServerHeader header = {};
    header.client_id = client_fd;
    header.payload_len = payload_len;

    if (sendAll(reinterpret_cast<char*>(&header), sizeof(header)) < 0) {
        closesocket(backend_socket);
        backend_socket = INVALID_SOCKET;
        return -1;
    }
    if (sendAll(data, payload_len) < 0) {
        closesocket(backend_socket);
        backend_socket = INVALID_SOCKET;
        return -1;
    }
    return 0;
}

void BackendBridgeClient::receiverLoop() {
    while (!stop_flag) {
        if (!ensureConnected()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        InterServerHeader header = {};
        if (recvAll(reinterpret_cast<char*>(&header), sizeof(header)) != sizeof(header)) {
            std::cerr << "backend socket closed while reading header" << std::endl;
            closesocket(backend_socket);
            backend_socket = INVALID_SOCKET;
            continue;
        }

        if (header.payload_len <= 0 || header.payload_len > PACKAGESIZE) {
            std::cerr << "invalid payload size from backend: " << header.payload_len << std::endl;
            closesocket(backend_socket);
            backend_socket = INVALID_SOCKET;
            continue;
        }

        ReadResult result = {};
        result.index = 0;
        result.client_fd = header.client_id;

        if (recvAll(result.data, header.payload_len) != header.payload_len) {
            std::cerr << "backend socket closed while reading body" << std::endl;
            closesocket(backend_socket);
            backend_socket = INVALID_SOCKET;
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            resp_queue.push(result);
        }
        queue_cv.notify_one();
    }
}

ReadResult BackendBridgeClient::waitForResponse() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    queue_cv.wait(lock, [this]() { return stop_flag || !resp_queue.empty(); });

    if (resp_queue.empty()) {
        return ReadResult{};
    }

    ReadResult result = resp_queue.front();
    resp_queue.pop();
    return result;
}
