#pragma once
#include "BaseTask.h"

class AlertAckTask : public BaseTask {
public:
    AlertAckTask(char* recv_msg_package, int len, int client_fd);
    void do_service() override;
};

