#pragma once
#include "BaseTask.h"

class RegisterTask : public BaseTask {
public:
    RegisterTask(char* recv_msg_package, int len, int client_fd);
    void do_service() override;
};

