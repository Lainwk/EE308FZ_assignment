#pragma once
#include "BaseTask.h"

class FetchReportTask : public BaseTask {
public:
    FetchReportTask(char* recv_msg_package, int len, int client_fd);
    void do_service() override;
};

