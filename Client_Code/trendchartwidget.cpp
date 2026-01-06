#include "trendchartwidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <cmath>

TrendChartWidget::TrendChartWidget(QWidget *parent)
    : QWidget(parent)
    , m_timeRange(7)
    , m_hoveredIndex(-1)
{
    setMinimumHeight(200);
    setMouseTracking(true);  // 启用鼠标跟踪以显示tooltip
    
    // 设置背景色
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(255, 255, 255));
    setPalette(pal);
}

TrendChartWidget::~TrendChartWidget()
{
}

void TrendChartWidget::setTrendData(const QVector<TrendPoint>& data)
{
    m_data = data;
    m_hoveredIndex = -1;
    update();  // 触发重绘
    qDebug() << "[TrendChart] Data updated, count:" << m_data.size();
}

void TrendChartWidget::setTimeRange(int days)
{
    m_timeRange = days;
    update();
}

void TrendChartWidget::clearData()
{
    m_data.clear();
    m_hoveredIndex = -1;
    update();
}

void TrendChartWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QRect chartRect = getChartRect();
    
    if (m_data.isEmpty()) {
        // 显示空状态
        painter.setPen(QColor(150, 150, 150));
        painter.drawText(rect(), Qt::AlignCenter, "暂无数据");
        return;
    }
    
    // 绘制各个部分
    drawBackground(painter, chartRect);
    drawGrid(painter, chartRect);
    drawAxes(painter, chartRect);
    drawTrendLine(painter, chartRect);
    drawDataPoints(painter, chartRect);
    drawLabels(painter, chartRect);
    
    if (m_hoveredIndex >= 0) {
        drawTooltip(painter, chartRect);
    }
}

void TrendChartWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_data.isEmpty()) {
        return;
    }
    
    QRect chartRect = getChartRect();
    QPoint pos = event->pos();
    
    // 查找最近的数据点
    int nearestIndex = -1;
    float minDist = 20.0f;  // 最大检测距离
    
    for (int i = 0; i < m_data.size(); ++i) {
        QPointF screenPos = dataToScreen(i, chartRect);
        float dx = pos.x() - screenPos.x();
        float dy = pos.y() - screenPos.y();
        float dist = sqrt(dx * dx + dy * dy);
        
        if (dist < minDist) {
            minDist = dist;
            nearestIndex = i;
        }
    }
    
    if (nearestIndex != m_hoveredIndex) {
        m_hoveredIndex = nearestIndex;
        update();
    }
}

void TrendChartWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    if (m_hoveredIndex >= 0) {
        m_hoveredIndex = -1;
        update();
    }
}

QRect TrendChartWidget::getChartRect() const
{
    int margin = 40;
    int topMargin = 20;
    int bottomMargin = 40;
    
    return QRect(
        margin,
        topMargin,
        width() - 2 * margin,
        height() - topMargin - bottomMargin
    );
}

void TrendChartWidget::drawBackground(QPainter& painter, const QRect& chartRect)
{
    // 绘制图表背景
    painter.fillRect(chartRect, QColor(250, 250, 250));
    
    // 绘制边框
    painter.setPen(QPen(QColor(200, 200, 200), 1));
    painter.drawRect(chartRect);
}

void TrendChartWidget::drawGrid(QPainter& painter, const QRect& chartRect)
{
    painter.setPen(QPen(QColor(230, 230, 230), 1, Qt::DotLine));
    
    // 绘制水平网格线（5条）
    for (int i = 1; i < 5; ++i) {
        int y = chartRect.top() + chartRect.height() * i / 5;
        painter.drawLine(chartRect.left(), y, chartRect.right(), y);
    }
    
    // 绘制垂直网格线
    if (m_data.size() > 1) {
        for (int i = 1; i < m_data.size(); ++i) {
            QPointF pos = dataToScreen(i, chartRect);
            painter.drawLine(pos.x(), chartRect.top(), pos.x(), chartRect.bottom());
        }
    }
}

