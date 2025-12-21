#pragma once

#include <cstring>
#include "Protocol.h"
#include "FrontBridgeSender.h"

class BaseTask {
protected:
    char* recv_msg_package;
    int client_fd;
    int package_len;
public:
    BaseTask(char* recv_msg_package, int len, int client_fd);
    virtual ~BaseTask();
    virtual void do_service() = 0;
};
