#define _CRT_SECURE_NO_WARNINGS 1
#include "BaseModel.h"

#include <array>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <numeric>
#include <algorithm>
#include <thread>

#ifdef _WIN32
#include <direct.h>  // for _mkdir
#include <Windows.h>  // for Sleep()
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>  // for usleep()
#endif

using std::string;
using std::vector;
using std::map;

namespace {
    string getCurrentTimestamp() {
        time_t now = time(nullptr);
        struct tm timeinfo;
        localtime_s(&timeinfo, &now);
        char buffer[64];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
        return string(buffer);
    }

    string getCurrentDate() {
        time_t now = time(nullptr);
        struct tm timeinfo;
        localtime_s(&timeinfo, &now);
        char buffer[64];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", &timeinfo);
        return string(buffer);
    }

    int getDayOfWeek(const string& date) {
        // Simple day of week calculation (0=Sunday, 1=Monday, ...)
        struct tm timeinfo = {};
        sscanf_s(date.c_str(), "%d-%d-%d", &timeinfo.tm_year, &timeinfo.tm_mon, &timeinfo.tm_mday);
        timeinfo.tm_year -= 1900;
        timeinfo.tm_mon -= 1;
        mktime(&timeinfo);
        return timeinfo.tm_wday;
    }
}

// ========== Enhanced Methods Implementation ==========

// Helper: fetch dimension_code and score by question_id / option_code from DB options_json
static bool FetchScoreAndDimension(sqlite3* db, int question_id, const string& option_code, float& out_score, string& out_dim) {
    SQLiteStmt stmt;
    if (!stmt.prepare(db,
        "SELECT dimension_code, option_json FROM questions WHERE id=? LIMIT 1")) {
        return false;
    }
    stmt.bindInt(1, question_id);
    if (stmt.step() != SQLITE_ROW) return false;

    out_dim = stmt.getText(0);
    string opt_json = stmt.getText(1);

    // naive search: find code, then nearest "score":
    size_t pos = opt_json.find("\"code\":\"" + option_code + "\"");
    if (pos != string::npos) {
        size_t score_pos = opt_json.find("\"score\":", pos);
        if (score_pos != string::npos) {
            score_pos += 8; // skip "score":
            size_t end = opt_json.find_first_of(",}", score_pos);
            if (end != string::npos) {
                try {
                    out_score = std::stof(opt_json.substr(score_pos, end - score_pos));
                    return true;
                }
                catch (...) {
                    // ignore parse error
                }
            }
        }
    }
    return false;
}

// Helper: fetch dimension_code and score from daily_question_pool (daily 1+1 mode)
static bool FetchDailyScoreAndDimension(sqlite3* db, int question_id, const string& option_code, float& out_score, string& out_dim) {
    SQLiteStmt stmt;
    if (!stmt.prepare(db,
        "SELECT dimension_code, option_json FROM daily_question_pool WHERE id=? LIMIT 1")) {
        return false;
    }
    stmt.bindInt(1, question_id);
    if (stmt.step() != SQLITE_ROW) return false;

    out_dim = stmt.getText(0);
    string opt_json = stmt.getText(1);

    size_t pos = opt_json.find("\"code\":\"" + option_code + "\"");
    if (pos != string::npos) {
        size_t score_pos = opt_json.find("\"score\":", pos);
        if (score_pos != string::npos) {
            score_pos += 8;
            size_t end = opt_json.find_first_of(",}", score_pos);
            if (end != string::npos) {
                try {
                    out_score = std::stof(opt_json.substr(score_pos, end - score_pos));
                    return true;
                } catch (...) {
                }
            }
        }
    }
    return false;
}

