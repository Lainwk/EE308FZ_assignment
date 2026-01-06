#include "assessmentcache.h"
#include <QDebug>
#include <cstring>

AssessmentCache& AssessmentCache::instance()
{
    static AssessmentCache inst;
    return inst;
}

bool AssessmentCache::hasHistoryCache(const QString& userId) const
{
    return m_historyCache.contains(userId) && !m_historyCache[userId].isEmpty();
}

QVector<CachedAssessmentRecord> AssessmentCache::getHistoryCache(const QString& userId) const
{
    return m_historyCache.value(userId);
}

void AssessmentCache::setHistoryCache(const QString& userId, const QVector<CachedAssessmentRecord>& records)
{
    m_historyCache[userId] = records;
    qDebug() << "[AssessmentCache] History cached for user:" << userId << "count:" << records.size();
}

void AssessmentCache::clearHistoryCache(const QString& userId)
{
    m_historyCache.remove(userId);
    qDebug() << "[AssessmentCache] History cache cleared for user:" << userId;
}

bool AssessmentCache::hasTodayStatusCache(const QString& userId) const
{
    if (!m_todayStatusCache.contains(userId)) {
        return false;
    }
    return m_todayStatusCache[userId].isValid();
}

CachedTodayStatus AssessmentCache::getTodayStatusCache(const QString& userId) const
{
    return m_todayStatusCache.value(userId);
}

void AssessmentCache::setTodayStatusCache(const QString& userId, const CachedTodayStatus& status)
{
    m_todayStatusCache[userId] = status;
    qDebug() << "[AssessmentCache] Today status cached for user:" << userId 
             << "completed:" << status.completed;
}

void AssessmentCache::clearTodayStatusCache(const QString& userId)
{
    m_todayStatusCache.remove(userId);
    qDebug() << "[AssessmentCache] Today status cache cleared for user:" << userId;
}

void AssessmentCache::clearAll()
{
    m_historyCache.clear();
    m_todayStatusCache.clear();
    qDebug() << "[AssessmentCache] All cache cleared";
}

void AssessmentCache::clearUser(const QString& userId)
{
    clearHistoryCache(userId);
    clearTodayStatusCache(userId);
    qDebug() << "[AssessmentCache] All cache cleared for user:" << userId;
}

bool AssessmentCache::loadHistoryFromServer(const QString& userId, int limit)
{
    qDebug() << "[AssessmentCache] Loading history from server for user:" << userId << "limit:" << limit;
    
    // 构造请求
    ASSESSMENT_HISTORY_REQ req;
    memset(&req, 0, sizeof(req));
    strncpy(req.user_id, userId.toStdString().c_str(), sizeof(req.user_id) - 1);
    req.limit = limit;
    req.offset = 0;
    
    QByteArray reqData((const char*)&req, sizeof(req));
    
    // 发送请求并接收响应
    HEAD respHead;
    QByteArray respBody = FrontClient::instance().sendAndReceive(
        ASSESSMENT_HISTORY, reqData, &respHead);
    
    if (respBody.isEmpty() || respBody.size() < (int)sizeof(ASSESSMENT_HISTORY_RESP)) {
        qDebug() << "[AssessmentCache] Failed to load history from server";
        return false;
    }
    
    // 解析响应
    ASSESSMENT_HISTORY_RESP* resp = (ASSESSMENT_HISTORY_RESP*)respBody.data();
    
    if (resp->status_code != 0) {
        qDebug() << "[AssessmentCache] Server returned error status:" << resp->status_code;
        return false;
    }
    
    // 解析记录列表
    QVector<CachedAssessmentRecord> records;
    const char* ptr = respBody.data() + sizeof(ASSESSMENT_HISTORY_RESP);
    
    for (int i = 0; i < resp->record_count; i++) {
        if (ptr + sizeof(ASSESSMENT_RECORD_ITEM) > respBody.data() + respBody.size()) {
            qDebug() << "[AssessmentCache] Buffer overflow when parsing record" << i;
            break;
        }
        
        ASSESSMENT_RECORD_ITEM* item = (ASSESSMENT_RECORD_ITEM*)ptr;
        
        CachedAssessmentRecord record;
        record.recordId = item->record_id;
        record.completedTime = QString::fromUtf8(item->completed_time);
        record.totalScore = item->total_score;
        record.riskLevel = QString::fromUtf8(item->risk_level);
        record.suggestion = QString::fromUtf8(item->suggestion);
        
        records.append(record);
        
        ptr += sizeof(ASSESSMENT_RECORD_ITEM);
    }
    
    // 缓存数据
    setHistoryCache(userId, records);
    
    qDebug() << "[AssessmentCache] Loaded and cached" << records.size() << "records";
    return true;
}

bool AssessmentCache::loadTodayStatusFromServer(const QString& userId)
{
    qDebug() << "[AssessmentCache] Loading today status from server for user:" << userId;
    
    // 构造请求
    CHECK_TODAY_ASSESSMENT_REQ req;
    memset(&req, 0, sizeof(req));
    strncpy(req.user_id, userId.toStdString().c_str(), sizeof(req.user_id) - 1);
    
    QByteArray reqData((const char*)&req, sizeof(req));
    
    // 发送请求并接收响应
    HEAD respHead;
    QByteArray respBody = FrontClient::instance().sendAndReceive(
        CHECK_TODAY_ASSESSMENT, reqData, &respHead);
    
    if (respBody.isEmpty() || respBody.size() < (int)sizeof(CHECK_TODAY_ASSESSMENT_RESP)) {
        qDebug() << "[AssessmentCache] Failed to load today status from server";
        return false;
    }
    
    // 解析响应
    CHECK_TODAY_ASSESSMENT_RESP* resp = (CHECK_TODAY_ASSESSMENT_RESP*)respBody.data();
    
    if (resp->status_code != 0) {
        qDebug() << "[AssessmentCache] Server returned error status:" << resp->status_code;
        return false;
    }
    
    // 构建缓存数据
    CachedTodayStatus status;
    status.completed = (resp->completed == 1);
    status.lastCompletedTime = QString::fromUtf8(resp->last_completed_time);
    status.lastScore = resp->last_score;
    status.cacheTime = QDateTime::currentDateTime();
    
    // 缓存数据
    setTodayStatusCache(userId, status);
    
    qDebug() << "[AssessmentCache] Today status loaded and cached, completed:" << status.completed;
    return true;
}