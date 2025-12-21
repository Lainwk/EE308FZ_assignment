#pragma once

// Service types
enum service_type {
    SMS_CODE = 0,
    LOGIN = 1,
    REGISTER = 2,
    BLOG = 3,
    MESSAGE = 4,
    PULL_DAILY_QUESTION = 5,      // Legacy: single question
    SUBMIT_ANSWER = 6,              // Legacy: single answer
    FETCH_REPORT = 7,               // Legacy: simple report
    ALERT_ACK = 8,
    PULL_QUESTIONNAIRE = 9,         // Enhanced: complete questionnaire
    SUBMIT_ASSESSMENT = 10,         // Enhanced: complete assessment
    FETCH_DETAILED_REPORT = 11,      // Enhanced: detailed report with trends
    FRIEND_APPLY = 12,
    FRIEND_LIST = 13,
    FRIEND_PENDING = 14,
    FRIEND_ACCEPT = 15,
    MESSAGE_PULL = 16,
    BLOG_FEED = 17,
    BLOG_LIKE = 18,
    BLOG_COMMENT = 19,
    BLOG_COMMENT_LIST = 20,
    ASSESSMENT_HISTORY = 21,         // New: get assessment history list
    CHECK_TODAY_ASSESSMENT = 22,     // New: check if today's assessment is completed
    AI_CHAT = 23,                    // New: AI chat service
    MESSAGE_PUSH = 24,                // New: server push message to client
    CHANGE_PASSWORD = 25  // Change password
};

struct HEAD {
    int service_type;
    int data_len;
};

// SMS
struct SMS_CODE_REQ {
    char user_phone[12];
};

struct SMS_CODE_RESP {
    int status_code;
};

// Login
struct LOGIN_REQ {
    char user_phone[12];
    char user_pwd[33];
    char code[5];
};

struct LOGIN_RESP {
    int status_code;
    char user_name[32];
    char user_id[32];
    int profile_picture_index;
    int friend_num;
};

// Friend item (renamed to avoid enum name clash)
struct FRIEND_ITEM {
    char friend_id[32];
    char friend_name[32];
    char profile_picture_index;
};

struct FRIEND_APPLY_REQ {
    char from_user[32];
    char to_user[32];
    char hello[64];
};

struct FRIEND_APPLY_RESP {
    int status_code;
};

struct FRIEND_LIST_RESP {
    int status_code;
    int friend_count;
    // FRIEND_ITEM array follows
};

struct FRIEND_REQUEST_ITEM {
    char from_user[32];
    char from_name[32];
    char hello[64];
    char created_at[20];
};

struct FRIEND_PENDING_RESP {
    int status_code;
    int request_count;
    // FRIEND_REQUEST_ITEM array follows
};

// Register
struct REGISTER_REQ {
    char user_phone[12];
    char user_name[9];
    char user_pwd[33];
    char code[5];
};

struct REGISTER_RESP {
    int status_code;
};

// Blog / Message
struct BLOG_REQ {
    char user_id[32];
    int is_anonymous;
    int content_len;
    // content follows
};

struct BLOG_RESP {
    int status_code;
    char blog_id[10];
};

struct BLOG_FEED_REQ {
    char user_id[32];
    int limit;
};

struct BLOG_ITEM {
    char blog_id[10];
    char author_id[32];
    char author_name[32];
    int is_anonymous;
    int likes;
    int comments;
    char created_at[20];
    int content_len;
    // content follows
};

struct BLOG_FEED_RESP {
    int status_code;
    int blog_count;
    // BLOG_ITEM + content bytes follow
};

struct BLOG_LIKE_REQ {
    char blog_id[10];
    char user_id[32];
};

struct BLOG_LIKE_RESP {
    int status_code;
    int likes;
};

struct BLOG_COMMENT_REQ {
    char blog_id[10];
    char user_id[32];
    char user_name[32];
    int content_len;
    // content follows
};

struct BLOG_COMMENT_RESP {
    int status_code;
    int comments;
};

struct BLOG_COMMENT_ITEM {
    char author_name[32];
    char created_at[20];
    int content_len;
    // content follows
};

struct BLOG_COMMENT_LIST_REQ {
    char blog_id[10];
    int limit;
};

struct BLOG_COMMENT_LIST_RESP {
    int status_code;
    int comment_count;
    // BLOG_COMMENT_ITEM + content bytes follow
};

struct MESSAGE_REQ {
    char sender_id[32];
    char receiver_id[32];
    int content_len;
    // content follows
};

struct MESSAGE_RESP {
    int status_code;
};

struct MESSAGE_ITEM {
    char sender_id[32];
    int content_len;
    char created_at[20];
};

struct MESSAGE_LIST_RESP {
    int status_code;
    int message_count;
    // MESSAGE_ITEM + content bytes follow
};

// ========== Assessment (Legacy - Single Question Mode) ==========
struct PULL_DAILY_QUESTION_REQ {
    char user_id[32];
};

struct PULL_DAILY_QUESTION_RESP {
    int status_code;
    char questionnaire_code[16];
    char question_id[10];
    int stem_len;
    int options_len;
    // stem + options follow
};

