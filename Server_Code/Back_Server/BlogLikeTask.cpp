#include "BlogLikeTask.h"
#include "ModelControl.h"
#include "FrontBridgeSender.h"
#include "Protocol.h"
#include <cstring>
#include <string>

BlogLikeTask::BlogLikeTask(char* recv_msg_package, int len, int client_fd)
    : BaseTask(recv_msg_package, len, client_fd) {}

void BlogLikeTask::do_service() {
    BLOG_LIKE_REQ req = {0};
    memcpy(&req, this->recv_msg_package + sizeof(HEAD), sizeof(BLOG_LIKE_REQ));

    long long blog_id = 0;
    try { blog_id = std::stoll(std::string(req.blog_id)); } catch (...) { blog_id = 0; }
    long long user_id = 0;
    try { user_id = std::stoll(std::string(req.user_id)); } catch (...) { user_id = 0; }

    int likes = 0;
    bool ok = ModelControl::getInstance()->getModel()->LikeBlog(blog_id, user_id, likes);

    HEAD resp_head = {0};
    resp_head.service_type = BLOG_LIKE;
    resp_head.data_len = sizeof(BLOG_LIKE_RESP);
    BLOG_LIKE_RESP resp_body = {0};
    resp_body.status_code = ok ? 0 : 1;
    resp_body.likes = likes;

    char resp_package[PACKAGESIZE] = {0};
    memcpy(resp_package, &resp_head, sizeof(HEAD));
    memcpy(resp_package + sizeof(HEAD), &resp_body, sizeof(BLOG_LIKE_RESP));
    FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
}

