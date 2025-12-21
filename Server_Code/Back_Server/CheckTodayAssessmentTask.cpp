#include "CheckTodayAssessmentTask.h"
#include "FrontBridgeSender.h"
#include "ModelControl.h"
#include "SQLiteHelper.h"
#include <cstring>
#include <string>
#include <iostream>
#include <memory>

CheckTodayAssessmentTask::CheckTodayAssessmentTask(char* recv_msg_package, int len, int client_fd)
    : BaseTask(recv_msg_package, len, client_fd) {}

void CheckTodayAssessmentTask::do_service() {
    std::cout << "[CheckTodayAssessmentTask] Starting..." << std::endl;
    
    CHECK_TODAY_ASSESSMENT_REQ req = {0};
    memcpy(&req, this->recv_msg_package + sizeof(HEAD), sizeof(CHECK_TODAY_ASSESSMENT_REQ));
    
    std::cout << "[CheckTodayAssessmentTask] user_id=" << req.user_id << std::endl;
    
    CHECK_TODAY_ASSESSMENT_RESP resp = {0};
    resp.status_code = 0;
    resp.completed = 0;
    resp.last_score = 0.0f;
    strcpy_s(resp.last_completed_time, sizeof(resp.last_completed_time), "");
    
    try {
        // Open database
        auto db = std::make_unique<SQLiteDB>();
        if (!db->open("data/mindcarrer.db")) {
            throw std::runtime_error("Failed to open database");
        }
        
        // Check if there's an assessment completed today
        std::string sql = "SELECT completed_time, total_score "
                         "FROM assessment_records "
                         "WHERE user_id = ? "
                         "AND DATE(completed_time) = DATE('now', 'localtime') "
                         "ORDER BY completed_time DESC "
                         "LIMIT 1";
        
        SQLiteStmt stmt;
        if (!stmt.prepare(db->get(), sql)) {
            throw std::runtime_error("Failed to prepare statement");
        }
        
        stmt.bindText(1, req.user_id);
        
        if (stmt.step() == SQLITE_ROW) {
            // Found today's assessment
            resp.completed = 1;
            
            std::string completed_time = stmt.getText(0);
            strncpy_s(resp.last_completed_time, sizeof(resp.last_completed_time),
                     completed_time.c_str(), _TRUNCATE);
            
            resp.last_score = static_cast<float>(stmt.getDouble(1));
            
            std::cout << "[CheckTodayAssessmentTask] Today's assessment found: "
                      << "time=" << resp.last_completed_time
                      << " score=" << resp.last_score << std::endl;
        } else {
            // No assessment today, get the last one
            std::string last_sql = "SELECT completed_time, total_score "
                                  "FROM assessment_records "
                                  "WHERE user_id = ? "
                                  "ORDER BY completed_time DESC "
                                  "LIMIT 1";
            
            SQLiteStmt last_stmt;
            if (last_stmt.prepare(db->get(), last_sql)) {
                last_stmt.bindText(1, req.user_id);
                
                if (last_stmt.step() == SQLITE_ROW) {
                    std::string completed_time = last_stmt.getText(0);
                    strncpy_s(resp.last_completed_time, sizeof(resp.last_completed_time),
                             completed_time.c_str(), _TRUNCATE);
                    
                    resp.last_score = static_cast<float>(last_stmt.getDouble(1));
                    
                    std::cout << "[CheckTodayAssessmentTask] No assessment today. Last: "
                              << "time=" << resp.last_completed_time
                              << " score=" << resp.last_score << std::endl;
                } else {
                    std::cout << "[CheckTodayAssessmentTask] No assessment records found" << std::endl;
                }
            }
        }
        
    } catch (std::exception& e) {
        std::cout << "[CheckTodayAssessmentTask] Database error: " << e.what() << std::endl;
        resp.status_code = 1;  // Error
    }
    
    // Build response package
    HEAD resp_head = {CHECK_TODAY_ASSESSMENT, sizeof(CHECK_TODAY_ASSESSMENT_RESP)};
    char resp_package[PACKAGESIZE] = {0};
    memcpy(resp_package, &resp_head, sizeof(HEAD));
    memcpy(resp_package + sizeof(HEAD), &resp, sizeof(CHECK_TODAY_ASSESSMENT_RESP));
    
    std::cout << "[CheckTodayAssessmentTask] Sending response: completed=" << resp.completed << std::endl;
    FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
    std::cout << "[CheckTodayAssessmentTask] Task completed" << std::endl;
}