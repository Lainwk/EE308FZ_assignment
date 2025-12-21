#pragma once

#include "BaseTask.h"

class BlogLikeTask : public BaseTask {
public:
    BlogLikeTask(char* recv_msg_package, int len, int client_fd);
    void do_service() override;
};

