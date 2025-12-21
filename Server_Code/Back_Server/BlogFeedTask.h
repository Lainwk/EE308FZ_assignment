#pragma once

#include "BaseTask.h"

class BlogFeedTask : public BaseTask {
public:
    BlogFeedTask(char* recv_msg_package, int len, int client_fd);
    void do_service() override;
};

