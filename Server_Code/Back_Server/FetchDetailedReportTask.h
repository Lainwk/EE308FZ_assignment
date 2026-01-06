#pragma once
#include "BaseTask.h"

class FetchDetailedReportTask : public BaseTask {
public:
    FetchDetailedReportTask(char* recv_msg_package, int len, int client_fd);
    void do_service() override;
};