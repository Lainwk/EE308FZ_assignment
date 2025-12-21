#include "BlogCommentTask.h"
#include "ModelControl.h"
#include "FrontBridgeSender.h"
#include "Protocol.h"
#include <cstring>
#include <string>

BlogCommentTask::BlogCommentTask(char* recv_msg_package, int len, int client_fd)
    : BaseTask(recv_msg_package, len, client_fd) {}

void BlogCommentTask::do_service() {
    BLOG_COMMENT_REQ req = {0};
    memcpy(&req, this->recv_msg_package + sizeof(HEAD), sizeof(BLOG_COMMENT_REQ));

    int body_offset = sizeof(HEAD) + sizeof(BLOG_COMMENT_REQ);
    int available = this->package_len - body_offset;
    std::string content;
    if (req.content_len > 0 && req.content_len <= available) {
        content.assign(this->recv_msg_package + body_offset, req.content_len);
    }

    long long blog_id = 0;
    try { blog_id = std::stoll(std::string(req.blog_id)); } catch (...) { blog_id = 0; }
    long long user_id = 0;
    try { user_id = std::stoll(std::string(req.user_id)); } catch (...) { user_id = 0; }

    int comments = 0;
    bool ok = ModelControl::getInstance()->getModel()->AddComment(
        blog_id, user_id, std::string(req.user_name), content, comments);

    HEAD resp_head = {0};
    resp_head.service_type = BLOG_COMMENT;
    resp_head.data_len = sizeof(BLOG_COMMENT_RESP);
    BLOG_COMMENT_RESP resp_body = {0};
    resp_body.status_code = ok ? 0 : 1;
    resp_body.comments = comments;

    char resp_package[PACKAGESIZE] = {0};
    memcpy(resp_package, &resp_head, sizeof(HEAD));
    memcpy(resp_package + sizeof(HEAD), &resp_body, sizeof(BLOG_COMMENT_RESP));
    FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
}

