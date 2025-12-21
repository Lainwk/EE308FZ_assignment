#include "BlogCommentListTask.h"
#include "ModelControl.h"
#include "FrontBridgeSender.h"
#include "Protocol.h"
#include <cstring>
#include <string>

BlogCommentListTask::BlogCommentListTask(char* recv_msg_package, int len, int client_fd)
    : BaseTask(recv_msg_package, len, client_fd) {}

void BlogCommentListTask::do_service() {
    BLOG_COMMENT_LIST_REQ req = {0};
    memcpy(&req, this->recv_msg_package + sizeof(HEAD), sizeof(BLOG_COMMENT_LIST_REQ));
    int limit = req.limit > 0 ? req.limit : 20;
    long long blog_id = 0;
    try { blog_id = std::stoll(std::string(req.blog_id)); } catch (...) { blog_id = 0; }

    auto comments = ModelControl::getInstance()->getModel()->GetComments(blog_id, limit);

    char resp_package[PACKAGESIZE] = {0};
    HEAD resp_head = {0};
    resp_head.service_type = BLOG_COMMENT_LIST;

    int offset = sizeof(HEAD);
    BLOG_COMMENT_LIST_RESP resp_body = {0};
    resp_body.status_code = 0;
    resp_body.comment_count = static_cast<int>(comments.size());
    if (offset + static_cast<int>(sizeof(BLOG_COMMENT_LIST_RESP)) > PACKAGESIZE) {
        return;
    }
    memcpy(resp_package + offset, &resp_body, sizeof(BLOG_COMMENT_LIST_RESP));
    offset += sizeof(BLOG_COMMENT_LIST_RESP);

    int count_written = 0;
    for (auto& c : comments) {
        std::string content = c.content;
        BLOG_COMMENT_ITEM item = {0};
        strncpy_s(item.author_name, sizeof(item.author_name), c.author_name.c_str(), _TRUNCATE);
        strncpy_s(item.created_at, sizeof(item.created_at), c.created_at.c_str(), _TRUNCATE);
        item.content_len = static_cast<int>(content.size());

        int need = static_cast<int>(sizeof(BLOG_COMMENT_ITEM) + content.size());
        if (offset + need > PACKAGESIZE) {
            break;
        }
        memcpy(resp_package + offset, &item, sizeof(BLOG_COMMENT_ITEM));
        offset += sizeof(BLOG_COMMENT_ITEM);
        if (!content.empty()) {
            memcpy(resp_package + offset, content.data(), content.size());
            offset += static_cast<int>(content.size());
        }
        count_written++;
    }

    if (count_written != resp_body.comment_count) {
        reinterpret_cast<BLOG_COMMENT_LIST_RESP*>(resp_package + sizeof(HEAD))->comment_count = count_written;
    }

    resp_head.data_len = offset - static_cast<int>(sizeof(HEAD));
    memcpy(resp_package, &resp_head, sizeof(HEAD));
    FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
}

