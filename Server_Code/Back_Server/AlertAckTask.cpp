#include "AlertAckTask.h"
#include "FrontBridgeSender.h"
#include "ModelControl.h"

#include <cstring>
#include <string>

AlertAckTask::AlertAckTask(char* recv_msg_package, int len, int client_fd)
    : BaseTask(recv_msg_package, len, client_fd) {}

void AlertAckTask::do_service() {
    ALERT_ACK_REQ req = {0};
    memcpy(&req, this->recv_msg_package + sizeof(HEAD), sizeof(ALERT_ACK_REQ));

    long long user_id = 0;
    try {
        user_id = std::stoll(std::string(req.user_id));
    } catch (...) {
        user_id = 0;
    }
    if (user_id > 0) {
        ModelControl::getInstance()->getModel()->RecordRiskEvent(user_id, std::string(req.assessment_id), req.alert_level);
    }

    ALERT_ACK_RESP resp_body = {0};
    resp_body.status_code = 0;
    HEAD resp_head = {ALERT_ACK, sizeof(ALERT_ACK_RESP)};
    char resp_package[PACKAGESIZE] = {0};
    memcpy(resp_package, &resp_head, sizeof(HEAD));
    memcpy(resp_package + sizeof(HEAD), &resp_body, sizeof(ALERT_ACK_RESP));
    FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
}

