#pragma once
#include "BaseTask.h"

class SubmitAnswerTask : public BaseTask {
public:
    SubmitAnswerTask(char* recv_msg_package, int len, int client_fd);
    void do_service() override;
};

