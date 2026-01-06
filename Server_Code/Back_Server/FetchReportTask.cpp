#include "FetchReportTask.h"
#include "FrontBridgeSender.h"
#include "ModelControl.h"

#include <cstring>
#include <string>
#include <vector>

FetchReportTask::FetchReportTask(char* recv_msg_package, int len, int client_fd)
    : BaseTask(recv_msg_package, len, client_fd) {}

void FetchReportTask::do_service() {
    FETCH_REPORT_REQ req = {0};
    memcpy(&req, this->recv_msg_package + sizeof(HEAD), sizeof(FETCH_REPORT_REQ));

    long long user_id = 0;
    try {
        user_id = std::stoll(std::string(req.user_id));
    } catch (...) {
        user_id = 0;
    }

    std::vector<REPORT_ITEM> items;
    if (user_id > 0) {
        items = ModelControl::getInstance()->getModel()->FetchIndicators(user_id);
    }

    FETCH_REPORT_RESP resp_body = {};
    resp_body.status_code = 0;
    resp_body.item_count = static_cast<int>(items.size());

    int body_base = sizeof(FETCH_REPORT_RESP);
    int body_size = body_base + resp_body.item_count * sizeof(REPORT_ITEM);
    if (sizeof(HEAD) + body_size > PACKAGESIZE) {
        // truncate if overflow
        resp_body.item_count = (PACKAGESIZE - static_cast<int>(sizeof(HEAD)) - body_base) / static_cast<int>(sizeof(REPORT_ITEM));
        body_size = body_base + resp_body.item_count * sizeof(REPORT_ITEM);
    }

    HEAD resp_head = {FETCH_REPORT, body_size};
    char resp_package[PACKAGESIZE] = {0};
    memcpy(resp_package, &resp_head, sizeof(HEAD));
    memcpy(resp_package + sizeof(HEAD), &resp_body, sizeof(FETCH_REPORT_RESP));
    if (resp_body.item_count > 0) {
        memcpy(resp_package + sizeof(HEAD) + sizeof(FETCH_REPORT_RESP), items.data(),
               resp_body.item_count * sizeof(REPORT_ITEM));
    }

    FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
}

