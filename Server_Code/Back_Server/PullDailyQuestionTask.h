#pragma once
#include "BaseTask.h"

class PullDailyQuestionTask : public BaseTask {
public:
    PullDailyQuestionTask(char* recv_msg_package, int len, int client_fd);
    void do_service() override;
};

