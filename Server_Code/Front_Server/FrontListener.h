#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

class FrontListener {
public:
    explicit FrontListener(int port);
    ~FrontListener();

    int getPort() const;
    SOCKET getServerfd() const;

private:
    void Server_start();

    int port;
    SOCKET server_fd;
};
