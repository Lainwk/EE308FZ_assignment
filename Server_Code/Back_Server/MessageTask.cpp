#include "MessageTask.h"
#include "FrontBridgeSender.h"
#include "ModelControl.h"

#include <cstring>
#include <string>

MessageTask::MessageTask(char* recv_msg_package, int len, int client_fd)
    : BaseTask(recv_msg_package, len, client_fd) {}

void MessageTask::do_service() {
    MESSAGE_REQ req = {0};
    memcpy(&req, this->recv_msg_package + sizeof(HEAD), sizeof(MESSAGE_REQ));

    int content_offset = sizeof(HEAD) + sizeof(MESSAGE_REQ);
    int available = this->package_len - content_offset;
    std::string content;
    if (req.content_len > 0 && req.content_len <= available) {
        content.assign(this->recv_msg_package + content_offset, req.content_len);
    }

    HEAD resp_head = {0};
    MESSAGE_RESP resp_body = {0};
    resp_head.service_type = MESSAGE;

    long long sender_id = 0;
    long long receiver_id = 0;
    try {
        sender_id = std::stoll(std::string(req.sender_id));
        receiver_id = std::stoll(std::string(req.receiver_id));
    } catch (...) {
        sender_id = 0;
        receiver_id = 0;
    }

    bool ok = ModelControl::getInstance()->getModel()->InsertMessage(sender_id, receiver_id, content);
    resp_body.status_code = ok ? 0 : 1;

    resp_head.data_len = sizeof(MESSAGE_RESP);
    char resp_package[PACKAGESIZE] = {0};
    memcpy(resp_package, &resp_head, sizeof(HEAD));
    memcpy(resp_package + sizeof(HEAD), &resp_body, sizeof(MESSAGE_RESP));
    FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
}

