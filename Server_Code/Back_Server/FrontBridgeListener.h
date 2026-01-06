#pragma once

#include <mutex>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "InterServerProtocol.h"
#include "Protocol.h"
#include "WinSockInit.h"

#pragma comment(lib, "Ws2_32.lib")

// Back server side: accept connection from front server and read messages over TCP.
class FrontBridgeListener {
private:
    static FrontBridgeListener* instance;

    SOCKET listen_socket;
    SOCKET front_socket;
    std::mutex socket_mutex;

    FrontBridgeListener();
    bool ensureListening();
    bool ensureFrontConnected();
    int recvAll(char* buffer, int len);

public:
    ~FrontBridgeListener();
    static FrontBridgeListener* getInstance();

    ReadResult readFromShm();
    SOCKET getFrontSocket();
};
