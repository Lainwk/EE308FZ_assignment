#pragma once
#include "BaseTask.h"

class PullQuestionnaireTask : public BaseTask {
public:
    PullQuestionnaireTask(char* recv_msg_package, int len, int client_fd);
    void do_service() override;
};