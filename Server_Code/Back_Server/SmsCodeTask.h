#pragma once
#include "BaseTask.h"

class SmsCodeTask : public BaseTask {
public:
    SmsCodeTask(char* recv_msg_package, int len, int client_fd);
    void do_service() override;
};

