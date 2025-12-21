#pragma once
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "WinSockInit.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

class SmsCodeHttpClient
{
private:
    string account;
    string password;

    string hostname_;
    int port_;
    SOCKET sockfd_;
    bool connected_;

    void connect();
    void disconnect();
    string sendHTTPPost(const string& page, const string& postData);

public:
    SmsCodeHttpClient(const string& hostname = "106.ihuyi.com", int port = 80);
    ~SmsCodeHttpClient();
    string sendSMS(const string& mobile, const string& content);
};
