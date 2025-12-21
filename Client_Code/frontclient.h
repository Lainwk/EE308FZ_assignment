#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QString>
#include <QTimer>

// 业务类型枚举（与服务端保持一致）
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

// 包头
struct HEAD {
    int service_type;
    int data_len;
};

// 请求体结构（客户端构包使用）
struct SMS_CODE_REQ {
    char user_phone[12];
};

struct LOGIN_REQ {
    char user_phone[12];
    char user_pwd[33];
    char code[5];
};

struct REGISTER_REQ {
    char user_phone[12];
    char user_name[9];
    char user_pwd[33];
    char code[5];
};

struct CHANGE_PASSWORD_REQ {
    char user_id[32];
    char old_password[33];
    char new_password[33];
};

struct CHANGE_PASSWORD_RESP {
    int status_code;  // 0=成功, 1=原密码错误, 2=其他错误
    char message[128];
};

// 响应体结构（客户端解析使用）
struct LOGIN_RESP {
    int status_code;
    char user_name[32];
    char user_id[32];
    int profile_picture_index;
    int friend_num;
};

struct REGISTER_RESP {
    int status_code;
};

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
};

struct BLOG_REQ {
    char user_id[32];
    int is_anonymous;
    int content_len;
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
};

struct BLOG_FEED_RESP {
    int status_code;
    int blog_count;
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
};

struct BLOG_COMMENT_RESP {
    int status_code;
    int comments;
};

struct BLOG_COMMENT_ITEM {
    char author_name[32];
    char created_at[20];
    int content_len;
};

struct BLOG_COMMENT_LIST_REQ {
    char blog_id[10];
    int limit;
};

struct BLOG_COMMENT_LIST_RESP {
    int status_code;
    int comment_count;
};

struct MESSAGE_REQ {
    char sender_id[32];
    char receiver_id[32];
    int content_len;
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
};

struct QUESTION_ITEM {
    int question_id;
    int seq;
    char dimension_code[16];
    int stem_len;
    int options_len;
};

struct PULL_QUESTIONNAIRE_REQ {
    char user_id[32];
    char date[11];
};

struct PULL_QUESTIONNAIRE_RESP {
    int status_code;
    int questionnaire_id;
    char questionnaire_code[16];
    char title[64];
    int expected_duration;
    int question_count;
    int total_data_len;
};

struct ANSWER_ITEM {
    int question_id;
    char option_code[8];
    float option_score;
    char dimension_code[16];
};

struct SUBMIT_ASSESSMENT_REQ {
    char user_id[32];
    int questionnaire_id;
    char questionnaire_code[16];
    char assessment_tag[32];
    int answer_count;
};

struct SUBMIT_ASSESSMENT_RESP {
    int status_code;
    char assessment_tag[32];
    float total_score;
    char risk_level[8];
    int need_followup;
    int suggestion_len;
    int dimension_count;
};

struct DIMENSION_SCORE {
    char dimension_name[32];
    float score;
};

struct TREND_POINT {
    char date[11];
    float score;
};

struct INDICATOR_ITEM {
    char indicator_name[32];
    float current_value;
    float avg_value;
};

struct FETCH_DETAILED_REPORT_REQ {
    char user_id[32];
    int days;
};

struct FETCH_DETAILED_REPORT_RESP {
    int status_code;
    int dimension_count;
    int trend_count;
    int indicator_count;
};

struct ASSESSMENT_HISTORY_REQ {
    char user_id[32];
    int limit;
    int offset;
};

struct ASSESSMENT_RECORD_ITEM {
    int record_id;
    char completed_time[32];
    float total_score;
    char risk_level[16];
    char suggestion[256];
};

struct ASSESSMENT_HISTORY_RESP {
    int status_code;
    int record_count;
};

struct CHECK_TODAY_ASSESSMENT_REQ {
    char user_id[32];
};

struct CHECK_TODAY_ASSESSMENT_RESP {
    int status_code;
    int completed;
    char last_completed_time[32];
    float last_score;
};

struct AI_CHAT_REQ {
    int user_id;
    char message[2048];
};

struct AI_CHAT_RESP {
    int status_code;
    char ai_response[4096];
    char error_msg[256];
};

// 双Socket架构的FrontClient类
class FrontClient : public QObject {
    Q_OBJECT
public:
    static FrontClient& instance();

    bool connectToServer(const QString& host = "127.0.0.1", int port = 10001);
    bool isConnected() const;
    bool sendPacket(const QByteArray& packet);
    bool sendSimple(int serviceType, const QByteArray& body);
    QByteArray sendAndReceive(int serviceType, const QByteArray& body, HEAD* outHead = nullptr);

    void enableMessagePush(bool enable);
    void setUserId(const QString& userId);

signals:
    void messageReceived(const QString& senderId, const QString& content, qint64 timestamp);
    void connectionStateChanged(bool connected);
    void pushConnectionStateChanged(bool connected);

private slots:
    void onPushSocketReadyRead();
    void onPushSocketDisconnected();
    void onPushSocketError(QAbstractSocket::SocketError error);
    void onPushSocketConnected();
    void onReconnectTimer();

private:
    FrontClient();
    bool ensureConnected();
    void processPushData();
    void startReconnectTimer();
    void stopReconnectTimer();
    void connectPushSocket();

    QTcpSocket mainSocket;          // 主socket：用于同步请求/响应
    QString currentHost = "127.0.0.1";
    int currentPort = 10001;

    QTcpSocket* pushSocket = nullptr;  // 推送socket：专门接收服务器推送
    QByteArray pushBuffer;
    bool pushEnabled = false;
    QString currentUserId;

    QTimer* reconnectTimer = nullptr;
    bool shouldReconnect = false;
};
