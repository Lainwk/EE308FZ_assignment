#pragma once
#include "BaseTask.h"

class CheckTodayAssessmentTask : public BaseTask {
public:
    CheckTodayAssessmentTask(char* recv_msg_package, int len, int client_fd);
    void do_service() override;
};