#include "BackendDispatcher.h"

#include <iostream>
#include <thread>
#include <cstring>
#include "ModelControl.h"
#include "FrontBridgeSender.h"
#include "BlogFeedTask.h"
#include "BlogLikeTask.h"
#include "BlogCommentTask.h"
#include "BlogCommentListTask.h"
#include "AssessmentHistoryTask.h"
#include "CheckTodayAssessmentTask.h"
#include "AIChatTask.h"

BackendDispatcher::BackendDispatcher()
{
    this->isRunning = false;
    this->threadPool = new ThreadPoolStd(100);
    this->reader = FrontBridgeListener::getInstance();
}

BackendDispatcher::~BackendDispatcher()
{
    stop();
    delete threadPool;
}

void BackendDispatcher::run()
{
    isRunning = true;
    std::cout << "Server receiver started. Waiting for front server messages..." << std::endl;
    while (isRunning) {
        ReadResult readResult = reader->readFromShm();

        if (readResult.client_fd != 0) {
            std::cout << "new task received" << std::endl;
            processMessage(readResult);
        }
    }
}

void BackendDispatcher::stop()
{
    isRunning = false;
}

void BackendDispatcher::processMessage(const ReadResult& readResult)
{
    HEAD recv_head = { 0 };
    memcpy(&recv_head, readResult.data, sizeof(HEAD));

    if (recv_head.data_len < 0 || recv_head.data_len > (PACKAGESIZE - static_cast<int>(sizeof(HEAD)))) {
        std::cout << "Drop packet: invalid data_len=" << recv_head.data_len << std::endl;
        return;
    }

    BaseTask* task = nullptr;
    std::cout << "Processing service type: " << recv_head.service_type << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
    char* data = const_cast<char*>(readResult.data);
    int total_len = static_cast<int>(sizeof(HEAD) + recv_head.data_len);
    if (total_len > PACKAGESIZE) {
        total_len = PACKAGESIZE;
    }
    switch (recv_head.service_type) {
    case SMS_CODE:
        task = new SmsCodeTask(data, total_len, readResult.client_fd);
        break;
    case LOGIN:
        task = new LoginTask(data, total_len, readResult.client_fd);
        break;
    case REGISTER:
        task = new RegisterTask(data, total_len, readResult.client_fd);
        break;
    case BLOG:
        task = new BlogTask(data, total_len, readResult.client_fd);
        break;
    case BLOG_FEED:
        task = new BlogFeedTask(data, total_len, readResult.client_fd);
        break;
    case BLOG_LIKE:
        task = new BlogLikeTask(data, total_len, readResult.client_fd);
        break;
    case BLOG_COMMENT:
        task = new BlogCommentTask(data, total_len, readResult.client_fd);
        break;
    case BLOG_COMMENT_LIST:
        task = new BlogCommentListTask(data, total_len, readResult.client_fd);
        break;
    case MESSAGE:
        task = new MessageTask(data, total_len, readResult.client_fd);
        break;
    case PULL_DAILY_QUESTION:
        task = new PullDailyQuestionTask(data, total_len, readResult.client_fd);
        break;
    case SUBMIT_ANSWER:
        task = new SubmitAnswerTask(data, total_len, readResult.client_fd);
        break;
    case FETCH_REPORT:
        task = new FetchReportTask(data, total_len, readResult.client_fd);
        break;
    case ALERT_ACK:
        task = new AlertAckTask(data, total_len, readResult.client_fd);
        break;
    case PULL_QUESTIONNAIRE:
        task = new PullQuestionnaireTask(data, total_len, readResult.client_fd);
        break;
    case SUBMIT_ASSESSMENT:
        task = new SubmitAssessmentTask(data, total_len, readResult.client_fd);
        break;
    case FETCH_DETAILED_REPORT:
        task = new FetchDetailedReportTask(data, total_len, readResult.client_fd);
        break;
    case ASSESSMENT_HISTORY:
        task = new AssessmentHistoryTask(data, total_len, readResult.client_fd);
        break;
    case CHECK_TODAY_ASSESSMENT:
        task = new CheckTodayAssessmentTask(data, total_len, readResult.client_fd);
        break;
    case AI_CHAT:
        task = new AIChatTask(data, total_len, readResult.client_fd);
        break;
    case CHANGE_PASSWORD:
        task = new ChangePasswordTask(data, total_len, readResult.client_fd);
        break;
    case FRIEND_APPLY:
    {
        FRIEND_APPLY_RESP resp{};
        const auto* req = reinterpret_cast<const FRIEND_APPLY_REQ*>(data + sizeof(HEAD));
        std::string from = std::string(req->from_user);
        std::string to = std::string(req->to_user);
        std::string hello = std::string(req->hello);
        bool ok = ModelControl::getInstance()->getModel()->StoreFriendApply(from, to, hello);
        resp.status_code = ok ? 0 : 1;
        HEAD head{ FRIEND_APPLY, static_cast<int>(sizeof(resp)) };
        char buf[PACKAGESIZE]{};
        memcpy(buf, &head, sizeof(head));
        memcpy(buf + sizeof(head), &resp, sizeof(resp));
        FrontBridgeSender::getInstance()->writeToShm(buf, static_cast<int>(sizeof(head) + sizeof(resp)), readResult.client_fd);
        return;
    }
    case FRIEND_LIST:
    {
        FRIEND_LIST_RESP resp{};
        std::string user_id;
        if (recv_head.data_len >= 1) {
            const char* body_ptr = reinterpret_cast<const char*>(data + sizeof(HEAD));
            user_id.assign(body_ptr, strnlen(body_ptr, recv_head.data_len));
        }
        auto list = ModelControl::getInstance()->getModel()->GetFriendList(user_id);
        resp.status_code = 0;
        resp.friend_count = static_cast<int>(list.size());
        int body_len = sizeof(resp) + static_cast<int>(list.size() * sizeof(FRIEND_ITEM));
        HEAD head{ FRIEND_LIST, body_len };
        char buf[PACKAGESIZE]{};
        memcpy(buf, &head, sizeof(head));
        memcpy(buf + sizeof(head), &resp, sizeof(resp));
        if (!list.empty()) {
            memcpy(buf + sizeof(head) + sizeof(resp), list.data(), list.size() * sizeof(FRIEND_ITEM));
        }
        FrontBridgeSender::getInstance()->writeToShm(buf, static_cast<int>(sizeof(head) + body_len), readResult.client_fd);
        return;
    }
    case FRIEND_PENDING:
    {
        FRIEND_PENDING_RESP resp{};
        std::string user_id;
        if (recv_head.data_len >= 1) {
            const char* body_ptr = reinterpret_cast<const char*>(data + sizeof(HEAD));
            user_id.assign(body_ptr, strnlen(body_ptr, recv_head.data_len));
        }
        auto list = ModelControl::getInstance()->getModel()->GetPendingFriendRequests(user_id);
        resp.status_code = 0;
        resp.request_count = static_cast<int>(list.size());
        int body_len = sizeof(resp) + static_cast<int>(list.size() * sizeof(FRIEND_REQUEST_ITEM));
        HEAD head{ FRIEND_PENDING, body_len };
        char buf[PACKAGESIZE]{};
        memcpy(buf, &head, sizeof(head));
        memcpy(buf + sizeof(head), &resp, sizeof(resp));
        if (!list.empty()) {
            memcpy(buf + sizeof(head) + sizeof(resp), list.data(), list.size() * sizeof(FRIEND_REQUEST_ITEM));
        }
        FrontBridgeSender::getInstance()->writeToShm(buf, static_cast<int>(sizeof(head) + body_len), readResult.client_fd);
        return;
    }
    case FRIEND_ACCEPT:
    {
        FRIEND_APPLY_RESP resp{};
        const auto* req = reinterpret_cast<const FRIEND_APPLY_REQ*>(data + sizeof(HEAD));
        std::string from = std::string(req->from_user);
        std::string to = std::string(req->to_user);
        bool ok = ModelControl::getInstance()->getModel()->AcceptFriend(from, to);
        resp.status_code = ok ? 0 : 1;
        HEAD head{ FRIEND_ACCEPT, static_cast<int>(sizeof(resp)) };
        char buf[PACKAGESIZE]{};
        memcpy(buf, &head, sizeof(head));
        memcpy(buf + sizeof(head), &resp, sizeof(resp));
        FrontBridgeSender::getInstance()->writeToShm(buf, static_cast<int>(sizeof(head) + sizeof(resp)), readResult.client_fd);
        return;
    }
    case MESSAGE_PULL:
    {
        MESSAGE_LIST_RESP resp{};
        std::string user_id;
        if (recv_head.data_len >= 1) {
            const char* body_ptr = reinterpret_cast<const char*>(data + sizeof(HEAD));
            user_id.assign(body_ptr, strnlen(body_ptr, recv_head.data_len));
        }
        auto list = ModelControl::getInstance()->getModel()->GetUnreadMessages(user_id, 20);
        resp.status_code = 0;
        resp.message_count = static_cast<int>(list.size());

        char buf[PACKAGESIZE]{};
        int offset = 0;
        HEAD head{ MESSAGE_PULL, 0 };
        memcpy(buf, &head, sizeof(head));
        offset += sizeof(head);
        memcpy(buf + offset, &resp, sizeof(resp));
        offset += sizeof(resp);

        for (const auto& m : list) {
            MESSAGE_ITEM item{};
            strncpy_s(item.sender_id, sizeof(item.sender_id), m.sender_id.c_str(), _TRUNCATE);
            strncpy_s(item.created_at, sizeof(item.created_at), m.created_at.c_str(), _TRUNCATE);
            item.content_len = static_cast<int>(m.content.size());

            int need = static_cast<int>(sizeof(item) + item.content_len);
            if (offset + need > PACKAGESIZE) {
                break; // avoid overflow
            }
            memcpy(buf + offset, &item, sizeof(item));
            offset += sizeof(item);
            memcpy(buf + offset, m.content.data(), item.content_len);
            offset += item.content_len;
        }

        // fix counts if truncated
        int real_count = 0;
        {
            int tmp_offset = sizeof(HEAD) + sizeof(MESSAGE_LIST_RESP);
            while (tmp_offset + static_cast<int>(sizeof(MESSAGE_ITEM)) <= offset) {
                const auto* it = reinterpret_cast<const MESSAGE_ITEM*>(buf + tmp_offset);
                tmp_offset += static_cast<int>(sizeof(MESSAGE_ITEM)) + it->content_len;
                if (tmp_offset <= offset) real_count++;
            }
        }
        reinterpret_cast<MESSAGE_LIST_RESP*>(buf + sizeof(HEAD))->message_count = real_count;
        reinterpret_cast<HEAD*>(buf)->data_len = offset - static_cast<int>(sizeof(HEAD));
        FrontBridgeSender::getInstance()->writeToShm(buf, offset, readResult.client_fd);
        return;
    }
    default:
        std::cout << "Unknown service type: " << recv_head.service_type << std::endl;
        return;
    }

    this->threadPool->add_task(task);
}
