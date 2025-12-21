#pragma once

#include <iostream>
#include <mutex>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

inline void ensure_winsock_initialized() {
    static std::once_flag winsock_flag;
    std::call_once(winsock_flag, []() {
        WSADATA wsaData;
        int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (res != 0) {
            std::cerr << "WSAStartup failed: " << res << std::endl;
        }
    });
}
