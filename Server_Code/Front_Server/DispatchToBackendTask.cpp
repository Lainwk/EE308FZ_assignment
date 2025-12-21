#include "DispatchToBackendTask.h"
#include "Protocol.h"
#include <iostream>

namespace {
bool is_valid_service_type(int type) {
    switch (type) {
    case SMS_CODE:
    case LOGIN:
    case REGISTER:
    case BLOG:
    case MESSAGE:
    case PULL_QUESTIONNAIRE:
    case SUBMIT_ASSESSMENT:
    case FETCH_DETAILED_REPORT:
    case FRIEND_APPLY:
    case FRIEND_LIST:
    case FRIEND_PENDING:
    case FRIEND_ACCEPT:
    case MESSAGE_PULL:
    case BLOG_FEED:
    case BLOG_LIKE:
    case BLOG_COMMENT:
    case BLOG_COMMENT_LIST:
    case ASSESSMENT_HISTORY:        // 21 - 测评历史记录
    case CHECK_TODAY_ASSESSMENT:    // 22 - 检查今日测评状态
    case AI_CHAT:                   // 23 - AI对话功能
    case CHANGE_PASSWORD:
        return true;
    default:
        return false;
    }
}
} // namespace

DispatchToBackendTask::DispatchToBackendTask(char* recv_msg_package, int len, int client_fd)
    : BaseTask(recv_msg_package, len, client_fd) {
}

void DispatchToBackendTask::do_service()
{
    std::cout << "Data Check Task running" << std::endl;
    HEAD recv_head = { 0 };
    memcpy(&recv_head, this->recv_msg_package, sizeof(HEAD));
    std::cout << "SERVER ACCEPT MSG FROM CLIENT" << std::endl;
    std::cout << "MESSAGE TYPE:" << recv_head.service_type << std::endl;

    if (!is_valid_service_type(recv_head.service_type)) {
        std::cout << "Unknown service type, drop packet: " << recv_head.service_type << std::endl;
        return;
    }

    int res = BackendBridgeClient::getInstance()->writeToShm(this->recv_msg_package, this->datalen, this->client_fd);
    if (res == -1)
    {
        std::cout << "backend send fail" << std::endl;
    }
    else
    {
        std::cout << "backend send success" << std::endl;
    }
}

void DispatchToBackendTask::setDatalen(int datalen)
{
    this->datalen = datalen;
}

void DispatchToBackendTask::setClientfd(int clientfd)
{
    client_fd = clientfd;
}
