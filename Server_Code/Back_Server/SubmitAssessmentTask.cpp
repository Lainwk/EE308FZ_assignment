#include "SubmitAssessmentTask.h"
#include "FrontBridgeSender.h"
#include "ModelControl.h"

#include <cstring>
#include <string>
#include <vector>

SubmitAssessmentTask::SubmitAssessmentTask(char* recv_msg_package, int len, int client_fd)
    : BaseTask(recv_msg_package, len, client_fd) {}

void SubmitAssessmentTask::do_service() {
    std::cout << "[SubmitAssessmentTask] Starting assessment submission..." << std::endl;
    
    SUBMIT_ASSESSMENT_REQ req = {0};
    memcpy(&req, this->recv_msg_package + sizeof(HEAD), sizeof(SUBMIT_ASSESSMENT_REQ));

    // Compute how many ANSWER_ITEMs are actually present based on packet length
    int available_bytes = package_len - static_cast<int>(sizeof(HEAD)) - static_cast<int>(sizeof(SUBMIT_ASSESSMENT_REQ));
    int items_by_len = available_bytes > 0 ? available_bytes / static_cast<int>(sizeof(ANSWER_ITEM)) : 0;
    int effective_answer_count = items_by_len;
    if (req.answer_count != effective_answer_count) {
        std::cout << "[SubmitAssessmentTask] answer_count mismatch, req=" << req.answer_count
                  << " by_len=" << effective_answer_count << std::endl;
    }

    // Normalize daily 1+1 code to actual questionnaire code used in DB
    std::string questionnaire_code = std::string(req.questionnaire_code);
    if (questionnaire_code == "DAILY_1+1") {
        questionnaire_code = "DAILY_SNAPSHOT";
    }

    std::cout << "[SubmitAssessmentTask] user_id=" << req.user_id
              << " questionnaire_id=" << req.questionnaire_id
              << " code=" << questionnaire_code
              << " tag=" << req.assessment_tag
              << " answer_count=" << req.answer_count << std::endl;

    // Parse user_id
    long long user_id = 0;
    try {
        user_id = std::stoll(std::string(req.user_id));
    } catch (...) {
        user_id = 0;
    }
    
    std::cout << "[SubmitAssessmentTask] Parsed user_id=" << user_id << std::endl;

    // Parse answer items
    std::vector<BaseModel::AnswerData> answers;
    int offset = sizeof(HEAD) + sizeof(SUBMIT_ASSESSMENT_REQ);
    
    for (int i = 0; i < effective_answer_count; ++i) {
        ANSWER_ITEM item = {0};
        memcpy(&item, this->recv_msg_package + offset, sizeof(ANSWER_ITEM));
        offset += sizeof(ANSWER_ITEM);

        BaseModel::AnswerData ans;
        ans.question_id = item.question_id;
        ans.option_code = std::string(item.option_code);
        ans.option_score = item.option_score;
        ans.dimension_code = std::string(item.dimension_code);
        
        // 添加详细日志
        std::cout << "[SubmitAssessmentTask] Answer " << i << ": "
                  << "question_id=" << ans.question_id
                  << " option_code=" << ans.option_code
                  << " option_score=" << ans.option_score
                  << " dimension_code=" << ans.dimension_code << std::endl;
        
        answers.push_back(ans);
    }

    // Submit complete assessment
    std::cout << "[SubmitAssessmentTask] Calling SubmitCompleteAssessment..." << std::endl;
    BaseModel::SubmitResult result = ModelControl::getInstance()->getModel()->SubmitCompleteAssessment(
        user_id,
        req.questionnaire_id,  // may be -1; model will resolve by code
        questionnaire_code,
        std::string(req.assessment_tag),
        answers
    );
    
    std::cout << "[SubmitAssessmentTask] Result: status_code=" << result.status_code
              << " total_score=" << result.total_score
              << " risk_level=" << result.risk_level << std::endl;

    // Build response
    SUBMIT_ASSESSMENT_RESP resp_body = {};
    resp_body.status_code = result.status_code;
    strncpy_s(resp_body.assessment_tag, sizeof(resp_body.assessment_tag), 
              result.assessment_tag.c_str(), _TRUNCATE);
    resp_body.total_score = result.total_score;
    strncpy_s(resp_body.risk_level, sizeof(resp_body.risk_level), 
              result.risk_level.c_str(), _TRUNCATE);
    resp_body.need_followup = result.need_followup ? 1 : 0;
    resp_body.suggestion_len = static_cast<int>(result.suggestion.size());
    resp_body.dimension_count = static_cast<int>(result.dimension_scores.size());

    // Calculate total size
    int base_size = sizeof(HEAD) + sizeof(SUBMIT_ASSESSMENT_RESP);
    int data_size = resp_body.suggestion_len + 
                    resp_body.dimension_count * sizeof(DIMENSION_SCORE);
    int total_size = base_size + data_size;

    if (total_size > PACKAGESIZE) {
        // Truncate if too large
        resp_body.status_code = 2;  // Package size exceeded
        resp_body.suggestion_len = 0;
        resp_body.dimension_count = 0;
        total_size = base_size;
    }

    // Build response package
    HEAD resp_head = {SUBMIT_ASSESSMENT, total_size - static_cast<int>(sizeof(HEAD))};
    char resp_package[PACKAGESIZE] = {0};
    memcpy(resp_package, &resp_head, sizeof(HEAD));
    memcpy(resp_package + sizeof(HEAD), &resp_body, sizeof(SUBMIT_ASSESSMENT_RESP));

    if (resp_body.status_code == 0) {
        int pkg_offset = sizeof(HEAD) + sizeof(SUBMIT_ASSESSMENT_RESP);

        // Write suggestion text
        if (resp_body.suggestion_len > 0) {
            memcpy(resp_package + pkg_offset, result.suggestion.data(), resp_body.suggestion_len);
            pkg_offset += resp_body.suggestion_len;
        }

        // Write dimension scores
        for (const auto& dim_pair : result.dimension_scores) {
            const std::string& dim_code = dim_pair.first;
            float score = dim_pair.second;
            
            DIMENSION_SCORE dim_score = {};
            strncpy_s(dim_score.dimension_code, sizeof(dim_score.dimension_code),
                      dim_code.c_str(), _TRUNCATE);
            dim_score.score = score;
            memcpy(resp_package + pkg_offset, &dim_score, sizeof(DIMENSION_SCORE));
            pkg_offset += sizeof(DIMENSION_SCORE);
        }
    }

    std::cout << "[SubmitAssessmentTask] Sending response to client, total_size=" << total_size << std::endl;
    FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
    std::cout << "[SubmitAssessmentTask] Task completed" << std::endl;
}
