#include "AssessmentHistoryTask.h"
#include "FrontBridgeSender.h"
#include "ModelControl.h"
#include "SQLiteHelper.h"
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <memory>

AssessmentHistoryTask::AssessmentHistoryTask(char* recv_msg_package, int len, int client_fd)
    : BaseTask(recv_msg_package, len, client_fd) {}

void AssessmentHistoryTask::do_service() {
    std::cout << "[AssessmentHistoryTask] Starting..." << std::endl;
    
    ASSESSMENT_HISTORY_REQ req = {0};
    memcpy(&req, this->recv_msg_package + sizeof(HEAD), sizeof(ASSESSMENT_HISTORY_REQ));
    
    std::cout << "[AssessmentHistoryTask] user_id=" << req.user_id
              << " limit=" << req.limit
              << " offset=" << req.offset << std::endl;
    
    // Build SQL query
    std::string sql = "SELECT id, completed_time, total_score, risk_level, suggestion "
                     "FROM assessment_records "
                     "WHERE user_id = ? "
                     "ORDER BY completed_time DESC";
    
    if (req.limit > 0) {
        sql += " LIMIT ? OFFSET ?";
    }
    
    std::vector<ASSESSMENT_RECORD_ITEM> records;
    
    try {
        // Open database
        auto db = std::make_unique<SQLiteDB>();
        if (!db->open("data/mindcarrer.db")) {
            throw std::runtime_error("Failed to open database");
        }
        
        SQLiteStmt stmt;
        if (!stmt.prepare(db->get(), sql)) {
            throw std::runtime_error("Failed to prepare statement");
        }
        
        stmt.bindText(1, req.user_id);
        
        if (req.limit > 0) {
            stmt.bindInt(2, req.limit);
            stmt.bindInt(3, req.offset);
        }
        
        while (stmt.step() == SQLITE_ROW) {
            ASSESSMENT_RECORD_ITEM item = {0};
            item.record_id = stmt.getInt(0);
            
            std::string completed_time = stmt.getText(1);
            strncpy_s(item.completed_time, sizeof(item.completed_time), 
                     completed_time.c_str(), _TRUNCATE);
            
            item.total_score = static_cast<float>(stmt.getDouble(2));
            
            std::string risk_level = stmt.getText(3);
            strncpy_s(item.risk_level, sizeof(item.risk_level), 
                     risk_level.c_str(), _TRUNCATE);
            
            std::string suggestion = stmt.getText(4);
            strncpy_s(item.suggestion, sizeof(item.suggestion), 
                     suggestion.c_str(), _TRUNCATE);
            
            records.push_back(item);
            
            std::cout << "[AssessmentHistoryTask] Record: id=" << item.record_id
                      << " time=" << item.completed_time
                      << " score=" << item.total_score
                      << " risk=" << item.risk_level << std::endl;
        }
        
        std::cout << "[AssessmentHistoryTask] Found " << records.size() << " records" << std::endl;
        
    } catch (std::exception& e) {
        std::cout << "[AssessmentHistoryTask] Database error: " << e.what() << std::endl;
        
        // Send error response
        ASSESSMENT_HISTORY_RESP resp = {0};
        resp.status_code = 1;  // Error
        resp.record_count = 0;
        
        HEAD resp_head = {ASSESSMENT_HISTORY, sizeof(ASSESSMENT_HISTORY_RESP)};
        char resp_package[PACKAGESIZE] = {0};
        memcpy(resp_package, &resp_head, sizeof(HEAD));
        memcpy(resp_package + sizeof(HEAD), &resp, sizeof(ASSESSMENT_HISTORY_RESP));
        
        FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
        return;
    }
    
    // Build response
    ASSESSMENT_HISTORY_RESP resp = {0};
    resp.status_code = 0;  // Success
    resp.record_count = static_cast<int>(records.size());
    
    int total_size = sizeof(HEAD) + sizeof(ASSESSMENT_HISTORY_RESP) + 
                    records.size() * sizeof(ASSESSMENT_RECORD_ITEM);
    
    if (total_size > PACKAGESIZE) {
        // Truncate if too large
        int max_records = (PACKAGESIZE - sizeof(HEAD) - sizeof(ASSESSMENT_HISTORY_RESP)) / 
                         sizeof(ASSESSMENT_RECORD_ITEM);
        records.resize(max_records);
        resp.record_count = max_records;
        total_size = sizeof(HEAD) + sizeof(ASSESSMENT_HISTORY_RESP) + 
                    max_records * sizeof(ASSESSMENT_RECORD_ITEM);
        std::cout << "[AssessmentHistoryTask] Truncated to " << max_records << " records" << std::endl;
    }
    
    // Build response package
    char resp_package[PACKAGESIZE] = {0};
    HEAD resp_head = {ASSESSMENT_HISTORY, total_size - static_cast<int>(sizeof(HEAD))};
    memcpy(resp_package, &resp_head, sizeof(HEAD));
    memcpy(resp_package + sizeof(HEAD), &resp, sizeof(ASSESSMENT_HISTORY_RESP));
    
    if (!records.empty()) {
        memcpy(resp_package + sizeof(HEAD) + sizeof(ASSESSMENT_HISTORY_RESP),
               records.data(),
               records.size() * sizeof(ASSESSMENT_RECORD_ITEM));
    }
    
    std::cout << "[AssessmentHistoryTask] Sending response, total_size=" << total_size << std::endl;
    FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
    std::cout << "[AssessmentHistoryTask] Task completed" << std::endl;
}