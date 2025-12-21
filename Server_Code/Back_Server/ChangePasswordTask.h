#pragma once
#include "BaseTask.h"

class ChangePasswordTask : public BaseTask {
public:
    ChangePasswordTask(char* recv_msg_package, int len, int client_fd);
    void do_service();
};