#ifndef ASSESSMENTREPORTPAGE_H
#define ASSESSMENTREPORTPAGE_H

#include <QWidget>
#include <QVector>
#include "frontclient.h"

namespace Ui {
class AssessmentReportPage;
}

// 维度得分数据
struct DimensionData {
    QString name;
    float score;
};

// 趋势数据点
struct TrendData {
    QString date;
    float score;
};

// 指标数据
struct IndicatorData {
    QString name;
    float currentValue;
    float avgValue;
};

class AssessmentReportPage : public QWidget
{
    Q_OBJECT

public:
    explicit AssessmentReportPage(QWidget *parent = nullptr);
    ~AssessmentReportPage();

    // 设置用户ID
    void setUserId(const QString& userId);
    
    // 显示即时报告（刚完成测评后）
    void showInstantReport(float score, const QString& riskLevel, const QString& suggestion);
    
    // 加载详细报告（历史数据）
    bool loadDetailedReport(int days = 7);

signals:
    void backRequested();

private slots:
    void onBackClicked();
    void onViewHistoryClicked();

private:
    Ui::AssessmentReportPage *ui;
    
    QString m_userId;
    float m_currentScore;
    QString m_riskLevel;
    QString m_suggestion;
    
    QVector<DimensionData> m_dimensions;
    QVector<TrendData> m_trends;
    QVector<IndicatorData> m_indicators;
    
    // UI辅助方法
    void setupUI();
    void displayInstantReport();
    void displayDetailedReport();
    void updateScoreDisplay();
    void updateRiskLevelDisplay();
    void updateSuggestionDisplay();
    void updateDimensionsDisplay();
    void updateTrendsDisplay();
    void updateIndicatorsDisplay();
    QString getRiskLevelText(const QString& level);
    QString getRiskLevelColor(const QString& level);
};

#endif // ASSESSMENTREPORTPAGE_H