void TrendChartWidget::drawAxes(QPainter& painter, const QRect& chartRect)
{
    painter.setPen(QPen(QColor(100, 100, 100), 2));
    
    // X轴
    painter.drawLine(chartRect.bottomLeft(), chartRect.bottomRight());
    
    // Y轴
    painter.drawLine(chartRect.topLeft(), chartRect.bottomLeft());
}

void TrendChartWidget::drawTrendLine(QPainter& painter, const QRect& chartRect)
{
    if (m_data.size() < 2) {
        return;
    }
    
    // 绘制趋势线
    painter.setPen(QPen(QColor(52, 152, 219), 2));
    
    QPainterPath path;
    QPointF firstPoint = dataToScreen(0, chartRect);
    path.moveTo(firstPoint);
    
    for (int i = 1; i < m_data.size(); ++i) {
        QPointF point = dataToScreen(i, chartRect);
        path.lineTo(point);
    }
    
    painter.drawPath(path);
    
    // 绘制渐变填充
    QLinearGradient gradient(chartRect.topLeft(), chartRect.bottomLeft());
    gradient.setColorAt(0, QColor(52, 152, 219, 50));
    gradient.setColorAt(1, QColor(52, 152, 219, 10));
    
    QPainterPath fillPath = path;
    fillPath.lineTo(dataToScreen(m_data.size() - 1, chartRect).x(), chartRect.bottom());
    fillPath.lineTo(firstPoint.x(), chartRect.bottom());
    fillPath.closeSubpath();
    
    painter.fillPath(fillPath, gradient);
}

void TrendChartWidget::drawDataPoints(QPainter& painter, const QRect& chartRect)
{
    for (int i = 0; i < m_data.size(); ++i) {
        QPointF pos = dataToScreen(i, chartRect);
        QColor color = getRiskColor(m_data[i].riskLevel);
        
        // 绘制外圈
        painter.setPen(QPen(color, 2));
        painter.setBrush(Qt::white);
        
        int radius = (i == m_hoveredIndex) ? 6 : 4;
        painter.drawEllipse(pos, radius, radius);
        
        // 绘制内圈
        painter.setBrush(color);
        painter.drawEllipse(pos, radius - 2, radius - 2);
    }
}

void TrendChartWidget::drawLabels(QPainter& painter, const QRect& chartRect)
{
    painter.setPen(QColor(100, 100, 100));
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);
    
    // 绘制Y轴标签（分数）
    float maxScore = getMaxScore();
    float minScore = getMinScore();
    float scoreRange = maxScore - minScore;
    
    if (scoreRange < 0.1f) {
        scoreRange = 10.0f;  // 默认范围
        minScore = maxScore - 5.0f;
        maxScore = maxScore + 5.0f;
    }
    
    for (int i = 0; i <= 5; ++i) {
        float score = minScore + scoreRange * (5 - i) / 5.0f;
        int y = chartRect.top() + chartRect.height() * i / 5;
        
        QString label = QString::number(score, 'f', 1);
        QRect textRect(0, y - 10, chartRect.left() - 5, 20);
        painter.drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, label);
    }
    
    // 绘制X轴标签（日期）
    int labelStep = (m_data.size() > 7) ? 2 : 1;  // 数据多时隔一个显示
    
    for (int i = 0; i < m_data.size(); i += labelStep) {
        QPointF pos = dataToScreen(i, chartRect);
        QRect textRect(pos.x() - 30, chartRect.bottom() + 5, 60, 20);
        painter.drawText(textRect, Qt::AlignCenter, m_data[i].date);
    }
}

