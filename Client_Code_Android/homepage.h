#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QListWidget>
#include <QTimer>
#include <QMap>
#include <QStringList>

// 前向声明
class DailyAssessmentPage;
class AssessmentReportPage;
class TrendChartWidget;

namespace Ui {
class HomePage;
}

class HomePage : public QWidget
{
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = nullptr);
    ~HomePage();
    
    // 设置用户信息
    void setUserInfo(const QString& userId, const QString& userName);

private slots:
    // 开始测评
    void onStartTestClicked();
    
    // 查看历史报告
    void onHistoryClicked();
    
    // 测评完成
    void onAssessmentCompleted(float score, const QString& riskLevel, const QString& suggestion);
    
    // 返回首页
    void onBackToHome();
    
    // 查看更多历史记录
    void onViewMoreHistoryClicked();
    
    // 历史记录项被点击
    void onHistoryItemClicked(QListWidgetItem* item);
    
    // 定时检查今日测评状态（无参数版本，用于定时器）
    void checkTodayAssessmentStatus();
    
    // 切换趋势图时间范围
    void onTimeRangeChanged(int days);

private:
    Ui::HomePage *ui;
    
    QString m_userId;
    QString m_userName;
    
    // 子页面
    DailyAssessmentPage* m_assessmentPage;
    AssessmentReportPage* m_reportPage;
    
    // 历史记录列表
    QListWidget* m_historyListWidget;
    
    // 趋势图
    TrendChartWidget* m_trendChart;
    
    // 定时器
    QTimer* m_checkTimer;
    
    // ✅ 每日心理小tips
    QMap<QString, QStringList> m_tipsLibrary;  // 按风险等级分类的tips库
    QString m_currentRiskLevel;                 // 当前风险等级
    
    void setupPages();
    void setupHistoryList();
    void setupTrendChart();
    void showHomePage();
    void showAssessmentPage();
    void showReportPage();
    
    // 测评历史功能（使用缓存优化）
    void loadRecentHistory(int limit = 5, bool forceRefresh = false);
    void updateHistoryList(const QVector<QPair<QString, QString>>& historyData);
    void showFullHistoryDialog();
    void showRecordDetailDialog(int recordId);
    
    // 趋势图功能
    void loadTrendData(int days = 7);
    void updateTrendChart();
    
    // 今日测评检查（内部使用，带参数）
    void checkTodayAssessmentStatusInternal(bool forceRefresh);
    void updateTodayAssessmentStatus(bool completed, const QString& lastTime, float lastScore);
    void showTodayReminderIfNeeded();
    int calculateMoodFromRecentHistory(const QString& currentRiskLevel);
    
    // ✅ 每日心理小tips功能
    void initializeTipsLibrary();              // 初始化tips库
    QString getDailyTip(const QString& category = "general");  // 获取每日tip
    void updateDailyTip();                     // 更新显示的tip
};

#endif // HOMEPAGE_H
