#pragma once

#include "SQLiteHelper.h"
#include "Protocol.h"

#include <memory>
#include <string>
#include <vector>
#include <map>

// Enhanced DB helper for daily psychological assessment
class BaseModel {
public:
    BaseModel() = default;
    ~BaseModel() = default;

    LOGIN_RESP LoginCheck(const std::string& phone_num, const std::string& user_pwd);
    REGISTER_RESP NewUserRegister(const std::string& phone_num, const std::string& user_pwd, const std::string& user_name);
    
    // Change password
    struct ChangePasswordResult {
        int status_code{0};  // 0=success, 1=wrong old password, 2=user not found, 3=other error
        std::string message;
    };
    ChangePasswordResult ChangePassword(const std::string& user_id, const std::string& old_password, const std::string& new_password);

    // SMS code
    int StoreSmsCode(const std::string& phone_num, const std::string& code, const std::string& purpose, int expire_seconds = 300);
    int VerifySmsCode(const std::string& phone_num, const std::string& code);

    // Blog & message
    bool InsertBlog(long long user_id, bool is_anonymous, const std::string& content, std::string& out_blog_id);
    bool InsertMessage(long long sender_id, long long receiver_id, const std::string& content);
    struct BlogItem {
        long long id{0};
        long long user_id{0};
        std::string author_name;
        int is_anonymous{0};
        int likes{0};
        int comments{0};
        std::string created_at;
        std::string content;
    };
    struct CommentItem {
        std::string author_name;
        std::string created_at;
        std::string content;
    };
    std::vector<BlogItem> GetBlogs(int limit);
    bool LikeBlog(long long blog_id, long long user_id, int& out_likes);
    bool AddComment(long long blog_id, long long user_id, const std::string& user_name, const std::string& content, int& out_comments);
    std::vector<CommentItem> GetComments(long long blog_id, int limit);

    // Friends & requests
    bool StoreFriendApply(const std::string& from_user, const std::string& to_user, const std::string& hello);
    bool AcceptFriend(const std::string& from_user, const std::string& to_user);
    std::vector<FRIEND_REQUEST_ITEM> GetPendingFriendRequests(const std::string& user_id);
    std::vector<FRIEND_ITEM> GetFriendList(const std::string& user_id);
    struct UnreadMessage {
        std::string sender_id;
        std::string content;
        std::string created_at;
    };
    std::vector<UnreadMessage> GetUnreadMessages(const std::string& user_id, int limit);

    // ========== ENHANCED: Daily Assessment Functions ==========
    
    // Question item structure
    struct QuestionItem {
        int question_id{0};
        int seq{0};
        std::string stem;
        std::string dimension_code;
        std::string options_json;
        std::string scoring_rule;
    };
    
    // Complete questionnaire data
    struct QuestionnaireData {
        bool ok{false};
        int questionnaire_id{0};
        std::string questionnaire_code;
        std::string title;
        int expected_duration{0};
        std::vector<QuestionItem> questions;
    };
    
    // Answer data for submission
    struct AnswerData {
        int question_id{0};
        std::string option_code;
        float option_score{0};
        std::string dimension_code;
    };
    
    // Enhanced submit result
    struct SubmitResult {
        int status_code{0};
        std::string assessment_tag;
        float total_score{0};
        std::string risk_level;
        std::map<std::string, float> dimension_scores;
        bool need_followup{false};
        std::string suggestion;
    };
    
    // Trend point for charts
    struct TrendPoint {
        std::string date;
        float score{0};
    };
    
    // Indicator with trend data
    struct IndicatorItem {
        std::string dimension_code;
        float last_score{0};
        float avg_7d{0};
        float avg_30d{0};
        std::string trend;
        std::string risk_level;
        std::vector<TrendPoint> trend_data;
    };
    
    // Detailed report
    struct ReportData {
        int status_code{0};
        std::vector<IndicatorItem> indicators;
        std::string summary;
    };

    // Check if user completed today's assessment
    bool HasCompletedToday(long long user_id, const std::string& date);
    
    // Fetch complete daily questionnaire
    QuestionnaireData FetchDailyQuestionnaire(long long user_id, const std::string& date);
    
    // Submit complete assessment (all answers at once)
    SubmitResult SubmitCompleteAssessment(
        long long user_id,
        int questionnaire_id,
        const std::string& questionnaire_code,
        const std::string& assessment_tag,
        const std::vector<AnswerData>& answers
    );
    
    // Fetch detailed report with trends
    ReportData FetchDetailedReport(long long user_id, int scope_days);

    // ========== NEW: Daily "1+1" Assessment Mode ==========
    
    // Daily question structure (for 1+1 mode)
    struct DailyQuestion {
        int question_id{0};
        std::string stem;
        std::string options_json;
        float max_score{5.0f};
    };
    
    // Result of getting today's 2 questions
    struct DailyQuestionsResult {
        bool ok{false};
        int status_code{0};  // 0=success, 1=already_completed, 2=error
        DailyQuestion question1;  // Emotion snapshot
        DailyQuestion question2;  // Core symptom
    };
    
    // Result of submitting today's assessment
    struct DailySubmitResult {
        bool ok{false};
        float emotion_index{0};  // 0-100
        std::string feedback;
        std::string risk_level;
    };
    
    // Check if user completed today's daily assessment
    bool CheckTodayDailyCompleted(long long user_id, const std::string& date);
    
    // Get today's 2 questions (1 emotion + 1 symptom based on weekday)
    DailyQuestionsResult GetTodayDailyQuestions(long long user_id, const std::string& date);
    
    // Submit today's daily assessment (2 answers)
    DailySubmitResult SubmitTodayDaily(
        long long user_id,
        const std::string& date,
        int question1_id,
        int answer1,
        float score1,
        int question2_id,
        int answer2,
        float score2
    );

    // ========== LEGACY: Keep for backward compatibility ==========
    struct QuestionData {
        bool ok{false};
        std::string questionnaire_code;
        std::string question_id;
        std::string stem;
        std::string options_json;
    };
    QuestionData FetchDailyQuestion();
    SubmitResult SubmitAnswer(const SUBMIT_ANSWER_REQ& req);
    std::vector<REPORT_ITEM> FetchIndicators(long long user_id);
    void RecordRiskEvent(long long user_id, const std::string& assessment_tag, int alert_level);

private:
    std::unique_ptr<SQLiteDB> openDB();
    long long ensureUserId(SQLiteDB* db, const std::string& user_id_str);
    long long questionnaireIdByCode(SQLiteDB* db, const std::string& code);
    
    // Helper functions for enhanced features
    std::string SelectQuestionnaireByDate(const std::string& date);
    std::string AssessRiskLevel(const std::string& questionnaire_code, float total_score);
    std::string GenerateSuggestion(const std::string& risk_level, const std::string& questionnaire_code);
    void UpdateIndicators(SQLiteDB* db, long long user_id, const std::map<std::string, float>& dimension_scores);
    std::string CalculateTrend(float last, float avg7d, float avg30d);

    void EnsureBlogTables(SQLiteDB* db);
};
