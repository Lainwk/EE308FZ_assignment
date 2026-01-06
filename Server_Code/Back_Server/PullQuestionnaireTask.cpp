#include "PullQuestionnaireTask.h"
#include "FrontBridgeSender.h"
#include "ModelControl.h"

#include <cstring>
#include <string>
#include <ctime>

PullQuestionnaireTask::PullQuestionnaireTask(char* recv_msg_package, int len, int client_fd)
    : BaseTask(recv_msg_package, len, client_fd) {}

void PullQuestionnaireTask::do_service() {
    PULL_QUESTIONNAIRE_REQ req = {0};
    memcpy(&req, this->recv_msg_package + sizeof(HEAD), sizeof(PULL_QUESTIONNAIRE_REQ));

    // Parse user_id
    long long user_id = 0;
    try {
        user_id = std::stoll(std::string(req.user_id));
    } catch (...) {
        user_id = 0;
    }

    // Get date (use provided date or current date)
    std::string date_str(req.date);
    if (date_str.empty() || date_str == "0000-00-00") {
        time_t now = time(nullptr);
        struct tm timeinfo;
        localtime_s(&timeinfo, &now);
        char buffer[11];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", &timeinfo);
        date_str = buffer;
    }

    printf("[PullQuestionnaireTask] Processing request for user_id=%lld, date=%s\n",
           user_id, date_str.c_str());

    // Try "1+1" daily mode first
    BaseModel::DailyQuestionsResult daily_result =
        ModelControl::getInstance()->getModel()->GetTodayDailyQuestions(user_id, date_str);

    if (daily_result.ok || daily_result.status_code == 1) {
        // Daily mode succeeded or already completed
        printf("[PullQuestionnaireTask] Using daily 1+1 mode, status_code=%d\n",
               daily_result.status_code);
        
        PULL_QUESTIONNAIRE_RESP resp_body = {};
        resp_body.status_code = daily_result.status_code;
        
        if (daily_result.status_code == 0) {
            // Success: return 2 questions
            resp_body.questionnaire_id = -1;  // Special ID for daily mode
            strncpy_s(resp_body.questionnaire_code, sizeof(resp_body.questionnaire_code),
                      "DAILY_1+1", _TRUNCATE);
            strncpy_s(resp_body.title, sizeof(resp_body.title),
                      "每日心理测评", _TRUNCATE);
            resp_body.expected_duration = 1;  // 1 minute
            resp_body.question_count = 2;

            // Calculate total data length for 2 questions
            int total_data_len = 2 * sizeof(QUESTION_ITEM);
            total_data_len += static_cast<int>(daily_result.question1.stem.size());
            total_data_len += static_cast<int>(daily_result.question1.options_json.size());
            total_data_len += static_cast<int>(daily_result.question2.stem.size());
            total_data_len += static_cast<int>(daily_result.question2.options_json.size());
            resp_body.total_data_len = total_data_len;

            // Check package size
            int base_size = sizeof(HEAD) + sizeof(PULL_QUESTIONNAIRE_RESP);
            int total_size = base_size + total_data_len;
            
            printf("[PullQuestionnaireTask] Daily mode package size: %d bytes\n", total_size);
            
            if (total_size > PACKAGESIZE) {
                printf("[PullQuestionnaireTask] ERROR: Package size exceeded!\n");
                resp_body.status_code = 2;
                resp_body.question_count = 0;
                resp_body.total_data_len = 0;
                total_size = base_size;
            }

            // Build response package
            HEAD resp_head = {PULL_QUESTIONNAIRE, total_size - static_cast<int>(sizeof(HEAD))};
            char resp_package[PACKAGESIZE] = {0};
            memcpy(resp_package, &resp_head, sizeof(HEAD));
            memcpy(resp_package + sizeof(HEAD), &resp_body, sizeof(PULL_QUESTIONNAIRE_RESP));

            if (resp_body.status_code == 0 && resp_body.question_count == 2) {
                int offset = sizeof(HEAD) + sizeof(PULL_QUESTIONNAIRE_RESP);

                // Write question 1
                QUESTION_ITEM item1 = {};
                item1.question_id = daily_result.question1.question_id;
                item1.seq = 1;
                strncpy_s(item1.dimension_code, sizeof(item1.dimension_code), "EMOTION", _TRUNCATE);
                item1.stem_len = static_cast<int>(daily_result.question1.stem.size());
                item1.options_len = static_cast<int>(daily_result.question1.options_json.size());
                
                memcpy(resp_package + offset, &item1, sizeof(QUESTION_ITEM));
                offset += sizeof(QUESTION_ITEM);
                
                if (!daily_result.question1.stem.empty()) {
                    memcpy(resp_package + offset, daily_result.question1.stem.data(),
                           daily_result.question1.stem.size());
                    offset += static_cast<int>(daily_result.question1.stem.size());
                }
                
                if (!daily_result.question1.options_json.empty()) {
                    memcpy(resp_package + offset, daily_result.question1.options_json.data(),
                           daily_result.question1.options_json.size());
                    offset += static_cast<int>(daily_result.question1.options_json.size());
                }

                // Write question 2
                QUESTION_ITEM item2 = {};
                item2.question_id = daily_result.question2.question_id;
                item2.seq = 2;
                strncpy_s(item2.dimension_code, sizeof(item2.dimension_code), "SYMPTOM", _TRUNCATE);
                item2.stem_len = static_cast<int>(daily_result.question2.stem.size());
                item2.options_len = static_cast<int>(daily_result.question2.options_json.size());
                
                memcpy(resp_package + offset, &item2, sizeof(QUESTION_ITEM));
                offset += sizeof(QUESTION_ITEM);
                
                if (!daily_result.question2.stem.empty()) {
                    memcpy(resp_package + offset, daily_result.question2.stem.data(),
                           daily_result.question2.stem.size());
                    offset += static_cast<int>(daily_result.question2.stem.size());
                }
                
                if (!daily_result.question2.options_json.empty()) {
                    memcpy(resp_package + offset, daily_result.question2.options_json.data(),
                           daily_result.question2.options_json.size());
                    offset += static_cast<int>(daily_result.question2.options_json.size());
                }

                printf("[PullQuestionnaireTask] Daily mode: packed 2 questions, total offset=%d\n", offset);
            }

            FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
            return;
        } else if (daily_result.status_code == 1) {
            // Already completed today
            printf("[PullQuestionnaireTask] User already completed today's assessment\n");
            
            HEAD resp_head = {PULL_QUESTIONNAIRE, sizeof(PULL_QUESTIONNAIRE_RESP)};
            char resp_package[PACKAGESIZE] = {0};
            memcpy(resp_package, &resp_head, sizeof(HEAD));
            memcpy(resp_package + sizeof(HEAD), &resp_body, sizeof(PULL_QUESTIONNAIRE_RESP));
            
            FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
            return;
        }
    }

    // Fallback to complete questionnaire mode (for PHQ-9, GAD-7, etc.)
    printf("[PullQuestionnaireTask] Falling back to complete questionnaire mode\n");
    BaseModel::QuestionnaireData qdata = ModelControl::getInstance()->getModel()->FetchDailyQuestionnaire(user_id, date_str);

    PULL_QUESTIONNAIRE_RESP resp_body = {};
    resp_body.status_code = qdata.ok ? 0 : 1;
    
    if (qdata.ok) {
        resp_body.questionnaire_id = qdata.questionnaire_id;
        strncpy_s(resp_body.questionnaire_code, sizeof(resp_body.questionnaire_code), 
                  qdata.questionnaire_code.c_str(), _TRUNCATE);
        strncpy_s(resp_body.title, sizeof(resp_body.title), qdata.title.c_str(), _TRUNCATE);
        resp_body.expected_duration = qdata.expected_duration;
        resp_body.question_count = static_cast<int>(qdata.questions.size());

        // Calculate total data length
        int total_data_len = resp_body.question_count * sizeof(QUESTION_ITEM);
        for (const auto& q : qdata.questions) {
            total_data_len += static_cast<int>(q.stem.size());
            total_data_len += static_cast<int>(q.options_json.size());
        }
        resp_body.total_data_len = total_data_len;
    }

    // Check if response fits in package
    int base_size = sizeof(HEAD) + sizeof(PULL_QUESTIONNAIRE_RESP);
    int total_size = base_size + resp_body.total_data_len;
    
    printf("[PullQuestionnaireTask] Package size check:\n");
    printf("  base_size: %d\n", base_size);
    printf("  total_data_len: %d\n", resp_body.total_data_len);
    printf("  total_size: %d\n", total_size);
    printf("  PACKAGESIZE: %d\n", PACKAGESIZE);
    
    if (total_size > PACKAGESIZE) {
        // Response too large, return error
        printf("[PullQuestionnaireTask] ERROR: Package size exceeded!\n");
        resp_body.status_code = 2;  // Package size exceeded
        resp_body.question_count = 0;
        resp_body.total_data_len = 0;
        total_size = base_size;
    }

    // Build response package
    HEAD resp_head = {PULL_QUESTIONNAIRE, total_size - static_cast<int>(sizeof(HEAD))};
    char resp_package[PACKAGESIZE] = {0};
    memcpy(resp_package, &resp_head, sizeof(HEAD));
    memcpy(resp_package + sizeof(HEAD), &resp_body, sizeof(PULL_QUESTIONNAIRE_RESP));

    if (resp_body.status_code == 0 && resp_body.question_count > 0) {
        int offset = sizeof(HEAD) + sizeof(PULL_QUESTIONNAIRE_RESP);

        // Write each question with its stem and options immediately following
        for (const auto& q : qdata.questions) {
            // Write QUESTION_ITEM
            QUESTION_ITEM item = {};
            item.question_id = q.question_id;
            item.seq = q.seq;
            strncpy_s(item.dimension_code, sizeof(item.dimension_code),
                      q.dimension_code.c_str(), _TRUNCATE);
            item.stem_len = static_cast<int>(q.stem.size());
            item.options_len = static_cast<int>(q.options_json.size());
            
            memcpy(resp_package + offset, &item, sizeof(QUESTION_ITEM));
            offset += sizeof(QUESTION_ITEM);
            
            // Write stem immediately after QUESTION_ITEM
            if (!q.stem.empty()) {
                memcpy(resp_package + offset, q.stem.data(), q.stem.size());
                offset += static_cast<int>(q.stem.size());
            }
            
            // Write options immediately after stem
            if (!q.options_json.empty()) {
                memcpy(resp_package + offset, q.options_json.data(), q.options_json.size());
                offset += static_cast<int>(q.options_json.size());
            }
        }
    }

    FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
}