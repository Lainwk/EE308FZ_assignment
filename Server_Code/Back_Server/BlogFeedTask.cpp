#include "BlogFeedTask.h"
#include "ModelControl.h"
#include "FrontBridgeSender.h"
#include "Protocol.h"
#include <cstring>
#include <string>

BlogFeedTask::BlogFeedTask(char* recv_msg_package, int len, int client_fd)
    : BaseTask(recv_msg_package, len, client_fd) {}

void BlogFeedTask::do_service() {
    BLOG_FEED_REQ req = {0};
    memcpy(&req, this->recv_msg_package + sizeof(HEAD), sizeof(BLOG_FEED_REQ));
    int limit = req.limit > 0 ? req.limit : 50;

    std::vector<BaseModel::BlogItem> blogs;
    try {
        blogs = ModelControl::getInstance()->getModel()->GetBlogs(limit);
    } catch (const std::exception& e) {
        std::cerr << "[BlogFeedTask] Error: " << e.what() << std::endl;
        blogs.clear();
    } catch (...) {
        std::cerr << "[BlogFeedTask] Unknown error" << std::endl;
        blogs.clear();
    }

    char resp_package[PACKAGESIZE] = {0};
    HEAD resp_head = {0};
    resp_head.service_type = BLOG_FEED;

    int offset = sizeof(HEAD);
    BLOG_FEED_RESP resp_body = {0};
    resp_body.status_code = 0;
    resp_body.blog_count = static_cast<int>(blogs.size());
    
    if (offset + static_cast<int>(sizeof(BLOG_FEED_RESP)) > PACKAGESIZE) {
        resp_body.blog_count = 0;
        resp_head.data_len = sizeof(BLOG_FEED_RESP);
        memcpy(resp_package, &resp_head, sizeof(HEAD));
        memcpy(resp_package + sizeof(HEAD), &resp_body, sizeof(BLOG_FEED_RESP));
        FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
        return;
    }
    memcpy(resp_package + offset, &resp_body, sizeof(BLOG_FEED_RESP));
    offset += sizeof(BLOG_FEED_RESP);

    int count_written = 0;
    for (auto& b : blogs) {
        std::string content = b.content;
        BLOG_ITEM item = {0};
        std::string blog_id = std::to_string(b.id);
        strncpy_s(item.blog_id, sizeof(item.blog_id), blog_id.c_str(), _TRUNCATE);
        std::string author_id = std::to_string(b.user_id);
        strncpy_s(item.author_id, sizeof(item.author_id), author_id.c_str(), _TRUNCATE);
        strncpy_s(item.author_name, sizeof(item.author_name), b.author_name.c_str(), _TRUNCATE);
        item.is_anonymous = b.is_anonymous;
        item.likes = b.likes;
        item.comments = b.comments;
        strncpy_s(item.created_at, sizeof(item.created_at), b.created_at.c_str(), _TRUNCATE);
        item.content_len = static_cast<int>(content.size());

        int need = static_cast<int>(sizeof(BLOG_ITEM) + content.size());
        if (offset + need > PACKAGESIZE) {
            break;
        }
        memcpy(resp_package + offset, &item, sizeof(BLOG_ITEM));
        offset += sizeof(BLOG_ITEM);
        if (!content.empty()) {
            memcpy(resp_package + offset, content.data(), content.size());
            offset += static_cast<int>(content.size());
        }
        count_written++;
    }

    // 修正实际写入数量
    if (count_written != resp_body.blog_count) {
        reinterpret_cast<BLOG_FEED_RESP*>(resp_package + sizeof(HEAD))->blog_count = count_written;
    }

    resp_head.data_len = offset - static_cast<int>(sizeof(HEAD));
    memcpy(resp_package, &resp_head, sizeof(HEAD));
    
    FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
}
