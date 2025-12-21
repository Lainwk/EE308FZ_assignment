#include "BaseTask.h"

BaseTask::BaseTask(char* recv_msg_package, int len, int client_fd) {
    this->recv_msg_package = new char[len];
    memcpy(this->recv_msg_package, recv_msg_package, len);
    this->client_fd = client_fd;
}

BaseTask::~BaseTask()
{
    delete[] this->recv_msg_package;
}