struct SUBMIT_ANSWER_REQ {
    char user_id[32];
    char questionnaire_code[16];
    char assessment_id[16];
    char question_id[10];
    char option_code[8];
    float option_score;
    long long client_ts;
};

struct SUBMIT_ANSWER_RESP {
    int status_code;
    char assessment_id[16];
    int need_followup;
};

struct FETCH_REPORT_REQ {
    char user_id[32];
    int scope_days;
};

struct REPORT_ITEM {
    char dimension_code[8];
    float last_score;
    float avg_7d;
    float avg_30d;
    char trend;
};

struct FETCH_REPORT_RESP {
    int status_code;
    int item_count;
    // items follow
};

// ========== Enhanced Assessment (Complete Questionnaire Mode) ==========

// Question item in questionnaire
struct QUESTION_ITEM {
    int question_id;
    int seq;
    char dimension_code[16];
    int stem_len;
    int options_len;
    // stem + options_json follow
};

// Pull complete questionnaire request
struct PULL_QUESTIONNAIRE_REQ {
    char user_id[32];
    char date[11];  // YYYY-MM-DD
};

// Pull complete questionnaire response
struct PULL_QUESTIONNAIRE_RESP {
    int status_code;
    int questionnaire_id;
    char questionnaire_code[16];
    char title[64];
    int expected_duration;
    int question_count;
    int total_data_len;  // total length of all question data
    // QUESTION_ITEM array + all stems and options follow
};

// Answer item for submission
struct ANSWER_ITEM {
    int question_id;
    char option_code[8];
    float option_score;
    char dimension_code[16];
};

// Submit complete assessment request
struct SUBMIT_ASSESSMENT_REQ {
    char user_id[32];
    int questionnaire_id;
    char questionnaire_code[16];
    char assessment_tag[32];
    int answer_count;
    // ANSWER_ITEM array follows
};

// Submit complete assessment response
struct SUBMIT_ASSESSMENT_RESP {
    int status_code;
    char assessment_tag[32];
    float total_score;
    char risk_level[8];  // green/yellow/orange/red
    int need_followup;
    int suggestion_len;
    int dimension_count;
    // suggestion text + dimension scores follow
};

// Dimension score item
struct DIMENSION_SCORE {
    char dimension_code[16];
    float score;
};

// Trend data point
struct TREND_POINT {
    char date[11];  // YYYY-MM-DD
    float score;
};

// Enhanced indicator item with trend data
struct INDICATOR_ITEM {
    char dimension_code[16];
    float last_score;
    float avg_7d;
    float avg_30d;
    char trend[8];  // up/down/flat
    char risk_level[8];
    int trend_point_count;
    // TREND_POINT array follows
};

// Fetch detailed report request
struct FETCH_DETAILED_REPORT_REQ {
    char user_id[32];
    int scope_days;
};

// Fetch detailed report response
struct FETCH_DETAILED_REPORT_RESP {
    int status_code;
    int indicator_count;
    int summary_len;
    int total_data_len;  // total length of all indicator data + summary
    // INDICATOR_ITEM array + trend points + summary text follow
};

struct ALERT_ACK_REQ {
    char user_id[32];
    char assessment_id[16];
    int alert_level;
};

struct ALERT_ACK_RESP {
    int status_code;
};

// ========== Assessment History (New Feature) ==========

// Assessment history request
struct ASSESSMENT_HISTORY_REQ {
    char user_id[32];
    int limit;        // Number of records to return, 0 = all
    int offset;       // Pagination offset
};

// Assessment record item
struct ASSESSMENT_RECORD_ITEM {
    int record_id;
    char completed_time[32];  // "YYYY-MM-DD HH:MM:SS"
    float total_score;
    char risk_level[16];
    char suggestion[256];
};

// Assessment history response
struct ASSESSMENT_HISTORY_RESP {
    int status_code;
    int record_count;
    // ASSESSMENT_RECORD_ITEM array follows
};

// Check today's assessment request
struct CHECK_TODAY_ASSESSMENT_REQ {
    char user_id[32];
};

// Check today's assessment response
struct CHECK_TODAY_ASSESSMENT_RESP {
    int status_code;
    int completed;  // 0=not completed, 1=completed
    char last_completed_time[32];
    float last_score;
};

// AI Chat request
struct AI_CHAT_REQ {
    int user_id;
    char message[2048];  // User's message to AI
};

// AI Chat response
struct AI_CHAT_RESP {
    int status_code;  // 0=success, 1=error
    char ai_response[4096];  // AI's response message
    char error_msg[256];  // Error message if failed
};

// Change Password request
struct CHANGE_PASSWORD_REQ {
    char user_id[32];
    char old_password[33];  // MD5 hash (32 chars + null terminator)
    char new_password[33];  // MD5 hash (32 chars + null terminator)
};

// Change Password response
struct CHANGE_PASSWORD_RESP {
    int status_code;  // 0=success, 1=wrong old password, 2=other error
    char message[128];  // Error or success message
};

#define PACKAGESIZE 8192

struct MSGBUF {
    long mtype;
    char mtext[PACKAGESIZE];
    int client_fd;
};

struct ReadResult {
    int index;
    int client_fd;
    char data[PACKAGESIZE];
};
