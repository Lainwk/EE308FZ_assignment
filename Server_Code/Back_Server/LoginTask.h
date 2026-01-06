#pragma once
#include "BaseTask.h"

class LoginTask : public BaseTask {
public:
    LoginTask(char* recv_msg_package, int len, int client_fd);
    void do_service();
};

