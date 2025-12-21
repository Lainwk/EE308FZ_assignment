#pragma once

#include <atomic>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "FrontListener.h"
#include "Protocol.h"
#include "ThreadPoolStd.h"
#include "BackendBridgeReceiver.h"
#include "BackendBridgeClient.h"
#include "DispatchToBackendTask.h"

class FrontGatewayServer {
public:
    explicit FrontGatewayServer(int port);
    ~FrontGatewayServer();

    void epoll_event_start();

private:
    void send_loop();
    int recv_all(SOCKET sock, char* buffer, int len);
    int send_all(SOCKET sock, const char* buffer, int len);
    void remove_client(SOCKET sock);

    int port;
    FrontListener* listener;
    std::unordered_map<int, SOCKET> client_socket_map;   // logical id -> socket
    std::unordered_map<SOCKET, int> socket_client_map;   // socket -> logical id
    std::mutex client_mutex;
    std::thread sender_thread;
    std::atomic<bool> running;
    int next_client_id;
};
