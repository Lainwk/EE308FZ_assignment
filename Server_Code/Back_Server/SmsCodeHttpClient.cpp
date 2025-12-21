#include "SmsCodeHttpClient.h"

#include <sstream>
#include <iomanip>

SmsCodeHttpClient::SmsCodeHttpClient(const string& hostname, int port)
    : hostname_(hostname), port_(port), sockfd_(INVALID_SOCKET), connected_(false) {
    ensure_winsock_initialized();
    this->account = "C36072882";
    this->password = "ae875d93dd242de8946660941c82e6b3";
}

SmsCodeHttpClient::~SmsCodeHttpClient() {
    this->disconnect();
}

void SmsCodeHttpClient::connect() {
    if (connected_) return;

    struct addrinfo hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* result = nullptr;
    std::string portStr = std::to_string(port_);
    int res = getaddrinfo(hostname_.c_str(), portStr.c_str(), &hints, &result);
    if (res != 0) {
        throw runtime_error("getaddrinfo failed");
    }

    sockfd_ = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sockfd_ == INVALID_SOCKET) {
        freeaddrinfo(result);
        throw runtime_error("Error opening socket");
    }

    if (::connect(sockfd_, result->ai_addr, static_cast<int>(result->ai_addrlen)) == SOCKET_ERROR) {
        freeaddrinfo(result);
        closesocket(sockfd_);
        sockfd_ = INVALID_SOCKET;
        throw runtime_error("Error connecting");
    }

    freeaddrinfo(result);
    connected_ = true;
}

void SmsCodeHttpClient::disconnect() {
    if (connected_) {
        closesocket(sockfd_);
        sockfd_ = INVALID_SOCKET;
        connected_ = false;
    }
}

string SmsCodeHttpClient::sendHTTPPost(const string& page, const string& postData) {
    if (!connected_) {
        throw runtime_error("Not connected to server");
    }

    string request = "POST " + page + " HTTP/1.1\r\n";
    request += "Host: " + hostname_ + "\r\n";
    request += "Content-Type: application/x-www-form-urlencoded\r\n";
    request += "Content-Length: " + to_string(postData.length()) + "\r\n";
    request += "Connection: close\r\n\r\n";
    request += postData;

    int bytesWritten = send(sockfd_, request.c_str(), static_cast<int>(request.length()), 0);
    if (bytesWritten < 0) {
        throw runtime_error("Error writing to socket");
    }

    char buffer[4096];
    string response;
    int bytesRead;
    while ((bytesRead = recv(sockfd_, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesRead] = '\0';
        response += buffer;
    }

    if (bytesRead < 0) {
        throw runtime_error("Error reading from socket");
    }

    return response;
}

string SmsCodeHttpClient::sendSMS(const string& mobile, const string& content) {
    try {
        connect();
        // Percent-encode for form body to support UTF-8 content
        auto enc = [](const string& v) {
            std::ostringstream escaped;
            escaped << std::hex << std::uppercase;
            for (unsigned char c : v) {
                if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                    escaped << c;
                } else if (c == ' ') {
                    escaped << '+';
                } else {
                    escaped << '%' << std::setw(2) << std::setfill('0') << int(c);
                }
            }
            return escaped.str();
        };

        string postData = "account=" + enc(this->account) + "&password=" + enc(this->password) +
            "&mobile=" + enc(mobile) + "&content=" + enc(content);
        string response = sendHTTPPost("/webservice/sms.php?method=Submit&format=json", postData);
        disconnect();
        return response;
    }
    catch (const exception&) {
        disconnect();
        throw;
    }
}
