#pragma once

#include "BaseTask.h"

class BlogCommentListTask : public BaseTask {
public:
    BlogCommentListTask(char* recv_msg_package, int len, int client_fd);
    void do_service() override;
};

