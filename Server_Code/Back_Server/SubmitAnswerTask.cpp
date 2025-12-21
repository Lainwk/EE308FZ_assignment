#include "SubmitAnswerTask.h"
#include "FrontBridgeSender.h"
#include "ModelControl.h"

#include <cstring>

SubmitAnswerTask::SubmitAnswerTask(char* recv_msg_package, int len, int client_fd)
    : BaseTask(recv_msg_package, len, client_fd) {}

void SubmitAnswerTask::do_service() {
    SUBMIT_ANSWER_REQ req = {0};
    memcpy(&req, this->recv_msg_package + sizeof(HEAD), sizeof(SUBMIT_ANSWER_REQ));

    BaseModel::SubmitResult r = ModelControl::getInstance()->getModel()->SubmitAnswer(req);

    HEAD resp_head = {0};
    SUBMIT_ANSWER_RESP resp_body = {0};
    resp_head.service_type = SUBMIT_ANSWER;
    resp_body.status_code = r.status_code;
    resp_body.need_followup = r.need_followup ? 1 : 0;
    strncpy_s(resp_body.assessment_id, sizeof(resp_body.assessment_id), r.assessment_tag.c_str(), _TRUNCATE);

    resp_head.data_len = sizeof(SUBMIT_ANSWER_RESP);
    char resp_package[PACKAGESIZE] = {0};
    memcpy(resp_package, &resp_head, sizeof(HEAD));
    memcpy(resp_package + sizeof(HEAD), &resp_body, sizeof(SUBMIT_ANSWER_RESP));

    FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
}
