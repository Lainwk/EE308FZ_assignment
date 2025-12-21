#pragma once

#include <QObject>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStandardPaths>
#include <QVector>

struct SessionData {
    QString phone;
    QString pwd;
    QString code;
    QString userId;
    QString userName;
    int avatarIndex = 0;
};

struct MessageRecord {
    QString convId;
    QString senderId;
    QString content;
    qint64 timestampMs = 0;
    int direction = 0; // 0 outgoing, 1 incoming
};

struct PostRecord {
    int id = 0;
    QString authorId;
    QString authorName;
    QString content;
    qint64 timestampMs = 0;
    int likes = 0;
    int comments = 0;
};

struct CommentRecord {
    int id = 0;
    int postId = 0;
    QString authorId;
    QString authorName;
    QString content;
    qint64 timestampMs = 0;
};

// 本地存储：会话凭证 + 聊天记录 + 社区动态(SQLite)
class LocalStore : public QObject {
    Q_OBJECT
public:
    static LocalStore& instance();

    bool init();

    bool hasSession() const;
    SessionData loadSession() const;
    void saveSession(const SessionData& s);
    void clearSession();

    // 消息存储/读取
    bool saveMessage(const QString& convId, const QString& senderId, const QString& content, qint64 timestampMs, int direction);
    QVector<MessageRecord> loadMessages(const QString& convId, int limit = 100) const;

    // 社区帖子与评论
    bool savePost(const PostRecord& post);
    QVector<PostRecord> loadPosts(int limit = 50) const;
    bool likePost(int postId);
    bool addComment(int postId, const QString& authorId, const QString& authorName, const QString& content);
    QVector<CommentRecord> loadComments(int postId, int limit = 10) const;

private:
    LocalStore();
    QString dbPath() const;
    bool ensureTables();

    QSqlDatabase db;
    QSettings settings;
};