void TrendChartWidget::drawTooltip(QPainter& painter, const QRect& chartRect)
{
    if (m_hoveredIndex < 0 || m_hoveredIndex >= m_data.size()) {
        return;
    }
    
    const TrendPoint& point = m_data[m_hoveredIndex];
    QPointF pos = dataToScreen(m_hoveredIndex, chartRect);
    
    // 构建tooltip文本
    QString text = QString("%1\n评分: %2\n%3")
        .arg(point.date)
        .arg(point.score, 0, 'f', 1)
        .arg(point.riskLevel);
    
    // 计算tooltip大小
    QFont font = painter.font();
    font.setPointSize(9);
    painter.setFont(font);
    
    QFontMetrics fm(font);
    QStringList lines = text.split('\n');
    int maxWidth = 0;
    for (const QString& line : lines) {
        maxWidth = qMax(maxWidth, fm.width(line));
    }
    
    int tooltipWidth = maxWidth + 20;
    int tooltipHeight = lines.size() * fm.height() + 10;
    
    // 计算tooltip位置（避免超出边界）
    int tooltipX = pos.x() + 10;
    int tooltipY = pos.y() - tooltipHeight - 10;
    
    if (tooltipX + tooltipWidth > width()) {
        tooltipX = pos.x() - tooltipWidth - 10;
    }
    if (tooltipY < 0) {
        tooltipY = pos.y() + 10;
    }
    
    QRect tooltipRect(tooltipX, tooltipY, tooltipWidth, tooltipHeight);
    
    // 绘制tooltip背景
    painter.setPen(QPen(QColor(100, 100, 100), 1));
    painter.setBrush(QColor(255, 255, 255, 240));
    painter.drawRoundedRect(tooltipRect, 5, 5);
    
    // 绘制tooltip文本
    painter.setPen(QColor(50, 50, 50));
    painter.drawText(tooltipRect, Qt::AlignCenter, text);
}

QPointF TrendChartWidget::dataToScreen(int index, const QRect& chartRect) const
{
    if (m_data.isEmpty() || index < 0 || index >= m_data.size()) {
        return QPointF();
    }
    
    float maxScore = getMaxScore();
    float minScore = getMinScore();
    float scoreRange = maxScore - minScore;
    
    if (scoreRange < 0.1f) {
        scoreRange = 10.0f;
        minScore = maxScore - 5.0f;
    }
    
    // X坐标
    float x = chartRect.left();
    if (m_data.size() > 1) {
        x += chartRect.width() * index / (m_data.size() - 1.0f);
    } else {
        x += chartRect.width() / 2.0f;
    }
    
    // Y坐标（反转，因为屏幕坐标Y向下）
    float normalizedScore = (m_data[index].score - minScore) / scoreRange;
    float y = chartRect.bottom() - chartRect.height() * normalizedScore;
    
    return QPointF(x, y);
}

float TrendChartWidget::getMaxScore() const
{
    if (m_data.isEmpty()) {
        return 27.0f;  // 默认最大值
    }
    
    float maxScore = m_data[0].score;
    for (const TrendPoint& point : m_data) {
        maxScore = qMax(maxScore, point.score);
    }
    
    // 向上取整到5的倍数
    return ceil(maxScore / 5.0f) * 5.0f;
}

float TrendChartWidget::getMinScore() const
{
    if (m_data.isEmpty()) {
        return 0.0f;
    }
    
    float minScore = m_data[0].score;
    for (const TrendPoint& point : m_data) {
        minScore = qMin(minScore, point.score);
    }
    
    // 向下取整到5的倍数
    return floor(minScore / 5.0f) * 5.0f;
}

QColor TrendChartWidget::getRiskColor(const QString& riskLevel) const
{
    if (riskLevel.contains("低") || riskLevel.contains("Low")) {
        return QColor(46, 204, 113);  // 绿色
    } else if (riskLevel.contains("中") || riskLevel.contains("Medium")) {
        return QColor(241, 196, 15);  // 黄色
    } else if (riskLevel.contains("高") || riskLevel.contains("High")) {
        return QColor(231, 76, 60);   // 红色
    }
    return QColor(149, 165, 166);  // 灰色（未知）
}