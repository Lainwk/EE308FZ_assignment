#include "localstore.h"

#include <QDir>
#include <QFileInfo>
#include <algorithm>

LocalStore& LocalStore::instance() {
    static LocalStore inst;
    return inst;
}

LocalStore::LocalStore()
    : settings("MindCarrer", "Client") {
}

QString LocalStore::dbPath() const {
    QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (base.isEmpty()) {
        base = QDir::homePath() + "/.mindcarrer";
    }
    QDir().mkpath(base);
    
    // 根据用户ID生成独立的数据库文件
    if (m_currentUserId.isEmpty()) {
        // 未登录时使用临时数据库
        return base + "/temp_cache.sqlite";
    }
    return base + QString("/user_%1.sqlite").arg(m_currentUserId);
}

bool LocalStore::init() {
    if (db.isOpen()) return true;
    db = QSqlDatabase::addDatabase("QSQLITE", "local_store");
    db.setDatabaseName(dbPath());
    if (!db.open()) {
        return false;
    }
    return ensureTables();
}

bool LocalStore::ensureTables() {
    QSqlQuery q(db);
    // 会话表 - 添加owner_id字段
    if (!q.exec("CREATE TABLE IF NOT EXISTS conversations ("
                "conv_id TEXT PRIMARY KEY,"
                "owner_id TEXT NOT NULL DEFAULT '',"
                "last_msg_time INTEGER DEFAULT 0,"
                "unread_count INTEGER DEFAULT 0"
                ");")) return false;
    
    // 消息表 - 添加owner_id字段
    if (!q.exec("CREATE TABLE IF NOT EXISTS messages ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "conv_id TEXT,"
                "owner_id TEXT NOT NULL DEFAULT '',"
                "sender_id TEXT,"
                "content TEXT,"
                "timestamp_ms INTEGER,"
                "direction INTEGER,"
                "status INTEGER DEFAULT 0"
                ");")) return false;
    
    // 为owner_id创建索引，提高查询性能
    q.exec("CREATE INDEX IF NOT EXISTS idx_messages_owner ON messages(owner_id);");
    q.exec("CREATE INDEX IF NOT EXISTS idx_conversations_owner ON conversations(owner_id);");
    if (!q.exec("CREATE TABLE IF NOT EXISTS posts ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "author_id TEXT,"
                "author_name TEXT,"
                "content TEXT,"
                "timestamp_ms INTEGER,"
                "likes INTEGER DEFAULT 0,"
                "comments INTEGER DEFAULT 0"
                ");")) return false;
    if (!q.exec("CREATE TABLE IF NOT EXISTS comments ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "post_id INTEGER,"
                "author_id TEXT,"
                "author_name TEXT,"
                "content TEXT,"
                "timestamp_ms INTEGER"
                ");")) return false;
    return true;
}

bool LocalStore::hasSession() const {
    return settings.contains("session/phone") && settings.contains("session/pwd");
}

SessionData LocalStore::loadSession() const {
    SessionData s;
    s.phone = settings.value("session/phone").toString();
    s.pwd = settings.value("session/pwd").toString();
    s.code = settings.value("session/code").toString();
    s.userId = settings.value("session/userId").toString();
    s.userName = settings.value("session/userName").toString();
    s.avatarIndex = settings.value("session/avatarIndex", 0).toInt();
    return s;
}

void LocalStore::saveSession(const SessionData& s) {
    settings.setValue("session/phone", s.phone);
    settings.setValue("session/pwd", s.pwd);
    settings.setValue("session/code", s.code);
    settings.setValue("session/userId", s.userId);
    settings.setValue("session/userName", s.userName);
    settings.setValue("session/avatarIndex", s.avatarIndex);
    settings.sync();
}

void LocalStore::clearSession() {
    settings.remove("session");
    settings.sync();
}

void LocalStore::setCurrentUserId(const QString& userId) {
    if (m_currentUserId == userId) {
        return;  // 用户ID未变化，无需切换
    }
    
    // 关闭当前数据库连接
    if (db.isOpen()) {
        db.close();
        QSqlDatabase::removeDatabase("local_store");
    }
    
    // 设置新用户ID
    m_currentUserId = userId;
    
    // 重新初始化数据库（会使用新的dbPath）
    if (!userId.isEmpty()) {
        init();
    }
}

QString LocalStore::getCurrentUserId() const {
    return m_currentUserId;
}

