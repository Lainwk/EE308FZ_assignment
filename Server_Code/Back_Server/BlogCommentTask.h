#pragma once

#include "BaseTask.h"

class BlogCommentTask : public BaseTask {
public:
    BlogCommentTask(char* recv_msg_package, int len, int client_fd);
    void do_service() override;
};

