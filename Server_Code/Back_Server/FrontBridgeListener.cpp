#include "FrontBridgeListener.h"

#include <chrono>
#include <iostream>
#include <thread>

FrontBridgeListener* FrontBridgeListener::instance = nullptr;

FrontBridgeListener::FrontBridgeListener() : listen_socket(INVALID_SOCKET), front_socket(INVALID_SOCKET) {
    ensure_winsock_initialized();
    ensureListening();
}

FrontBridgeListener::~FrontBridgeListener() {
    if (front_socket != INVALID_SOCKET) {
        closesocket(front_socket);
        front_socket = INVALID_SOCKET;
    }
    if (listen_socket != INVALID_SOCKET) {
        closesocket(listen_socket);
        listen_socket = INVALID_SOCKET;
    }
}

FrontBridgeListener* FrontBridgeListener::getInstance() {
    if (instance == nullptr) {
        instance = new FrontBridgeListener();
    }
    return instance;
}

bool FrontBridgeListener::ensureListening() {
    if (listen_socket != INVALID_SOCKET) {
        return true;
    }

    listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket == INVALID_SOCKET) {
        std::cerr << "create listen socket failed: " << WSAGetLastError() << std::endl;
        return false;
    }

    BOOL opt = TRUE;
    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(BACK_SERVER_PORT);

    if (bind(listen_socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "bind listen socket failed: " << WSAGetLastError() << std::endl;
        closesocket(listen_socket);
        listen_socket = INVALID_SOCKET;
        return false;
    }

    if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen socket failed: " << WSAGetLastError() << std::endl;
        closesocket(listen_socket);
        listen_socket = INVALID_SOCKET;
        return false;
    }

    std::cout << "Back server listening on port " << BACK_SERVER_PORT << std::endl;
    return true;
}

bool FrontBridgeListener::ensureFrontConnected() {
    std::lock_guard<std::mutex> lock(socket_mutex);
    if (front_socket != INVALID_SOCKET) {
        return true;
    }

    if (!ensureListening()) {
        return false;
    }

    std::cout << "Waiting for front server connection..." << std::endl;
    front_socket = accept(listen_socket, nullptr, nullptr);
    if (front_socket == INVALID_SOCKET) {
        std::cerr << "accept front server failed: " << WSAGetLastError() << std::endl;
        return false;
    }
    std::cout << "Front server connected." << std::endl;
    return true;
}

int FrontBridgeListener::recvAll(char* buffer, int len) {
    int received = 0;
    while (received < len) {
        int res = recv(front_socket, buffer + received, len - received, 0);
        if (res <= 0) {
            return -1;
        }
        received += res;
    }
    return received;
}

ReadResult FrontBridgeListener::readFromShm() {
    ReadResult result = {};

    if (!ensureFrontConnected()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        return result;
    }

    InterServerHeader header = {};
    if (recvAll(reinterpret_cast<char*>(&header), sizeof(header)) != sizeof(header)) {
        std::cerr << "lost connection to front server while reading header" << std::endl;
        closesocket(front_socket);
        front_socket = INVALID_SOCKET;
        return result;
    }

    if (header.payload_len <= 0 || header.payload_len > PACKAGESIZE) {
        std::cerr << "invalid payload size from front server: " << header.payload_len << std::endl;
        closesocket(front_socket);
        front_socket = INVALID_SOCKET;
        return result;
    }

    if (recvAll(result.data, header.payload_len) != header.payload_len) {
        std::cerr << "lost connection to front server while reading body" << std::endl;
        closesocket(front_socket);
        front_socket = INVALID_SOCKET;
        return result;
    }

    result.index = 0;
    result.client_fd = header.client_id;
    return result;
}

SOCKET FrontBridgeListener::getFrontSocket() {
    std::lock_guard<std::mutex> lock(socket_mutex);
    return front_socket;
}
