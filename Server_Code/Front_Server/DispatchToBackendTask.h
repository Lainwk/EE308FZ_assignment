#pragma once
#include "BaseTask.h"
#include "BackendBridgeClient.h"

class DispatchToBackendTask : public BaseTask
{
public:
    DispatchToBackendTask(char* recv_msg_package, int len, int client_fd);
    void do_service();
    void setDatalen(int datalen);
    void setClientfd(int clientfd);
private:
    int datalen;
    int client_fd;
};
