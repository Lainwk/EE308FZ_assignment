#pragma once

// Service types
enum service_type {
    SMS_CODE = 0,
    LOGIN = 1,
    REGISTER = 2,
    BLOG = 3,
    MESSAGE = 4,
    PULL_DAILY_QUESTION = 5,
    SUBMIT_ANSWER = 6,
    FETCH_REPORT = 7,
    ALERT_ACK = 8,
    PULL_QUESTIONNAIRE = 9,
    SUBMIT_ASSESSMENT = 10,
    FETCH_DETAILED_REPORT = 11,
    FRIEND_APPLY = 12,
    FRIEND_LIST = 13,
    FRIEND_PENDING = 14,
    FRIEND_ACCEPT = 15,
    MESSAGE_PULL = 16,
    BLOG_FEED = 17,
    BLOG_LIKE = 18,
    BLOG_COMMENT = 19,
    BLOG_COMMENT_LIST = 20,
    ASSESSMENT_HISTORY = 21,
    CHECK_TODAY_ASSESSMENT = 22,
    AI_CHAT = 23,
    MESSAGE_PUSH = 24,  // Server push message to client
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

// Change Password
struct CHANGE_PASSWORD_REQ {
    char user_id[32];
    char old_password[33];
    char new_password[33];
};

struct CHANGE_PASSWORD_RESP {
    int status_code;  // 0=success, 1=wrong old password, 2=other error
    char message[128];
};

// Friend item (renamed to avoid enum name clash)
struct FRIEND_ITEM {
    char friend_id[32];
    char friend_name[32];
    char profile_picture_index;
};

// Friend apply / list
struct FRIEND_APPLY_REQ {
    char from_user[32];
    char to_user[32];
    char hello[64];
};

struct FRIEND_APPLY_RESP {
    int status_code; // 0=ok
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

// Assessment
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

struct ALERT_ACK_REQ {
    char user_id[32];
    char assessment_id[16];
    int alert_level;
};

struct ALERT_ACK_RESP {
    int status_code;
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
