#pragma once
#include "BaseTask.h"

class SubmitAssessmentTask : public BaseTask {
public:
    SubmitAssessmentTask(char* recv_msg_package, int len, int client_fd);
    void do_service() override;
};