bool BaseModel::HasCompletedToday(long long user_id, const string& date) {
    try {
        auto db = openDB();
        if (!db || !db->isOpen()) return false;

        SQLiteStmt stmt;
        if (!stmt.prepare(db->get(),
            "SELECT COUNT(*) FROM assessments "
            "WHERE user_id=? AND DATE(submit_at)=? AND source='daily'")) {
            return false;
        }

        stmt.bindInt64(1, user_id);
        stmt.bindText(2, date);

        if (stmt.step() == SQLITE_ROW) {
            return stmt.getInt(0) > 0;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[HasCompletedToday] Exception: " << e.what() << std::endl;
    }
    return false;
}

string BaseModel::SelectQuestionnaireByDate(const string& date) {
    int day_of_week = getDayOfWeek(date);

    // Monday: PHQ9, Tuesday: SAS, Others: DAILY_SNAPSHOT
    if (day_of_week == 1) {
        return "PHQ9";
    }
    else if (day_of_week == 2) {
        return "SAS";
    }
    else {
        return "DAILY_SNAPSHOT";
    }
}

BaseModel::QuestionnaireData BaseModel::FetchDailyQuestionnaire(long long user_id, const string& date) {
    QuestionnaireData result;

    try {
        auto db = openDB();
        if (!db || !db->isOpen()) {
            std::cerr << "[FetchDailyQuestionnaire] Failed to open database" << std::endl;
            return result;
        }

        // Check if already completed today
        if (HasCompletedToday(user_id, date)) {
            std::cout << "[FetchDailyQuestionnaire] User " << user_id << " already completed today" << std::endl;
            result.ok = false;
            return result;
        }

        // Select questionnaire by date
        string questionnaire_code = SelectQuestionnaireByDate(date);
        std::cout << "[FetchDailyQuestionnaire] Selected questionnaire: " << questionnaire_code << std::endl;

        // Get questionnaire info
        SQLiteStmt qStmt;
        if (!qStmt.prepare(db->get(),
            "SELECT id, code, title, expected_duration_sec FROM questionnaires "
            "WHERE code=? AND status=1 LIMIT 1")) {
            return result;
        }

        qStmt.bindText(1, questionnaire_code);

        if (qStmt.step() != SQLITE_ROW) {
            std::cerr << "[FetchDailyQuestionnaire] Questionnaire not found: " << questionnaire_code << std::endl;
            return result;
        }

        result.questionnaire_id = static_cast<int>(qStmt.getInt64(0));
        result.questionnaire_code = qStmt.getText(1);
        result.title = qStmt.getText(2);
        result.expected_duration = qStmt.getInt(3);

        // Get all questions for this questionnaire
        SQLiteStmt qnStmt;
        if (!qnStmt.prepare(db->get(),
            "SELECT id, seq, stem, dimension_code, option_json, scoring_rule "
            "FROM questions WHERE questionnaire_id=? AND enable_flag=1 ORDER BY seq")) {
            return result;
        }

        qnStmt.bindInt64(1, result.questionnaire_id);

        while (qnStmt.step() == SQLITE_ROW) {
            QuestionItem item;
            item.question_id = static_cast<int>(qnStmt.getInt64(0));
            item.seq = qnStmt.getInt(1);
            item.stem = qnStmt.getText(2);
            item.dimension_code = qnStmt.getText(3);
            item.options_json = qnStmt.getText(4);
            item.scoring_rule = qnStmt.getText(5);
            result.questions.push_back(item);
        }

        result.ok = !result.questions.empty();
        std::cout << "[FetchDailyQuestionnaire] Loaded " << result.questions.size() << " questions" << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "[FetchDailyQuestionnaire] Exception: " << e.what() << std::endl;
        result.ok = false;
    }

    return result;
}

string BaseModel::AssessRiskLevel(const string& questionnaire_code, float total_score) {
    if (questionnaire_code == "PHQ9") {
        // PHQ-9 scoring: 0-27
        if (total_score <= 4) return "green";
        if (total_score <= 9) return "yellow";
        if (total_score <= 14) return "orange";
        return "red";
    }
    else if (questionnaire_code == "SAS") {
        // SAS scoring: raw score * 1.25 = standard score
        int standard_score = static_cast<int>(total_score * 1.25);
        if (standard_score < 50) return "green";
        if (standard_score < 60) return "yellow";
        if (standard_score < 70) return "orange";
        return "red";
    }
    else { // DAILY_SNAPSHOT
        // 每日测评: 分数越高越好(表示心理状态越好)
        // 每题1-5分，2题总分2-10分
        // 8-10分: green (很好) - 平均4-5分
        // 6-7分: yellow (一般) - 平均3-3.5分
        // 4-5分: orange (需要关注) - 平均2-2.5分
        // 2-3分: red (需要特别关注) - 平均1-1.5分
        if (total_score >= 8) return "green";
        if (total_score >= 6) return "yellow";
        if (total_score >= 4) return "orange";
        return "red";
    }
}

string BaseModel::GenerateSuggestion(const string& risk_level, const string& questionnaire_code) {
    std::stringstream ss;

    if (questionnaire_code == "DAILY_SNAPSHOT") {
        // 每日测评的建议（分数高=状态好）
        if (risk_level == "green") {
            ss << "您的心理状态很好！";
            ss << "继续保持积极乐观的心态，规律作息，适当运动。";
        }
        else if (risk_level == "yellow") {
            ss << "您的心理状态基本正常，但有些小波动。";
            ss << "建议保持良好的生活习惯，注意休息，多参加有益身心的活动。";
        }
        else if (risk_level == "orange") {
            ss << "您可能正在经历一些心理压力。";
            ss << "建议保持规律作息，适当运动，多与朋友交流，必要时咨询专业人士。";
        }
        else { // red
            ss << "您的评估结果显示需要特别关注。";
            ss << "建议关注自己的心理健康，必要时寻求专业帮助。";
        }
    }
    else {
        // PHQ9和SAS的建议（分数高=问题严重）
        if (risk_level == "red") {
            ss << "您的评估结果显示需要特别关注。";
            if (questionnaire_code == "PHQ9") {
                ss << "建议尽快寻求专业心理咨询师或精神科医生的帮助。";
            }
            else if (questionnaire_code == "SAS") {
                ss << "您可能正在经历较严重的焦虑，建议咨询专业人士。";
            }
        }
        else if (risk_level == "orange") {
            ss << "您可能正在经历一些心理压力。";
            ss << "建议保持规律作息，适当运动，多与朋友交流，必要时咨询专业人士。";
        }
        else if (risk_level == "yellow") {
            ss << "您的心理状态基本正常，但有些小波动。";
            ss << "建议保持良好的生活习惯，注意休息，多参加有益身心的活动。";
        }
        else {
            ss << "您的心理状态良好！";
            ss << "继续保持积极乐观的心态，规律作息，适当运动。";
        }
    }

    return ss.str();
}

void BaseModel::UpdateIndicators(SQLiteDB* db, long long user_id, const map<string, float>& dimension_scores) {
    try {
        for (const auto& dim_pair : dimension_scores) {
            const string& dim_code = dim_pair.first;
            float score = dim_pair.second;
            // Calculate 7-day average
            float avg_7d = 0;
            {
                SQLiteStmt stmt;
                if (stmt.prepare(db->get(),
                    "SELECT AVG(a.option_score) FROM answers a "
                    "JOIN assessments ass ON a.assessment_id = ass.id "
                    "JOIN questions q ON a.question_id = q.id "
                    "WHERE ass.user_id=? AND q.dimension_code=? "
                    "AND ass.submit_at >= datetime('now', '-7 days')")) {
                    stmt.bindInt64(1, user_id);
                    stmt.bindText(2, dim_code);
                    if (stmt.step() == SQLITE_ROW) {
                        avg_7d = static_cast<float>(stmt.getDouble(0));
                    }
                }
            }

            // Calculate 30-day average
            float avg_30d = 0;
            {
                SQLiteStmt stmt;
                if (stmt.prepare(db->get(),
                    "SELECT AVG(a.option_score) FROM answers a "
                    "JOIN assessments ass ON a.assessment_id = ass.id "
                    "JOIN questions q ON a.question_id = q.id "
                    "WHERE ass.user_id=? AND q.dimension_code=? "
                    "AND ass.submit_at >= datetime('now', '-30 days')")) {
                    stmt.bindInt64(1, user_id);
                    stmt.bindText(2, dim_code);
                    if (stmt.step() == SQLITE_ROW) {
                        avg_30d = static_cast<float>(stmt.getDouble(0));
                    }
                }
            }

            // Calculate trend
            string trend = CalculateTrend(score, avg_7d, avg_30d);

            // Assess risk level for this dimension
            string risk = AssessRiskLevel("DAILY_SNAPSHOT", score);

            // Insert or update indicator
            SQLiteStmt stmt;
            if (stmt.prepare(db->get(),
                "INSERT OR REPLACE INTO indicators "
                "(user_id, dimension_code, last_score, avg_7d, avg_30d, trend, risk_level, updated_at) "
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?)")) {
                stmt.bindInt64(1, user_id);
                stmt.bindText(2, dim_code);
                stmt.bindDouble(3, score);
                stmt.bindDouble(4, avg_7d);
                stmt.bindDouble(5, avg_30d);
                stmt.bindText(6, trend);
                stmt.bindText(7, risk);
                stmt.bindText(8, getCurrentTimestamp());
                stmt.execute();
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[UpdateIndicators] Exception: " << e.what() << std::endl;
    }
}

string BaseModel::CalculateTrend(float last, float avg7d, float avg30d) {
    if (avg7d == 0 || avg30d == 0) return "flat";

    float short_term_change = last - avg7d;
    float long_term_change = avg7d - avg30d;

    if (short_term_change > 0.5 && long_term_change > 0.3) {
        return "up";
    }
    else if (short_term_change < -0.5 && long_term_change < -0.3) {
        return "down";
    }
    else {
        return "flat";
    }
}

BaseModel::SubmitResult BaseModel::SubmitCompleteAssessment(
    long long user_id,
    int questionnaire_id,
    const string& questionnaire_code,
    const string& assessment_tag,
    const vector<AnswerData>& answers)
{
    SubmitResult result;
    result.assessment_tag = assessment_tag;

    try {
        auto db = openDB();
        if (!db || !db->isOpen()) {
            result.status_code = 1;
            return result;
        }

        // Normalize daily 1+1 code to actual DB code
        string norm_code = questionnaire_code;
        if (norm_code == "DAILY_1+1") {
            norm_code = "DAILY_SNAPSHOT";
        }

        // Ensure questionnaire_id is valid (daily 1+1 会传 -1，需要按 code 查询实际 ID)
        if (questionnaire_id <= 0) {
            questionnaire_id = questionnaireIdByCode(db.get(), norm_code);
            if (questionnaire_id == 0) {
                std::cerr << "[SubmitCompleteAssessment] Invalid questionnaire_id for code="
                    << norm_code << std::endl;
                result.status_code = 1;
                return result;
            }
        }

        db->beginTransaction();

        // Daily assessments use client-provided scores (daily_question_pool) and should not
        // be overridden by the legacy questions table (e.g. SAS/PHQ). For non-daily
        // questionnaires we still normalize from DB to guard against tampering.
        bool isDailyAssessment = (norm_code.find("DAILY") != string::npos);

        // Normalize answers: ensure score & dimension from DB
        vector<AnswerData> norm_answers;
        norm_answers.reserve(answers.size());
        for (const auto& ans : answers) {
            AnswerData fixed = ans;
            float score_db = ans.option_score;
            string dim_db = ans.dimension_code;
            if (isDailyAssessment) {
                FetchDailyScoreAndDimension(db->get(), ans.question_id, ans.option_code, score_db, dim_db);
                if (score_db <= 0) score_db = ans.option_score; // fallback to client score
                fixed.option_score = score_db;
                if (!dim_db.empty()) fixed.dimension_code = dim_db;
            } else {
                FetchScoreAndDimension(db->get(), ans.question_id, ans.option_code, score_db, dim_db);
                if (score_db <= 0) score_db = ans.option_score; // fallback
                fixed.option_score = score_db;
                if (!dim_db.empty()) fixed.dimension_code = dim_db;
            }
            norm_answers.push_back(fixed);
        }

        // Calculate total score
        result.total_score = 0;
        for (const auto& ans : norm_answers) {
            result.total_score += ans.option_score;
        }

        // Calculate dimension scores
        map<string, vector<float>> dim_scores_list;
        for (const auto& ans : norm_answers) {
            dim_scores_list[ans.dimension_code].push_back(ans.option_score);
        }

        for (const auto& dim_pair : dim_scores_list) {
            const string& dim = dim_pair.first;
            const vector<float>& scores = dim_pair.second;
            float sum = std::accumulate(scores.begin(), scores.end(), 0.0f);
            float avg = sum / scores.size();
            result.dimension_scores[dim] = avg;
        }

        // Assess risk level
        result.risk_level = AssessRiskLevel(norm_code, result.total_score);
        result.need_followup = (result.risk_level == "red" || result.risk_level == "orange");

        // Generate suggestion
        result.suggestion = GenerateSuggestion(result.risk_level, norm_code);

        // Insert assessment
        long long assessment_db_id = 0;
        {
            SQLiteStmt stmt;
            if (!stmt.prepare(db->get(),
                "INSERT INTO assessments "
                "(user_id, questionnaire_id, start_at, submit_at, total_score, risk_level, source, client_tag) "
                "VALUES (?, ?, ?, ?, ?, ?, 'daily', ?)")) {
                db->rollback();
                result.status_code = 1;
                return result;
            }

            string now = getCurrentTimestamp();
            stmt.bindInt64(1, user_id);
            stmt.bindInt64(2, questionnaire_id);
            stmt.bindText(3, now);
            stmt.bindText(4, now);
            stmt.bindDouble(5, result.total_score);
            stmt.bindText(6, result.risk_level);
            stmt.bindText(7, assessment_tag);

            if (!stmt.execute()) {
                db->rollback();
                result.status_code = 1;
                return result;
            }

            assessment_db_id = db->lastInsertId();
        }

        // Insert all answers
        for (const auto& ans : norm_answers) {
            SQLiteStmt stmt;
            if (stmt.prepare(db->get(),
                "INSERT INTO answers "
                "(assessment_id, question_id, option_code, option_score, answer_text, created_at) "
                "VALUES (?, ?, ?, ?, '', ?)")) {
                stmt.bindInt64(1, assessment_db_id);
                stmt.bindInt64(2, ans.question_id);
                stmt.bindText(3, ans.option_code);
                stmt.bindDouble(4, ans.option_score);
                stmt.bindText(5, getCurrentTimestamp());
                stmt.execute();
            }
        }

        // Update indicators
        UpdateIndicators(db.get(), user_id, result.dimension_scores);

        // Record risk event if needed
        if (result.need_followup) {
            SQLiteStmt stmt;
            if (stmt.prepare(db->get(),
                "INSERT INTO risk_events "
                "(user_id, assessment_id, level, trigger_desc, action_taken, created_at) "
                "VALUES (?, ?, ?, 'auto_assessment', 'notification_sent', ?)")) {
                stmt.bindInt64(1, user_id);
                stmt.bindInt64(2, assessment_db_id);
                stmt.bindText(3, result.risk_level);
                stmt.bindText(4, getCurrentTimestamp());
                stmt.execute();
            }
        }

        // Update daily_push_history if this is a daily assessment
        std::cout << "[SubmitCompleteAssessment] Checking questionnaire_code: '" << norm_code << "'" << std::endl;

        if (norm_code.find("DAILY") != string::npos || norm_code == "DAILY_SNAPSHOT" || norm_code == "DAILY_MOOD_CHECK") {
            std::cout << "[SubmitCompleteAssessment] This is a DAILY assessment, updating daily tables..." << std::endl;

            SQLiteStmt stmt;
            string today = getCurrentTimestamp().substr(0, 10); // Get YYYY-MM-DD
            std::cout << "[SubmitCompleteAssessment] Today's date: " << today << std::endl;

            if (stmt.prepare(db->get(),
                "INSERT INTO daily_push_history (user_id, questionnaire_code, push_date, complete_at, created_at) "
                "VALUES (?, ?, ?, ?, ?) "
                "ON CONFLICT(user_id, questionnaire_code, push_date) "
                "DO UPDATE SET complete_at=excluded.complete_at")) {
                string now = getCurrentTimestamp();
                stmt.bindInt64(1, user_id);
                stmt.bindText(2, norm_code);
                stmt.bindText(3, today);
                stmt.bindText(4, now);
                stmt.bindText(5, now);
                stmt.execute();
            }

            {
                SQLiteStmt stmt2;
                if (stmt2.prepare(db->get(),
                    "INSERT INTO daily_assessments "
                    "(user_id, questionnaire_code, assessment_id, score, complete_at, created_at) "
                    "VALUES (?, ?, ?, ?, ?, ?) "
                    "ON CONFLICT(user_id, questionnaire_code, complete_at) DO NOTHING")) {
                    string now = getCurrentTimestamp();
                    stmt2.bindInt64(1, user_id);
                    stmt2.bindText(2, norm_code);
                    stmt2.bindInt64(3, assessment_db_id);
                    stmt2.bindDouble(4, result.total_score);
                    stmt2.bindText(5, now);
                    stmt2.bindText(6, now);
                    stmt2.execute();
                }
            }
        }
        else {
            std::cout << "[SubmitCompleteAssessment] Not a daily assessment, skipping daily tables update" << std::endl;
        }

        // Insert into assessment_records table for history tracking
        {
            std::cout << "[SubmitCompleteAssessment] Inserting into assessment_records..." << std::endl;
            SQLiteStmt stmt;
            if (stmt.prepare(db->get(),
                "INSERT INTO assessment_records "
                "(user_id, questionnaire_id, total_score, risk_level, suggestion, completed_time) "
                "VALUES (?, ?, ?, ?, ?, ?)")) {
                string now = getCurrentTimestamp();
                stmt.bindText(1, std::to_string(user_id));
                stmt.bindInt64(2, questionnaire_id);
                stmt.bindDouble(3, result.total_score);
                stmt.bindText(4, result.risk_level);
                stmt.bindText(5, result.suggestion);
                stmt.bindText(6, now);

                if (stmt.execute()) {
                    std::cout << "[SubmitCompleteAssessment] Successfully inserted into assessment_records" << std::endl;
                }
                else {
                    std::cout << "[SubmitCompleteAssessment] Failed to insert into assessment_records" << std::endl;
                }
            }
        }

        db->commit();
        result.status_code = 0;

        std::cout << "[SubmitCompleteAssessment] Success: user=" << user_id
            << " score=" << result.total_score
            << " risk=" << result.risk_level << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "[SubmitCompleteAssessment] Exception: " << e.what() << std::endl;
        result.status_code = 1;
    }

    return result;
}

BaseModel::ReportData BaseModel::FetchDetailedReport(long long user_id, int scope_days) {
    ReportData result;
    result.status_code = 0;

    try {
        auto db = openDB();
        if (!db || !db->isOpen()) {
            result.status_code = 1;
            return result;
        }

        // Get all indicators for this user
        SQLiteStmt stmt;
        if (!stmt.prepare(db->get(),
            "SELECT dimension_code, last_score, avg_7d, avg_30d, trend, risk_level "
            "FROM indicators WHERE user_id=?")) {
            result.status_code = 1;
            return result;
        }

        stmt.bindInt64(1, user_id);

        while (stmt.step() == SQLITE_ROW) {
            IndicatorItem item;
            item.dimension_code = stmt.getText(0);
            item.last_score = static_cast<float>(stmt.getDouble(1));
            item.avg_7d = static_cast<float>(stmt.getDouble(2));
            item.avg_30d = static_cast<float>(stmt.getDouble(3));
            item.trend = stmt.getText(4);
            item.risk_level = stmt.getText(5);

            // Get trend data for this dimension
            SQLiteStmt trendStmt;
            if (trendStmt.prepare(db->get(),
                "SELECT DATE(ass.submit_at) as date, AVG(a.option_score) as score "
                "FROM answers a "
                "JOIN assessments ass ON a.assessment_id = ass.id "
                "JOIN questions q ON a.question_id = q.id "
                "WHERE ass.user_id=? AND q.dimension_code=? "
                "AND ass.submit_at >= datetime('now', '-? days') "
                "GROUP BY DATE(ass.submit_at) "
                "ORDER BY date")) {
                trendStmt.bindInt64(1, user_id);
                trendStmt.bindText(2, item.dimension_code);
                trendStmt.bindInt(3, scope_days);

                while (trendStmt.step() == SQLITE_ROW) {
                    TrendPoint point;
                    point.date = trendStmt.getText(0);
                    point.score = static_cast<float>(trendStmt.getDouble(1));
                    item.trend_data.push_back(point);
                }
            }

            result.indicators.push_back(item);
        }

        // Generate summary
        int good_count = 0, concern_count = 0, up_count = 0, down_count = 0;
        for (const auto& ind : result.indicators) {
            if (ind.risk_level == "green") good_count++;
            else if (ind.risk_level == "orange" || ind.risk_level == "red") concern_count++;

            if (ind.trend == "up") up_count++;
            else if (ind.trend == "down") down_count++;
        }

        std::stringstream ss;
        ss << "综合评估：";
        if (concern_count == 0) {
            ss << "您的整体心理状态良好，各项指标均在正常范围内。";
        }
        else {
            ss << "有" << concern_count << "项指标需要关注。";
        }

        if (up_count > down_count) {
            ss << "整体趋势向好。";
        }
        else if (down_count > up_count) {
            ss << "部分指标有下降趋势，建议加强自我关怀。";
        }
        else {
            ss << "各项指标保持稳定。";
        }

        result.summary = ss.str();

    }
    catch (const std::exception& e) {
        std::cerr << "[FetchDetailedReport] Exception: " << e.what() << std::endl;
        result.status_code = 1;
    }

    return result;
}

// ========== Legacy Methods (Keep for backward compatibility) ==========

namespace {
    string getTimestampAfterSeconds(int seconds) {
        time_t now = time(nullptr) + seconds;
        struct tm timeinfo;
        localtime_s(&timeinfo, &now);
        char buffer[64];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
        return string(buffer);
    }

    long long toInt(const string& s) {
        try {
            return std::stoll(s);
        }
        catch (...) {
            return 0;
        }
    }
}

std::unique_ptr<SQLiteDB> BaseModel::openDB() {
    try {
        std::cout << "[DB] Opening SQLite database..." << std::endl;

        auto db = std::make_unique<SQLiteDB>();

        string db_dir = "data";
        string db_filename = "mindcarrer.db";
        string db_path = db_dir + "/" + db_filename;

#ifdef _WIN32
        _mkdir(db_dir.c_str());
#else
        mkdir(db_dir.c_str(), 0755);
#endif

        std::cout << "[DB] Database path: " << db_path << std::endl;

        if (!db->open(db_path)) {
            std::cerr << "[DB] Failed to open database: " << db->lastError() << std::endl;
            return nullptr;
        }

        std::cout << "[DB] SQLite database opened successfully" << std::endl;
        return db;

    }
    catch (const std::exception& e) {
        std::cerr << "[DB] Exception: " << e.what() << std::endl;
        return nullptr;
    }
}

long long BaseModel::ensureUserId(SQLiteDB* /*db*/, const string& user_id_str) {
    long long uid = toInt(user_id_str);
    if (uid > 0) return uid;
    return 0;
}

long long BaseModel::questionnaireIdByCode(SQLiteDB* db, const string& code) {
    SQLiteStmt stmt;
    if (!stmt.prepare(db->get(), "SELECT id FROM questionnaires WHERE code=? LIMIT 1")) {
        return 0;
    }
    stmt.bindText(1, code);
    if (stmt.step() == SQLITE_ROW) {
        return stmt.getInt64(0);
    }
    return 0;
}

LOGIN_RESP BaseModel::LoginCheck(const string& phone_num, const string& user_pwd) {
    LOGIN_RESP resp = {};
    try {
        auto db = openDB();
        if (!db || !db->isOpen()) {
            resp.status_code = 2;
            return resp;
        }

        SQLiteStmt stmt;

        // 判断是密码登录还是验证码登录（验证码登录时pwd为空）
        if (user_pwd.empty()) {
            // 验证码登录：只验证手机号是否存在
            std::cout << "[LoginCheck] SMS code login - checking phone only" << std::endl;
            if (!stmt.prepare(db->get(), "SELECT id, name, avatar_index FROM users WHERE phone=? LIMIT 1")) {
                resp.status_code = 2;
                return resp;
            }
            stmt.bindText(1, phone_num);
        }
        else {
            // 密码登录：验证手机号和密码
            std::cout << "[LoginCheck] Password login - checking phone and password" << std::endl;
            if (!stmt.prepare(db->get(), "SELECT id, name, avatar_index FROM users WHERE phone=? AND pwd_hash=? LIMIT 1")) {
                resp.status_code = 2;
                return resp;
            }
            stmt.bindText(1, phone_num);
            stmt.bindText(2, user_pwd);
        }

        if (stmt.step() == SQLITE_ROW) {
            resp.status_code = 0;
            long long uid = stmt.getInt64(0);
            string name = stmt.getText(1);
            resp.profile_picture_index = stmt.getInt(2);
            resp.friend_num = 0;

            string uid_str = std::to_string(uid);
            strncpy_s(resp.user_name, sizeof(resp.user_name), name.c_str(), _TRUNCATE);
            strncpy_s(resp.user_id, sizeof(resp.user_id), uid_str.c_str(), _TRUNCATE);

            std::cout << "[LoginCheck] Login SUCCESS for user: " << name << " (id=" << uid << ")" << std::endl;
        }
        else {
            resp.status_code = 2;
            std::cout << "[LoginCheck] Login FAILED - user not found or password incorrect" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[LoginCheck] Exception: " << e.what() << std::endl;
        resp.status_code = 2;
    }
    return resp;
}

REGISTER_RESP BaseModel::NewUserRegister(const string& phone_num, const string& user_pwd, const string& user_name) {
    REGISTER_RESP resp = {};
    try {
        auto db = openDB();
        if (!db || !db->isOpen()) {
            resp.status_code = 2;
            return resp;
        }

        db->beginTransaction();

        {
            SQLiteStmt check;
            if (!check.prepare(db->get(), "SELECT id FROM users WHERE phone=? LIMIT 1")) {
                db->rollback();
                resp.status_code = 2;
                return resp;
            }
            check.bindText(1, phone_num);
            if (check.step() == SQLITE_ROW) {
                db->rollback();
                resp.status_code = 2;
                return resp;
            }
        }

        SQLiteStmt stmt;
        if (!stmt.prepare(db->get(), "INSERT INTO users (phone, pwd_hash, name, avatar_index, status) VALUES (?, ?, ?, ?, 1)")) {
            db->rollback();
            resp.status_code = 2;
            return resp;
        }

        stmt.bindText(1, phone_num);
        stmt.bindText(2, user_pwd);
        stmt.bindText(3, user_name);
        stmt.bindInt(4, 0);

        if (stmt.execute()) {
            db->commit();
            resp.status_code = 0;
        }
        else {
            db->rollback();
            resp.status_code = 2;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[NewUserRegister] Exception: " << e.what() << std::endl;
        resp.status_code = 2;
    }
    return resp;
}

int BaseModel::StoreSmsCode(const string& phone_num, const string& code, const string& purpose, int expire_seconds) {
    try {
        auto db = openDB();
        if (!db || !db->isOpen()) return -1;

        db->beginTransaction();

        string created_at = getCurrentTimestamp();
        string expire_at = getTimestampAfterSeconds(expire_seconds);

        SQLiteStmt stmt;
        if (!stmt.prepare(db->get(),
            "INSERT INTO auth_sms_codes (phone, code, purpose, expire_at, verified, created_at) "
            "VALUES (?, ?, ?, ?, 0, ?)")) {
            db->rollback();
            return -1;
        }

        stmt.bindText(1, phone_num);
        stmt.bindText(2, code);
        stmt.bindText(3, purpose);
        stmt.bindText(4, expire_at);
        stmt.bindText(5, created_at);

        if (stmt.execute()) {
            db->commit();
            return 0;
        }
        else {
            db->rollback();
            return -1;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[StoreSmsCode] Exception: " << e.what() << std::endl;
        return -1;
    }
}

int BaseModel::VerifySmsCode(const string& phone_num, const string& code) {
    try {
        auto db = openDB();
        if (!db || !db->isOpen()) return 1;

        db->beginTransaction();

        SQLiteStmt stmt;
        if (!stmt.prepare(db->get(),
            "SELECT id, code, expire_at, verified FROM auth_sms_codes "
            "WHERE phone=? AND code=? ORDER BY id DESC LIMIT 1")) {
            db->rollback();
            return 1;
        }

        stmt.bindText(1, phone_num);
        stmt.bindText(2, code);

        if (stmt.step() != SQLITE_ROW) {
            db->rollback();
            return 1;
        }

        long long id = stmt.getInt64(0);
        string db_code = stmt.getText(1);
        string expire_at = stmt.getText(2);
        int verified = stmt.getInt(3);

        string current_time = getCurrentTimestamp();
        bool isExpired = (current_time > expire_at);

        if (isExpired || verified) {
            db->rollback();
            return 3;
        }

        if (db_code != code) {
            db->rollback();
            return 1;
        }

        SQLiteStmt mark;
        if (!mark.prepare(db->get(), "UPDATE auth_sms_codes SET verified=1 WHERE id=?")) {
            db->rollback();
            return 1;
        }
        mark.bindInt64(1, id);

        if (mark.execute()) {
            db->commit();
            return 0;
        }
        else {
            db->rollback();
            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[VerifySmsCode] Exception: " << e.what() << std::endl;
        return 1;
    }
}

bool BaseModel::InsertBlog(long long user_id, bool is_anonymous, const string& content, string& out_blog_id) {
    try {
        auto db = openDB();
        if (!db || !db->isOpen()) return false;
        EnsureBlogTables(db.get());

        db->beginTransaction();

        string created_at = getCurrentTimestamp();

        SQLiteStmt stmt;
        if (!stmt.prepare(db->get(),
            "INSERT INTO blogs (user_id, content, is_anonymous, created_at) VALUES (?, ?, ?, ?)")) {
            db->rollback();
            return false;
        }

        stmt.bindInt64(1, user_id);
        stmt.bindText(2, content);
        stmt.bindInt(3, is_anonymous ? 1 : 0);
        stmt.bindText(4, created_at);

        if (stmt.execute()) {
            out_blog_id = std::to_string(db->lastInsertId());
            db->commit();
            return true;
        }
        else {
            db->rollback();
            return false;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[InsertBlog] Exception: " << e.what() << std::endl;
        return false;
    }
}

std::vector<BaseModel::BlogItem> BaseModel::GetBlogs(int limit) {
    std::vector<BlogItem> list;
    try {
        auto db = openDB();
        if (!db || !db->isOpen()) {
            std::cerr << "[GetBlogs] Failed to open database" << std::endl;
            return list;
        }
        EnsureBlogTables(db.get());

        SQLiteStmt stmt;
        if (!stmt.prepare(db->get(),
            "SELECT b.id, b.user_id, IFNULL(u.name, ''), b.is_anonymous, "
            "b.created_at, b.content, "
            "(SELECT COUNT(*) FROM blog_likes WHERE blog_id=b.id) AS likes_cnt, "
            "(SELECT COUNT(*) FROM blog_comments WHERE blog_id=b.id) AS comment_cnt "
            "FROM blogs b "
            "LEFT JOIN users u ON u.id=b.user_id "
            "ORDER BY b.created_at DESC LIMIT ?")) {
            std::cerr << "[GetBlogs] Failed to prepare statement" << std::endl;
            return list;
        }
        stmt.bindInt(1, limit);
        while (stmt.step() == SQLITE_ROW) {
            BlogItem item;
            item.id = stmt.getInt64(0);
            item.user_id = stmt.getInt64(1);
            item.author_name = stmt.getText(2);
            item.is_anonymous = stmt.getInt(3);
            item.created_at = stmt.getText(4);
            item.content = stmt.getText(5);
            item.likes = stmt.getInt(6);
            item.comments = stmt.getInt(7);
            list.push_back(item);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[GetBlogs] Exception: " << e.what() << std::endl;
    }
    return list;
}

bool BaseModel::LikeBlog(long long blog_id, long long user_id, int& out_likes) {
    out_likes = 0;
    try {
        auto db = openDB();
        if (!db || !db->isOpen()) return false;
        EnsureBlogTables(db.get());

        db->beginTransaction();
        {
            SQLiteStmt stmt;
            if (!stmt.prepare(db->get(), "INSERT OR IGNORE INTO blog_likes (blog_id, user_id) VALUES (?, ?)")) {
                db->rollback();
                return false;
            }
            stmt.bindInt64(1, blog_id);
            stmt.bindInt64(2, user_id);
            if (!stmt.execute()) {
                db->rollback();
                return false;
            }
        }
        {
            SQLiteStmt stmt;
            if (!stmt.prepare(db->get(), "SELECT COUNT(*) FROM blog_likes WHERE blog_id=?")) {
                db->rollback();
                return false;
            }
            stmt.bindInt64(1, blog_id);
            if (stmt.step()) {
                out_likes = stmt.getInt(0);
            }
        }
        db->commit();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "[LikeBlog] Exception: " << e.what() << std::endl;
        return false;
    }
}

bool BaseModel::AddComment(long long blog_id, long long user_id, const std::string& user_name, const std::string& content, int& out_comments) {
    out_comments = 0;
    try {
        auto db = openDB();
        if (!db || !db->isOpen()) return false;
        EnsureBlogTables(db.get());

        db->beginTransaction();
        {
            SQLiteStmt stmt;
            if (!stmt.prepare(db->get(), "INSERT INTO blog_comments (blog_id, user_id, user_name, content, created_at) "
                "VALUES (?, ?, ?, ?, datetime('now','localtime'))")) {
                db->rollback();
                return false;
            }
            stmt.bindInt64(1, blog_id);
            stmt.bindInt64(2, user_id);
            stmt.bindText(3, user_name);
            stmt.bindText(4, content);
            if (!stmt.execute()) {
                db->rollback();
                return false;
            }
        }
        {
            SQLiteStmt stmt;
            if (!stmt.prepare(db->get(), "SELECT COUNT(*) FROM blog_comments WHERE blog_id=?")) {
                db->rollback();
                return false;
            }
            stmt.bindInt64(1, blog_id);
            if (stmt.step()) {
                out_comments = stmt.getInt(0);
            }
        }
        db->commit();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "[AddComment] Exception: " << e.what() << std::endl;
        return false;
    }
}

std::vector<BaseModel::CommentItem> BaseModel::GetComments(long long blog_id, int limit) {
    std::vector<CommentItem> list;
    try {
        auto db = openDB();
        if (!db || !db->isOpen()) return list;
        EnsureBlogTables(db.get());

        SQLiteStmt stmt;
        if (!stmt.prepare(db->get(),
            "SELECT user_name, created_at, content FROM blog_comments WHERE blog_id=? "
            "ORDER BY created_at DESC LIMIT ?")) {
            return list;
        }
        stmt.bindInt64(1, blog_id);
        stmt.bindInt(2, limit);
        while (stmt.step()) {
            CommentItem c;
            c.author_name = stmt.getText(0);
            c.created_at = stmt.getText(1);
            c.content = stmt.getText(2);
            list.push_back(c);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[GetComments] Exception: " << e.what() << std::endl;
    }
    return list;
}

void BaseModel::EnsureBlogTables(SQLiteDB* db) {
    if (!db) return;
    db->execute("CREATE TABLE IF NOT EXISTS blog_likes ("
        "blog_id INTEGER NOT NULL,"
        "user_id INTEGER NOT NULL,"
        "created_at TEXT DEFAULT (datetime('now','localtime')),"
        "PRIMARY KEY(blog_id, user_id))");
    db->execute("CREATE INDEX IF NOT EXISTS idx_blog_likes_blog ON blog_likes(blog_id)");
    db->execute("CREATE TABLE IF NOT EXISTS blog_comments ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "blog_id INTEGER NOT NULL,"
        "user_id INTEGER NOT NULL,"
        "user_name TEXT,"
        "content TEXT NOT NULL,"
        "created_at TEXT DEFAULT (datetime('now','localtime')))");
    db->execute("CREATE INDEX IF NOT EXISTS idx_blog_comments_blog ON blog_comments(blog_id, created_at)");
}

bool BaseModel::InsertMessage(long long sender_id, long long receiver_id, const string& content) {
    try {
        auto db = openDB();
        if (!db || !db->isOpen()) return false;

        db->beginTransaction();

        string created_at = getCurrentTimestamp();

        SQLiteStmt stmt;
        if (!stmt.prepare(db->get(),
            "INSERT INTO messages (sender_id, receiver_id, content, created_at, is_read) VALUES (?, ?, ?, ?, 0)")) {
            db->rollback();
            return false;
        }

        stmt.bindInt64(1, sender_id);
        stmt.bindInt64(2, receiver_id);
        stmt.bindText(3, content);
        stmt.bindText(4, created_at);

        if (stmt.execute()) {
            db->commit();
            return true;
        }
        else {
            db->rollback();
            return false;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[InsertMessage] Exception: " << e.what() << std::endl;
        return false;
    }
}

std::vector<BaseModel::UnreadMessage> BaseModel::GetUnreadMessages(const string& user_id, int limit) {
    std::vector<UnreadMessage> list;
    try {
        auto db = openDB();
        if (!db || !db->isOpen()) return list;

        SQLiteStmt stmt;
        if (!stmt.prepare(db->get(),
            "SELECT id, sender_id, content, created_at FROM messages "
            "WHERE receiver_id=? AND is_read=0 ORDER BY created_at ASC LIMIT ?")) {
            return list;
        }
        stmt.bindText(1, user_id);
        stmt.bindInt(2, limit);

        std::vector<long long> ids;
        while (stmt.step() == SQLITE_ROW) {
            UnreadMessage m;
            ids.push_back(stmt.getInt64(0));
            m.sender_id = stmt.getText(1);
            m.content = stmt.getText(2);
            m.created_at = stmt.getText(3);
            list.push_back(m);
        }

        if (!ids.empty()) {
            std::string sql = "UPDATE messages SET is_read=1 WHERE id IN (";
            for (size_t i = 0; i < ids.size(); ++i) {
                sql += (i == 0 ? "?" : ",?");
            }
            sql += ")";
            SQLiteStmt upd;
            if (upd.prepare(db->get(), sql)) {
                for (size_t i = 0; i < ids.size(); ++i) {
                    upd.bindInt64(static_cast<int>(i + 1), ids[i]);
                }
                upd.execute();
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[GetUnreadMessages] Exception: " << e.what() << std::endl;
    }
    return list;
}

// ---------- Friends & Requests ----------
static void ensureFriendTables(SQLiteDB* db) {
    db->execute(
        "CREATE TABLE IF NOT EXISTS friend_requests ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "from_user TEXT NOT NULL,"
        "to_user TEXT NOT NULL,"
        "status INTEGER DEFAULT 0,"
        "hello TEXT,"
        "created_at TEXT DEFAULT (datetime('now','localtime'))"
        ");");
    db->execute("CREATE INDEX IF NOT EXISTS idx_friend_requests_to_user ON friend_requests(to_user);");
    db->execute(
        "CREATE TABLE IF NOT EXISTS friends ("
        "user_id TEXT NOT NULL,"
        "friend_id TEXT NOT NULL,"
        "created_at TEXT DEFAULT (datetime('now','localtime')),"
        "PRIMARY KEY(user_id, friend_id)"
        ");");
}

// 将手机号或任意输入归一化为用户ID（字符串形式），不存在则返回空
static string normalizeUserId(SQLiteDB* db, const string& raw) {
    if (!db || raw.empty()) return {};

    SQLiteStmt stmt;
    if (!stmt.prepare(db->get(),
        "SELECT id FROM users WHERE phone=? OR CAST(id AS TEXT)=? LIMIT 1")) {
        return {};
    }
    stmt.bindText(1, raw);
    stmt.bindText(2, raw);
    if (stmt.step() == SQLITE_ROW) {
        return std::to_string(stmt.getInt64(0));
    }
    return {};
}

bool BaseModel::StoreFriendApply(const string& from_user, const string& to_user, const string& hello) {
    try {
        auto db = openDB();
        if (!db || !db->isOpen()) return false;
        ensureFriendTables(db.get());
        string normFrom = normalizeUserId(db.get(), from_user);
        string normTo = normalizeUserId(db.get(), to_user);
        if (normFrom.empty() || normTo.empty() || normFrom == normTo) {
            return false;
        }

        db->beginTransaction();

        // 插入或更新为待处理的好友申请
        SQLiteStmt upsertReq;
        if (upsertReq.prepare(db->get(),
            "INSERT OR REPLACE INTO friend_requests(from_user, to_user, status, hello, created_at) "
            "VALUES(?, ?, 0, ?, datetime('now','localtime'))")) {
            upsertReq.bindText(1, normFrom);
            upsertReq.bindText(2, normTo);
            upsertReq.bindText(3, hello);
            if (!upsertReq.execute()) {
                db->rollback();
                return false;
            }
        }
        else {
            db->rollback();
            return false;
        }

        db->commit();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "[StoreFriendApply] Exception: " << e.what() << std::endl;
        return false;
    }
}

bool BaseModel::AcceptFriend(const string& from_user, const string& to_user) {
    try {
        auto db = openDB();
        if (!db || !db->isOpen()) return false;
        ensureFriendTables(db.get());
        string normFrom = normalizeUserId(db.get(), from_user);
        string normTo = normalizeUserId(db.get(), to_user);
        if (normFrom.empty() || normTo.empty() || normFrom == normTo) {
            return false;
        }
        db->beginTransaction();

        // 标记申请为已接受
        SQLiteStmt updReq;
        if (updReq.prepare(db->get(),
            "UPDATE friend_requests SET status=1, from_user=?, to_user=? "
            "WHERE (from_user=? OR from_user=?) AND (to_user=? OR to_user=?)")) {
            updReq.bindText(1, normFrom);
            updReq.bindText(2, normTo);
            updReq.bindText(3, normFrom);
            updReq.bindText(4, from_user);
            updReq.bindText(5, normTo);
            updReq.bindText(6, to_user);
            updReq.execute();
        }

        bool ok = true;
        // 写入双向好友关系
        SQLiteStmt insA;
        if (insA.prepare(db->get(),
            "INSERT OR IGNORE INTO friends(user_id, friend_id, created_at) "
            "VALUES(?, ?, datetime('now','localtime'))")) {
            insA.bindText(1, normTo);
            insA.bindText(2, normFrom);
            ok = insA.execute() && ok;
        }
        SQLiteStmt insB;
        if (insB.prepare(db->get(),
            "INSERT OR IGNORE INTO friends(user_id, friend_id, created_at) "
            "VALUES(?, ?, datetime('now','localtime'))")) {
            insB.bindText(1, normFrom);
            insB.bindText(2, normTo);
            ok = insB.execute() && ok;
        }

        if (!ok) {
            db->rollback();
            return false;
        }

        db->commit();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "[AcceptFriend] Exception: " << e.what() << std::endl;
        return false;
    }
}

std::vector<FRIEND_REQUEST_ITEM> BaseModel::GetPendingFriendRequests(const string& user_id) {
    std::vector<FRIEND_REQUEST_ITEM> list;
    try {
        auto db = openDB();
        if (!db || !db->isOpen()) return list;
        ensureFriendTables(db.get());

        // 兼容旧数据：支持使用手机号或ID作为接收方
        string userPhone = user_id;
        {
            SQLiteStmt phoneStmt;
            if (phoneStmt.prepare(db->get(), "SELECT phone FROM users WHERE id=? LIMIT 1")) {
                phoneStmt.bindText(1, user_id);
                if (phoneStmt.step() == SQLITE_ROW) {
                    userPhone = phoneStmt.getText(0);
                }
            }
        }

        SQLiteStmt stmt;
        if (!stmt.prepare(db->get(),
            "SELECT fr.from_user, fr.hello, fr.created_at, "
            "IFNULL(u_from.name, '') "
            "FROM friend_requests fr "
            "LEFT JOIN users u_from ON u_from.id = fr.from_user "
            "WHERE fr.status=0 AND (fr.to_user=? OR fr.to_user=?) "
            "ORDER BY fr.created_at DESC")) {
            return list;
        }
        stmt.bindText(1, user_id);
        stmt.bindText(2, userPhone);
        while (stmt.step() == SQLITE_ROW) {
            FRIEND_REQUEST_ITEM item{};
            std::string from = stmt.getText(0);
            std::string hello = stmt.getText(1);
            std::string created = stmt.getText(2);
            std::string from_name = stmt.getText(3);
            strncpy_s(item.from_user, sizeof(item.from_user), from.c_str(), _TRUNCATE);
            strncpy_s(item.hello, sizeof(item.hello), hello.c_str(), _TRUNCATE);
            strncpy_s(item.created_at, sizeof(item.created_at), created.c_str(), _TRUNCATE);
            if (from_name.empty()) from_name = from;
            strncpy_s(item.from_name, sizeof(item.from_name), from_name.c_str(), _TRUNCATE);
            list.push_back(item);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[GetPendingFriendRequests] Exception: " << e.what() << std::endl;
    }
    return list;
}

std::vector<FRIEND_ITEM> BaseModel::GetFriendList(const string& user_id) {
    std::vector<FRIEND_ITEM> list;
    try {
        auto db = openDB();
        if (!db || !db->isOpen()) return list;
        ensureFriendTables(db.get());

        SQLiteStmt stmt;
        if (!stmt.prepare(db->get(),
            "SELECT f.friend_id, IFNULL(u.name, ''), IFNULL(u.avatar_index, 0) "
            "FROM friends f "
            "LEFT JOIN users u ON u.id = f.friend_id "
            "WHERE f.user_id=?")) {
            return list;
        }
        stmt.bindText(1, user_id);
        while (stmt.step() == SQLITE_ROW) {
            FRIEND_ITEM item{};
            string fid = stmt.getText(0);
            string fname = stmt.getText(1);
            int avatar = stmt.getInt(2);
            strncpy_s(item.friend_id, sizeof(item.friend_id), fid.c_str(), _TRUNCATE);
            if (fname.empty()) fname = fid;
            strncpy_s(item.friend_name, sizeof(item.friend_name), fname.c_str(), _TRUNCATE);
            // 如果数据库中有头像索引则使用，否则基于 friend_id 生成一个伪随机索引
            if (avatar <= 0) {
                avatar = static_cast<int>(1 + (std::hash<string>{}(fid) % 6));
            }
            item.profile_picture_index = static_cast<char>(avatar);
            list.push_back(item);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[GetFriendList] Exception: " << e.what() << std::endl;
    }
    return list;
}

BaseModel::QuestionData BaseModel::FetchDailyQuestion() {
    QuestionData q;

    try {
        auto db = openDB();
        if (db && db->isOpen()) {
            SQLiteStmt stmt;
            if (stmt.prepare(db->get(),
                "SELECT q.id, q.stem, q.option_json, qt.code FROM questions q "
                "JOIN questionnaires qt ON q.questionnaire_id = qt.id "
                "WHERE qt.type='daily' AND qt.status=1 AND q.enable_flag=1 ORDER BY RANDOM() LIMIT 1")) {

                if (stmt.step() == SQLITE_ROW) {
                    q.ok = true;
                    q.question_id = std::to_string(stmt.getInt64(0));
                    q.stem = stmt.getText(1);
                    q.options_json = stmt.getText(2);
                    q.questionnaire_code = stmt.getText(3);
                }
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[FetchDailyQuestion] Exception: " << e.what() << std::endl;
    }

    if (!q.ok) {
        struct BuiltinQ { const char* code; const char* id; const char* stem; };
        static const std::array<BuiltinQ, 7> fallback = { {
            {"DAILY_PHQGAD", "PHQ1", "In the past 24h, how was your overall mood?"},
            {"DAILY_PHQGAD", "PHQ2", "In the past 24h, did you enjoy things you usually like?"},
            {"DAILY_PHQGAD", "PHQ3", "In the past 24h, did you feel nervous or hard to relax?"},
            {"DAILY_PHQGAD", "PHQ4", "In the past 24h, did you have trouble falling or staying asleep?"},
            {"DAILY_PHQGAD", "PHQ5", "In the past 24h, did you feel tired or low on energy?"},
            {"DAILY_PHQGAD", "PHQ6", "In the past 24h, were you worrying too much?"},
            {"DAILY_PHQGAD", "PHQ7", "In the past 24h, how often did you blame yourself or feel hopeless?"}
        } };
        static const string option_json =
            R"([{"code":"A","label":"Not at all","score":1},{"code":"B","label":"Several days","score":2},{"code":"C","label":"More than half the days","score":3},{"code":"D","label":"Nearly every day","score":4}])";

        size_t idx = static_cast<size_t>(std::rand() % fallback.size());
        q.ok = true;
        q.questionnaire_code = fallback[idx].code;
        q.question_id = fallback[idx].id;
        q.stem = fallback[idx].stem;
        q.options_json = option_json;
    }

    return q;
}

BaseModel::SubmitResult BaseModel::SubmitAnswer(const SUBMIT_ANSWER_REQ& req) {
    SubmitResult result;
    result.assessment_tag = req.assessment_id;
    try {
        auto db = openDB();
        if (!db || !db->isOpen()) {
            result.status_code = 1;
            return result;
        }

        db->beginTransaction();

        long long user_id = ensureUserId(db.get(), req.user_id);
        long long questionnaire_id = questionnaireIdByCode(db.get(), req.questionnaire_code);
        long long question_id = toInt(req.question_id);

        if (user_id == 0 || questionnaire_id == 0 || question_id == 0) {
            result.status_code = 1;
            db->rollback();
            return result;
        }

        long long assessment_db_id = 0;

        {
            SQLiteStmt find;
            if (find.prepare(db->get(),
                "SELECT id, total_score FROM assessments WHERE user_id=? AND client_tag=? LIMIT 1")) {
                find.bindInt64(1, user_id);
                find.bindText(2, req.assessment_id);

                if (find.step() == SQLITE_ROW) {
                    assessment_db_id = find.getInt64(0);
                    double total = find.getDouble(1) + req.option_score;

                    SQLiteStmt upd;
                    if (upd.prepare(db->get(),
                        "UPDATE assessments SET total_score=?, submit_at=? WHERE id=?")) {
                        upd.bindDouble(1, total);
                        upd.bindText(2, getCurrentTimestamp());
                        upd.bindInt64(3, assessment_db_id);
                        upd.execute();
                    }
                }
            }
        }

        if (assessment_db_id == 0) {
            SQLiteStmt ins;
            if (ins.prepare(db->get(),
                "INSERT INTO assessments (user_id, questionnaire_id, start_at, submit_at, total_score, risk_level, source, client_tag) "
                "VALUES (?, ?, ?, ?, ?, 'green', 'daily', ?)")) {
                string now = getCurrentTimestamp();
                ins.bindInt64(1, user_id);
                ins.bindInt64(2, questionnaire_id);
                ins.bindText(3, now);
                ins.bindText(4, now);
                ins.bindDouble(5, req.option_score);
                ins.bindText(6, req.assessment_id);

                if (ins.execute()) {
                    assessment_db_id = db->lastInsertId();
                }
            }
        }

        {
            SQLiteStmt insAns;
            if (insAns.prepare(db->get(),
                "INSERT INTO answers (assessment_id, question_id, option_code, option_score, answer_text, created_at) "
                "VALUES (?, ?, ?, ?, '', ?)")) {
                insAns.bindInt64(1, assessment_db_id);
                insAns.bindInt64(2, question_id);
                insAns.bindText(3, req.option_code);
                insAns.bindDouble(4, req.option_score);
                insAns.bindText(5, getCurrentTimestamp());
                insAns.execute();
            }
        }

        db->commit();
        result.status_code = 0;
    }
    catch (const std::exception& e) {
        std::cerr << "[SubmitAnswer] Exception: " << e.what() << std::endl;
        result.status_code = 1;
    }
    return result;
}

vector<REPORT_ITEM> BaseModel::FetchIndicators(long long user_id) {
    vector<REPORT_ITEM> items;
    try {
        auto db = openDB();
        if (!db || !db->isOpen()) return items;

        SQLiteStmt stmt;
        if (!stmt.prepare(db->get(),
            "SELECT dimension_code, last_score, avg_7d, avg_30d, trend FROM indicators WHERE user_id=?")) {
            return items;
        }

        stmt.bindInt64(1, user_id);

        while (stmt.step() == SQLITE_ROW) {
            REPORT_ITEM item = {};
            string dim_code = stmt.getText(0);
            strncpy_s(item.dimension_code, sizeof(item.dimension_code), dim_code.c_str(), _TRUNCATE);
            item.last_score = static_cast<float>(stmt.getDouble(1));
            item.avg_7d = static_cast<float>(stmt.getDouble(2));
            item.avg_30d = static_cast<float>(stmt.getDouble(3));
            string t = stmt.getText(4);
            item.trend = t.empty() ? 'F' : t[0];
            items.push_back(item);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[FetchIndicators] Exception: " << e.what() << std::endl;
    }
    return items;
}

void BaseModel::RecordRiskEvent(long long user_id, const string& assessment_tag, int alert_level) {
    try {
        auto db = openDB();
        if (!db || !db->isOpen()) return;

        db->beginTransaction();

        long long assessment_id_db = 0;
        if (!assessment_tag.empty()) {
            SQLiteStmt find;
            if (find.prepare(db->get(), "SELECT id FROM assessments WHERE client_tag=? LIMIT 1")) {
                find.bindText(1, assessment_tag);
                if (find.step() == SQLITE_ROW) {
                    assessment_id_db = find.getInt64(0);
                }
            }
        }

        SQLiteStmt ins;
        if (ins.prepare(db->get(),
            "INSERT INTO risk_events (user_id, assessment_id, level, trigger_desc, action_taken, created_at) "
            "VALUES (?, ?, ?, 'client_alert', 'ack', ?)")) {
            ins.bindInt64(1, user_id);
            ins.bindInt64(2, assessment_id_db);
            ins.bindText(3, std::to_string(alert_level));
            ins.bindText(4, getCurrentTimestamp());

            if (ins.execute()) {
                db->commit();
            }
            else {
                db->rollback();
            }
        }
        else {
            db->rollback();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[RecordRiskEvent] Exception: " << e.what() << std::endl;
    }
}

// ========== NEW: Daily "1+1" Assessment Mode Implementation ==========

bool BaseModel::CheckTodayDailyCompleted(long long user_id, const string& date) {
    try {
        auto db = openDB();
        if (!db || !db->isOpen()) return false;

        SQLiteStmt stmt;
        if (!stmt.prepare(db->get(),
            "SELECT COUNT(*) FROM daily_assessments "
            "WHERE user_id=? AND DATE(complete_at)=?")) {
            return false;
        }

        stmt.bindInt64(1, user_id);
        stmt.bindText(2, date);

        if (stmt.step() == SQLITE_ROW) {
            return stmt.getInt(0) > 0;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[CheckTodayDailyCompleted] Exception: " << e.what() << std::endl;
    }
    return false;
}

string GetDimensionByWeekday(const string& date) {
    int day_of_week = getDayOfWeek(date);

    // Rotation strategy:
    // Monday(1)/Thursday(4): DEP (Depression)
    // Tuesday(2)/Friday(5): ANX (Anxiety)
    // Wednesday(3)/Saturday(6): STRESS
    // Sunday(0): SOCIAL

    switch (day_of_week) {
    case 1: case 4: return "DEP";
    case 2: case 5: return "ANX";
    case 3: case 6: return "STRESS";
    case 0: return "SOCIAL";
    default: return "DEP";
    }
}

BaseModel::DailyQuestionsResult BaseModel::GetTodayDailyQuestions(long long user_id, const string& date) {
    DailyQuestionsResult result;
    result.status_code = 2;  // Default: error
    const string questionnaire_code = "DAILY_SNAPSHOT";

    try {
        auto db = openDB();
        if (!db || !db->isOpen()) {
            std::cerr << "[GetTodayDailyQuestions] Failed to open database" << std::endl;
            return result;
        }

        // 1. Check if already completed today
        if (CheckTodayDailyCompleted(user_id, date)) {
            std::cout << "[GetTodayDailyQuestions] User " << user_id << " already completed today" << std::endl;
            result.status_code = 1;  // Already completed
            result.ok = false;
            return result;
        }
        // 2. Deterministic question selection (stable per user+date)
        auto selectQuestion = [&](const std::string& dim, int seed_offset, DailyQuestion& out_q) -> bool {
            int count = 0;
            {
                SQLiteStmt cnt;
                if (cnt.prepare(db->get(),
                    "SELECT COUNT(*) FROM daily_question_pool WHERE dimension_code=? AND enable_flag=1")) {
                    cnt.bindText(1, dim);
                    if (cnt.step() == SQLITE_ROW) {
                        count = cnt.getInt(0);
                    }
                }
            }
            if (count <= 0) return false;
            int offset = seed_offset % count;

            SQLiteStmt stmt;
            if (!stmt.prepare(db->get(),
                "SELECT id, stem, option_json, max_score FROM daily_question_pool "
                "WHERE dimension_code=? AND enable_flag=1 "
                "ORDER BY id LIMIT 1 OFFSET ?")) {
                return false;
            }
            stmt.bindText(1, dim);
            stmt.bindInt(2, offset);
            if (stmt.step() == SQLITE_ROW) {
                out_q.question_id = stmt.getInt(0);
                out_q.stem = stmt.getText(1);
                out_q.options_json = stmt.getText(2);
                out_q.max_score = static_cast<float>(stmt.getDouble(3));
                return true;
            }
            return false;
        };

        // Seed offset: user_id + day-of-month to keep stable per day
        time_t now_t = time(nullptr);
        struct tm tm_now {};
        localtime_s(&tm_now, &now_t);
        int seed = static_cast<int>((user_id % 97) + tm_now.tm_mday);

        if (!selectQuestion("EMOTION", seed, result.question1)) {
            std::cerr << "[GetTodayDailyQuestions] No EMOTION questions found" << std::endl;
            return result;
        }

        string dimension = GetDimensionByWeekday(date);
        std::cout << "[GetTodayDailyQuestions] Selected dimension: " << dimension << " for date: " << date << std::endl;

        if (!selectQuestion(dimension, seed + 3, result.question2)) {
            std::cerr << "[GetTodayDailyQuestions] No " << dimension << " questions found" << std::endl;
            return result;
        }

        // 3. Record/ensure push history (no question ids needed)
        {
            SQLiteStmt pushStmt;
            if (pushStmt.prepare(db->get(),
                "INSERT INTO daily_push_history "
                "(user_id, questionnaire_code, push_date, complete_at, created_at) "
                "VALUES (?, ?, ?, NULL, ?) "
                "ON CONFLICT(user_id, questionnaire_code, push_date) "
                "DO NOTHING")) {
                pushStmt.bindInt64(1, user_id);
                pushStmt.bindText(2, questionnaire_code);
                pushStmt.bindText(3, date);
                pushStmt.bindText(4, getCurrentTimestamp());
                pushStmt.execute();
            }
        }

        result.ok = true;
        result.status_code = 0;

        std::cout << "[GetTodayDailyQuestions] Success: Q1=" << result.question1.question_id
            << " Q2=" << result.question2.question_id << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "[GetTodayDailyQuestions] Exception: " << e.what() << std::endl;
        result.ok = false;
        result.status_code = 2;
    }

    return result;
}

BaseModel::DailySubmitResult BaseModel::SubmitTodayDaily(
    long long user_id,
    const string& date,
    int question1_id,
    int answer1,
    float score1,
    int question2_id,
    int answer2,
    float score2)
{
    DailySubmitResult result;
    const string questionnaire_code = "DAILY_SNAPSHOT";

    try {
        auto db = openDB();
        if (!db || !db->isOpen()) {
            std::cerr << "[SubmitTodayDaily] Failed to open database" << std::endl;
            return result;
        }

        db->beginTransaction();

        long long questionnaire_id = questionnaireIdByCode(db.get(), questionnaire_code);
        if (questionnaire_id == 0) {
            std::cerr << "[SubmitTodayDaily] Questionnaire not found: " << questionnaire_code << std::endl;
            db->rollback();
            return result;
        }

        // Calculate total score and emotion index
        float total_score = score1 + score2;
        float max_possible = 10.0f;  // Assuming both questions have max score of 5

        // Emotion index: 0-100, higher is better
        result.emotion_index = (total_score / max_possible) * 100.0f;

        // Assess risk level based on emotion index
        if (result.emotion_index >= 80) {
            result.risk_level = "green";
            result.feedback = "您的心理状态很好！继续保持积极乐观的心态。";
        }
        else if (result.emotion_index >= 60) {
            result.risk_level = "yellow";
            result.feedback = "您的心理状态基本正常，注意保持良好的生活习惯。";
        }
        else if (result.emotion_index >= 40) {
            result.risk_level = "orange";
            result.feedback = "您可能正在经历一些压力，建议多与朋友交流，必要时咨询专业人士。";
        }
        else {
            result.risk_level = "red";
            result.feedback = "您的评估结果显示需要特别关注，建议寻求专业心理咨询师的帮助。";
        }

        // Insert assessment
        long long assessment_db_id = 0;
        string now = getCurrentTimestamp();
        {
            SQLiteStmt stmt;
            if (!stmt.prepare(db->get(),
                "INSERT INTO assessments (user_id, questionnaire_id, start_at, submit_at, total_score, risk_level, source, client_tag) "
                "VALUES (?, ?, ?, ?, ?, ?, 'daily', ?)")) {
                db->rollback();
                return result;
            }
            stmt.bindInt64(1, user_id);
            stmt.bindInt64(2, questionnaire_id);
            stmt.bindText(3, now);
            stmt.bindText(4, now);
            stmt.bindDouble(5, total_score);
            stmt.bindText(6, result.risk_level);
            stmt.bindText(7, date + "_snap");
            if (!stmt.execute()) {
                db->rollback();
                return result;
            }
            assessment_db_id = db->lastInsertId();
        }

        // Insert answers
        {
            SQLiteStmt stmt;
            if (!stmt.prepare(db->get(),
                "INSERT INTO answers (assessment_id, question_id, option_code, option_score, answer_text, created_at) "
                "VALUES (?, ?, ?, ?, '', ?)")) {
                db->rollback();
                return result;
            }
            stmt.bindInt64(1, assessment_db_id);
            stmt.bindInt64(2, question1_id);
            stmt.bindText(3, std::to_string(answer1));
            stmt.bindDouble(4, score1);
            stmt.bindText(5, now);
            if (!stmt.execute()) { db->rollback(); return result; }
        }
        {
            SQLiteStmt stmt;
            if (!stmt.prepare(db->get(),
                "INSERT INTO answers (assessment_id, question_id, option_code, option_score, answer_text, created_at) "
                "VALUES (?, ?, ?, ?, '', ?)")) {
                db->rollback();
                return result;
            }
            stmt.bindInt64(1, assessment_db_id);
            stmt.bindInt64(2, question2_id);
            stmt.bindText(3, std::to_string(answer2));
            stmt.bindDouble(4, score2);
            stmt.bindText(5, now);
            if (!stmt.execute()) { db->rollback(); return result; }
        }

        // Insert daily_assessments summary
        {
            SQLiteStmt stmt;
            if (!stmt.prepare(db->get(),
                "INSERT INTO daily_assessments "
                "(user_id, questionnaire_code, assessment_id, score, complete_at, created_at) "
                "VALUES (?, ?, ?, ?, ?, ?) "
                "ON CONFLICT(user_id, questionnaire_code, complete_at) DO NOTHING")) {
                db->rollback();
                return result;
            }
            stmt.bindInt64(1, user_id);
            stmt.bindText(2, questionnaire_code);
            stmt.bindInt64(3, assessment_db_id);
            stmt.bindDouble(4, total_score);
            stmt.bindText(5, now);
            stmt.bindText(6, now);
            if (!stmt.execute()) { db->rollback(); return result; }
        }

        // Update push history to mark as completed
        {
            SQLiteStmt updateStmt;
            if (updateStmt.prepare(db->get(),
                "INSERT INTO daily_push_history (user_id, questionnaire_code, push_date, complete_at, created_at) "
                "VALUES (?, ?, ?, ?, ?) "
                "ON CONFLICT(user_id, questionnaire_code, push_date) DO UPDATE SET complete_at=excluded.complete_at")) {
                updateStmt.bindInt64(1, user_id);
                updateStmt.bindText(2, questionnaire_code);
                updateStmt.bindText(3, date);
                updateStmt.bindText(4, now);
                updateStmt.bindText(5, now);
                updateStmt.execute();
            }
        }

        db->commit();
        result.ok = true;

        std::cout << "[SubmitTodayDaily] Success: user=" << user_id
            << " emotion_index=" << result.emotion_index
            << " risk=" << result.risk_level << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "[SubmitTodayDaily] Exception: " << e.what() << std::endl;
        result.ok = false;
    }

    return result;
}


// 修改密码方法
BaseModel::ChangePasswordResult BaseModel::ChangePassword(const std::string& user_id, const std::string& old_password, const std::string& new_password) {
    ChangePasswordResult result;
    result.status_code = 3;  // Default: other error
    result.message = "Unknown error";

    try {
        auto db = openDB();
        if (!db || !db->isOpen()) {
            std::cerr << "[ChangePassword] Failed to open database" << std::endl;
            result.status_code = 3;
            result.message = "Database connection failed";
            return result;
        }

        long long uid = toInt(user_id);
        if (uid <= 0) {
            std::cerr << "[ChangePassword] Invalid user_id: " << user_id << std::endl;
            result.status_code = 2;  // User not found
            result.message = "Invalid user ID";
            return result;
        }

        db->beginTransaction();

        // 1. Verify old password
        {
            SQLiteStmt stmt;
            if (!stmt.prepare(db->get(), "SELECT pwd_hash FROM users WHERE id=? LIMIT 1")) {
                db->rollback();
                result.status_code = 3;
                result.message = "Database query failed";
                return result;
            }
            stmt.bindInt64(1, uid);

            if (stmt.step() != SQLITE_ROW) {
                db->rollback();
                std::cerr << "[ChangePassword] User not found: " << uid << std::endl;
                result.status_code = 2;  // User not found
                result.message = "User not found";
                return result;
            }

            std::string current_pwd_hash = stmt.getText(0);
            if (current_pwd_hash != old_password) {
                db->rollback();
                std::cout << "[ChangePassword] Old password incorrect for user: " << uid << std::endl;
                result.status_code = 1;  // Wrong old password
                result.message = "Old password is incorrect";
                return result;
            }
        }

        // 2. Update to new password
        {
            SQLiteStmt stmt;
            if (!stmt.prepare(db->get(), "UPDATE users SET pwd_hash=? WHERE id=?")) {
                db->rollback();
                result.status_code = 3;
                result.message = "Database update failed";
                return result;
            }
            stmt.bindText(1, new_password);
            stmt.bindInt64(2, uid);

            if (!stmt.execute()) {
                db->rollback();
                std::cerr << "[ChangePassword] Failed to update password for user: " << uid << std::endl;
                result.status_code = 3;
                result.message = "Failed to update password";
                return result;
            }
        }

        db->commit();
        result.status_code = 0;  // Success
        result.message = "Password changed successfully";

        std::cout << "[ChangePassword] Password changed successfully for user: " << uid << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "[ChangePassword] Exception: " << e.what() << std::endl;
        result.status_code = 3;
        result.message = std::string("Exception: ") + e.what();
    }

    return result;
}
