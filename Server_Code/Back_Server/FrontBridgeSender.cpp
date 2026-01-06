#include "FrontBridgeSender.h"
#include "FrontBridgeListener.h"

#include <cstring>
#include <iostream>

FrontBridgeSender* FrontBridgeSender::instance = nullptr;

FrontBridgeSender::FrontBridgeSender() {
    ensure_winsock_initialized();
}

FrontBridgeSender::~FrontBridgeSender() {}

FrontBridgeSender* FrontBridgeSender::getInstance() {
    if (instance == nullptr) {
        instance = new FrontBridgeSender();
    }
    return instance;
}

int FrontBridgeSender::sendAll(SOCKET sock, const char* data, int len) {
    int sent = 0;
    while (sent < len) {
        int res = send(sock, data + sent, len - sent, 0);
        if (res <= 0) {
            return -1;
        }
        sent += res;
    }
    return sent;
}

int FrontBridgeSender::writeToShm(const char* data, int len, int client_fd) {
    if (len <= 0 || len > PACKAGESIZE) {
        return -1;
    }

    SOCKET sock = FrontBridgeListener::getInstance()->getFrontSocket();
    if (sock == INVALID_SOCKET) {
        std::cerr << "front socket unavailable, cannot send response" << std::endl;
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

    std::lock_guard<std::mutex> lock(send_mutex);
    InterServerHeader header = {};
    header.client_id = client_fd;
    header.payload_len = payload_len;

    if (sendAll(sock, reinterpret_cast<char*>(&header), sizeof(header)) < 0) {
        return -1;
    }
    if (sendAll(sock, data, payload_len) < 0) {
        return -1;
    }

    return 0;
}
