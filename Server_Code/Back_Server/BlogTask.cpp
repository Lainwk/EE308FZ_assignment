#include "BlogTask.h"
#include "FrontBridgeSender.h"
#include "ModelControl.h"

#include <cstring>
#include <string>

BlogTask::BlogTask(char* recv_msg_package, int len, int client_fd)
    : BaseTask(recv_msg_package, len, client_fd) {}

void BlogTask::do_service() {
    BLOG_REQ req = {0};
    memcpy(&req, this->recv_msg_package + sizeof(HEAD), sizeof(BLOG_REQ));

    int content_offset = sizeof(HEAD) + sizeof(BLOG_REQ);
    int available = this->package_len - content_offset;
    std::string content;
    if (req.content_len > 0 && req.content_len <= available) {
        content.assign(this->recv_msg_package + content_offset, req.content_len);
    }

    HEAD resp_head = {0};
    BLOG_RESP resp_body = {0};
    resp_head.service_type = BLOG;

    long long user_id = 0;
    try {
        user_id = std::stoll(std::string(req.user_id));
    } catch (...) {
        user_id = 0;
    }

    std::string blog_id;
    bool ok = ModelControl::getInstance()->getModel()->InsertBlog(user_id, req.is_anonymous != 0, content, blog_id);
    resp_body.status_code = ok ? 0 : 1;
    std::cout << "[BlogTask] insert blog user=" << user_id << " len=" << content.size() << " ok=" << ok << std::endl;
    if (ok) {
        strncpy_s(resp_body.blog_id, sizeof(resp_body.blog_id), blog_id.c_str(), _TRUNCATE);
    }

    resp_head.data_len = sizeof(BLOG_RESP);
    char resp_package[PACKAGESIZE] = {0};
    std::memcpy(resp_package, &resp_head, sizeof(HEAD));
    std::memcpy(resp_package + sizeof(HEAD), &resp_body, sizeof(BLOG_RESP));
    FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
}
