#include "PullDailyQuestionTask.h"
#include "FrontBridgeSender.h"
#include "ModelControl.h"

#include <cstring>
#include <string>

PullDailyQuestionTask::PullDailyQuestionTask(char* recv_msg_package, int len, int client_fd)
    : BaseTask(recv_msg_package, len, client_fd) {}

void PullDailyQuestionTask::do_service() {
    (void)recv_msg_package; // user_id not required for current selection
    BaseModel::QuestionData q = ModelControl::getInstance()->getModel()->FetchDailyQuestion();

    PULL_DAILY_QUESTION_RESP resp_body = {};
    resp_body.status_code = q.ok ? 0 : 1;
    if (q.ok) {
        strncpy_s(resp_body.questionnaire_code, sizeof(resp_body.questionnaire_code), q.questionnaire_code.c_str(), _TRUNCATE);
        strncpy_s(resp_body.question_id, sizeof(resp_body.question_id), q.question_id.c_str(), _TRUNCATE);
        resp_body.stem_len = static_cast<int>(q.stem.size());
        resp_body.options_len = static_cast<int>(q.options_json.size());
    }

    int body_len = sizeof(PULL_DAILY_QUESTION_RESP);
    int total_len = sizeof(HEAD) + body_len + resp_body.stem_len + resp_body.options_len;
    if (total_len > PACKAGESIZE) {
        resp_body.status_code = 1;
        resp_body.stem_len = 0;
        resp_body.options_len = 0;
        total_len = sizeof(HEAD) + sizeof(PULL_DAILY_QUESTION_RESP);
    }

    HEAD resp_head = {PULL_DAILY_QUESTION, total_len - static_cast<int>(sizeof(HEAD))};
    char resp_package[PACKAGESIZE] = {0};
    memcpy(resp_package, &resp_head, sizeof(HEAD));
    memcpy(resp_package + sizeof(HEAD), &resp_body, sizeof(PULL_DAILY_QUESTION_RESP));
    int offset = sizeof(HEAD) + sizeof(PULL_DAILY_QUESTION_RESP);
    if (resp_body.stem_len > 0) {
        memcpy(resp_package + offset, q.stem.data(), resp_body.stem_len);
        offset += resp_body.stem_len;
    }
    if (resp_body.options_len > 0) {
        memcpy(resp_package + offset, q.options_json.data(), resp_body.options_len);
    }

    FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
}
