#pragma once

#include <cstring>

class BaseTask {
protected:
    char* recv_msg_package;
    int client_fd;
public:
    BaseTask(char* recv_msg_package, int len, int client_fd);
    virtual ~BaseTask();
    virtual void do_service() = 0;
};