bool LocalStore::saveMessage(const QString& convId, const QString& senderId, const QString& content, qint64 timestampMs, int direction) {
    // 验证当前用户ID
    if (m_currentUserId.isEmpty()) {
        qWarning() << "[LocalStore] Cannot save message: no current user";
        return false;
    }
    
    // 插入消息记录（包含owner_id）
    QSqlQuery q(db);
    q.prepare("INSERT INTO messages (conv_id, owner_id, sender_id, content, timestamp_ms, direction, status) "
              "VALUES (?, ?, ?, ?, ?, ?, 0)");
    q.addBindValue(convId);
    q.addBindValue(m_currentUserId);  // 添加owner_id
    q.addBindValue(senderId);
    q.addBindValue(content);
    q.addBindValue(timestampMs);
    q.addBindValue(direction);
    if (!q.exec()) {
        qWarning() << "[LocalStore] Failed to save message:" << q.lastError().text();
        return false;
    }

    // 更新会话表（包含owner_id）
    QSqlQuery upd(db);
    upd.prepare("INSERT INTO conversations (conv_id, owner_id, last_msg_time, unread_count) "
                "VALUES (?, ?, ?, 0) "
                "ON CONFLICT(conv_id) DO UPDATE SET last_msg_time=excluded.last_msg_time");
    upd.addBindValue(convId);
    upd.addBindValue(m_currentUserId);  // 添加owner_id
    upd.addBindValue(timestampMs);
    return upd.exec();
}

QVector<MessageRecord> LocalStore::loadMessages(const QString& convId, int limit) const {
    QVector<MessageRecord> res;
    
    // 验证当前用户ID
    if (m_currentUserId.isEmpty()) {
        qWarning() << "[LocalStore] Cannot load messages: no current user";
        return res;
    }
    
    QSqlQuery q(db);
    // 添加owner_id过滤条件
    q.prepare("SELECT conv_id, sender_id, content, timestamp_ms, direction "
              "FROM messages WHERE conv_id=? AND owner_id=? "
              "ORDER BY timestamp_ms DESC LIMIT ?");
    q.addBindValue(convId);
    q.addBindValue(m_currentUserId);  // 只加载当前用户的消息
    q.addBindValue(limit);
    
    if (q.exec()) {
        while (q.next()) {
            MessageRecord r;
            r.convId = q.value(0).toString();
            r.senderId = q.value(1).toString();
            r.content = q.value(2).toString();
            r.timestampMs = q.value(3).toLongLong();
            r.direction = q.value(4).toInt();
            res.append(r);
        }
    }
    std::reverse(res.begin(), res.end());
    return res;
}

bool LocalStore::savePost(const PostRecord& post) {
    QSqlQuery q(db);
    q.prepare("INSERT INTO posts (author_id, author_name, content, timestamp_ms, likes, comments) "
              "VALUES (?, ?, ?, ?, ?, ?)");
    q.addBindValue(post.authorId);
    q.addBindValue(post.authorName);
    q.addBindValue(post.content);
    q.addBindValue(post.timestampMs);
    q.addBindValue(post.likes);
    q.addBindValue(post.comments);
    return q.exec();
}

QVector<PostRecord> LocalStore::loadPosts(int limit) const {
    QVector<PostRecord> res;
    QSqlQuery q(db);
    q.prepare("SELECT id, author_id, author_name, content, timestamp_ms, likes, comments "
              "FROM posts ORDER BY timestamp_ms DESC LIMIT ?");
    q.addBindValue(limit);
    if (q.exec()) {
        while (q.next()) {
            PostRecord p;
            p.id = q.value(0).toInt();
            p.authorId = q.value(1).toString();
            p.authorName = q.value(2).toString();
            p.content = q.value(3).toString();
            p.timestampMs = q.value(4).toLongLong();
            p.likes = q.value(5).toInt();
            p.comments = q.value(6).toInt();
            res.append(p);
        }
    }
    return res;
}

bool LocalStore::likePost(int postId) {
    QSqlQuery q(db);
    q.prepare("UPDATE posts SET likes = likes + 1 WHERE id=?");
    q.addBindValue(postId);
    return q.exec();
}

bool LocalStore::addComment(int postId, const QString& authorId, const QString& authorName, const QString& content) {
    QSqlQuery q(db);
    q.prepare("INSERT INTO comments (post_id, author_id, author_name, content, timestamp_ms) "
              "VALUES (?, ?, ?, ?, strftime('%s','now')*1000)");
    q.addBindValue(postId);
    q.addBindValue(authorId);
    q.addBindValue(authorName);
    q.addBindValue(content);
    if (!q.exec()) return false;

    QSqlQuery upd(db);
    upd.prepare("UPDATE posts SET comments = comments + 1 WHERE id=?");
    upd.addBindValue(postId);
    return upd.exec();
}

QVector<CommentRecord> LocalStore::loadComments(int postId, int limit) const {
    QVector<CommentRecord> res;
    QSqlQuery q(db);
    q.prepare("SELECT id, post_id, author_id, author_name, content, timestamp_ms "
              "FROM comments WHERE post_id=? ORDER BY timestamp_ms DESC LIMIT ?");
    q.addBindValue(postId);
    q.addBindValue(limit);
    if (q.exec()) {
        while (q.next()) {
            CommentRecord c;
            c.id = q.value(0).toInt();
            c.postId = q.value(1).toInt();
            c.authorId = q.value(2).toString();
            c.authorName = q.value(3).toString();
            c.content = q.value(4).toString();
            c.timestampMs = q.value(5).toLongLong();
            res.append(c);
        }
    }
    std::reverse(res.begin(), res.end());
    return res;
}
