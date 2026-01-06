#ifndef TRENDCHARTWIDGET_H
#define TRENDCHARTWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QString>

// 趋势数据点
struct TrendPoint {
    QString date;      // 日期 "MM-DD"
    float score;       // 评分
    QString riskLevel; // 风险等级
    
    TrendPoint() : score(0.0f) {}
    TrendPoint(const QString& d, float s, const QString& r) 
        : date(d), score(s), riskLevel(r) {}
};

class TrendChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrendChartWidget(QWidget *parent = nullptr);
    ~TrendChartWidget();
    
    // 设置趋势数据
    void setTrendData(const QVector<TrendPoint>& data);
    
    // 设置时间范围（7天或30天）
    void setTimeRange(int days);
    
    // 清空数据
    void clearData();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QVector<TrendPoint> m_data;
    int m_timeRange;  // 7 or 30
    int m_hoveredIndex; // 鼠标悬停的点索引，-1表示无
    
    // 绘制辅助方法
    void drawBackground(QPainter& painter, const QRect& chartRect);
    void drawAxes(QPainter& painter, const QRect& chartRect);
    void drawGrid(QPainter& painter, const QRect& chartRect);
    void drawTrendLine(QPainter& painter, const QRect& chartRect);
    void drawDataPoints(QPainter& painter, const QRect& chartRect);
    void drawLabels(QPainter& painter, const QRect& chartRect);
    void drawTooltip(QPainter& painter, const QRect& chartRect);
    
    // 计算辅助方法
    QRect getChartRect() const;
    QPointF dataToScreen(int index, const QRect& chartRect) const;
    float getMaxScore() const;
    float getMinScore() const;
    QColor getRiskColor(const QString& riskLevel) const;
};

#endif // TRENDCHARTWIDGET_H