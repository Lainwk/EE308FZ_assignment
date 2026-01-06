#pragma once

#include <mutex>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "InterServerProtocol.h"
#include "Protocol.h"
#include "WinSockInit.h"

#pragma comment(lib, "Ws2_32.lib")

class FrontBridgeSender {
private:
    static FrontBridgeSender* instance;

    std::mutex send_mutex;

    FrontBridgeSender();
    int sendAll(SOCKET sock, const char* data, int len);

public:
    ~FrontBridgeSender();
    static FrontBridgeSender* getInstance();

    int writeToShm(const char* data, int len, int client_fd);
};
