#pragma once
#include "BaseTask.h"

class BlogTask : public BaseTask {
public:
    BlogTask(char* recv_msg_package, int len, int client_fd);
    void do_service() override;
};

