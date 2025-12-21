#include "FrontListener.h"
#include "WinSockInit.h"

#include <iostream>

FrontListener::FrontListener(int port) : port(port), server_fd(INVALID_SOCKET) {
    ensure_winsock_initialized();
    this->Server_start();
}

FrontListener::~FrontListener() {
    if (server_fd != INVALID_SOCKET) {
        closesocket(server_fd);
        server_fd = INVALID_SOCKET;
    }
}

void FrontListener::Server_start() {
    std::cout << "start server" << std::endl;
    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "socket creation failed: " << WSAGetLastError() << std::endl;
        return;
    }

    BOOL opt = TRUE;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(this->port);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "bind fail: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        server_fd = INVALID_SOCKET;
        return;
    }
    std::cout << "open bind success" << std::endl;

    if (listen(server_fd, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen fail: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        server_fd = INVALID_SOCKET;
        return;
    }
    std::cout << "open listen success" << std::endl;
    std::cout << "ready to connect client" << std::endl;
}

int FrontListener::getPort() const {
    return port;
}

SOCKET FrontListener::getServerfd() const {
    return server_fd;
}
