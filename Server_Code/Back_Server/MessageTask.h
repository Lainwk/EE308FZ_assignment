#pragma once
#include "BaseTask.h"

class MessageTask : public BaseTask {
public:
    MessageTask(char* recv_msg_package, int len, int client_fd);
    void do_service() override;
};

