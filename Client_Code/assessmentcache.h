#ifndef ASSESSMENTCACHE_H
#define ASSESSMENTCACHE_H

#include <QString>
#include <QVector>
#include <QDateTime>
#include <QMap>
#include "frontclient.h"

// 缓存的测评记录
struct CachedAssessmentRecord {
    int recordId;
    QString completedTime;
    float totalScore;
    QString riskLevel;
    QString suggestion;
    
    CachedAssessmentRecord() : recordId(0), totalScore(0.0f) {}
};

// 缓存的今日状态
struct CachedTodayStatus {
    bool completed;
    QString lastCompletedTime;
    float lastScore;
    QDateTime cacheTime;
    
    CachedTodayStatus() : completed(false), lastScore(0.0f) {}
    
    bool isValid() const {
        // 缓存有效期：5分钟
        return cacheTime.isValid() && 
               cacheTime.secsTo(QDateTime::currentDateTime()) < 300;
    }
};

class AssessmentCache
{
public:
    static AssessmentCache& instance();
    
    // 历史记录缓存
    bool hasHistoryCache(const QString& userId) const;
    QVector<CachedAssessmentRecord> getHistoryCache(const QString& userId) const;
    void setHistoryCache(const QString& userId, const QVector<CachedAssessmentRecord>& records);
    void clearHistoryCache(const QString& userId);
    
    // 今日状态缓存
    bool hasTodayStatusCache(const QString& userId) const;
    CachedTodayStatus getTodayStatusCache(const QString& userId) const;
    void setTodayStatusCache(const QString& userId, const CachedTodayStatus& status);
    void clearTodayStatusCache(const QString& userId);
    
    // 清空所有缓存
    void clearAll();
    void clearUser(const QString& userId);
    
    // 从服务器加载并缓存
    bool loadHistoryFromServer(const QString& userId, int limit = 0);
    bool loadTodayStatusFromServer(const QString& userId);

private:
    AssessmentCache() = default;
    AssessmentCache(const AssessmentCache&) = delete;
    AssessmentCache& operator=(const AssessmentCache&) = delete;
    
    QMap<QString, QVector<CachedAssessmentRecord>> m_historyCache;
    QMap<QString, CachedTodayStatus> m_todayStatusCache;
};

#endif // ASSESSMENTCACHE_H