#pragma once
#include "BaseTask.h"

class AssessmentHistoryTask : public BaseTask {
public:
    AssessmentHistoryTask(char* recv_msg_package, int len, int client_fd);
    void do_service() override;
};