#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "InterServerProtocol.h"
#include "Protocol.h"
#include "WinSockInit.h"

#pragma comment(lib, "Ws2_32.lib")

// Windows edition: BackendBridgeClient now forwards data to the back server over TCP.
class BackendBridgeClient {
private:
    static BackendBridgeClient* instance;

    SOCKET backend_socket;
    std::mutex send_mutex;
    std::mutex queue_mutex;
    std::condition_variable queue_cv;
    std::queue<ReadResult> resp_queue;
    std::thread receiver_thread;
    bool stop_flag;

    BackendBridgeClient();
    bool ensureConnected();
    void receiverLoop();
    int sendAll(const char* data, int len);
    int recvAll(char* buffer, int len);

public:
    ~BackendBridgeClient();
    static BackendBridgeClient* getInstance();

    // len: bytes of HEAD+payload to forward.
    int writeToShm(const char* data, int len, int client_fd);

    // Block until a response from back server arrives.
    ReadResult waitForResponse();
};
