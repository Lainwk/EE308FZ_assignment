#include "assessmentreportpage.h"
#include "ui_assessmentreportpage.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QDebug>

AssessmentReportPage::AssessmentReportPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AssessmentReportPage),
    m_currentScore(0.0f)
{
    ui->setupUi(this);
    setupUI();
}

AssessmentReportPage::~AssessmentReportPage()
{
    delete ui;
}

void AssessmentReportPage::setupUI()
{
    connect(ui->backButton, &QPushButton::clicked, this, &AssessmentReportPage::onBackClicked);
    connect(ui->viewHistoryButton, &QPushButton::clicked, this, &AssessmentReportPage::onViewHistoryClicked);
}

void AssessmentReportPage::setUserId(const QString& userId)
{
    m_userId = userId;
}

void AssessmentReportPage::showInstantReport(float score, const QString& riskLevel, const QString& suggestion)
{
    m_currentScore = score;
    m_riskLevel = riskLevel;
    m_suggestion = suggestion;
    
    qDebug() << "[AssessmentReportPage] Showing instant report - Score:" << score 
             << "Risk:" << riskLevel;
    
    displayInstantReport();
}

bool AssessmentReportPage::loadDetailedReport(int days)
{
    qDebug() << "[AssessmentReportPage] Loading detailed report for" << days << "days";
    
    // 构造请求
    FETCH_DETAILED_REPORT_REQ req;
    memset(&req, 0, sizeof(req));
    strncpy(req.user_id, m_userId.toUtf8().constData(), sizeof(req.user_id) - 1);
    req.days = days;
    
    QByteArray reqBody(reinterpret_cast<const char*>(&req), sizeof(req));
    
    // 发送请求
    HEAD respHead;
    QByteArray respBody = FrontClient::instance().sendAndReceive(FETCH_DETAILED_REPORT, reqBody, &respHead);
    
    if (respBody.isEmpty()) {
        QMessageBox::warning(this, "错误", "无法连接到服务器");
        return false;
    }
    
    if (respBody.size() < static_cast<int>(sizeof(FETCH_DETAILED_REPORT_RESP))) {
        QMessageBox::warning(this, "错误", "服务器响应数据不完整");
        return false;
    }
    
    FETCH_DETAILED_REPORT_RESP* resp = reinterpret_cast<FETCH_DETAILED_REPORT_RESP*>(respBody.data());
    
    if (resp->status_code != 0) {
        if (resp->status_code == 1) {
            QMessageBox::information(this, "提示", "暂无历史数据");
        } else {
            QMessageBox::warning(this, "错误", "加载报告失败");
        }
        return false;
    }
    
    // 解析数据
    int offset = sizeof(FETCH_DETAILED_REPORT_RESP);
    
    // 解析维度得分
    m_dimensions.clear();
    for (int i = 0; i < resp->dimension_count; i++) {
        if (offset + static_cast<int>(sizeof(DIMENSION_SCORE)) > respBody.size()) break;
        
        DIMENSION_SCORE* dim = reinterpret_cast<DIMENSION_SCORE*>(respBody.data() + offset);
        DimensionData data;
        data.name = QString::fromUtf8(dim->dimension_name);
        data.score = dim->score;
        m_dimensions.append(data);
        offset += sizeof(DIMENSION_SCORE);
    }
    
    // 解析趋势数据
    m_trends.clear();
    for (int i = 0; i < resp->trend_count; i++) {
        if (offset + static_cast<int>(sizeof(TREND_POINT)) > respBody.size()) break;
        
        TREND_POINT* trend = reinterpret_cast<TREND_POINT*>(respBody.data() + offset);
        TrendData data;
        data.date = QString::fromUtf8(trend->date);
        data.score = trend->score;
        m_trends.append(data);
        offset += sizeof(TREND_POINT);
    }
    
    // 解析指标数据
    m_indicators.clear();
    for (int i = 0; i < resp->indicator_count; i++) {
        if (offset + static_cast<int>(sizeof(INDICATOR_ITEM)) > respBody.size()) break;
        
        INDICATOR_ITEM* ind = reinterpret_cast<INDICATOR_ITEM*>(respBody.data() + offset);
        IndicatorData data;
        data.name = QString::fromUtf8(ind->indicator_name);
        data.currentValue = ind->current_value;
        data.avgValue = ind->avg_value;
        m_indicators.append(data);
        offset += sizeof(INDICATOR_ITEM);
    }
    
    qDebug() << "[AssessmentReportPage] Loaded - Dimensions:" << m_dimensions.size()
             << "Trends:" << m_trends.size() << "Indicators:" << m_indicators.size();
    
    displayDetailedReport();
    return true;
}

void AssessmentReportPage::displayInstantReport()
{
    ui->titleLabel->setText("测评完成");
    
    updateScoreDisplay();
    updateRiskLevelDisplay();
    updateSuggestionDisplay();
    
    // 隐藏详细报告部分
    ui->dimensionsWidget->setVisible(false);
    ui->trendsWidget->setVisible(false);
    ui->indicatorsWidget->setVisible(false);
    ui->viewHistoryButton->setVisible(true);
}

void AssessmentReportPage::displayDetailedReport()
{
    ui->titleLabel->setText("详细报告");
    
    updateDimensionsDisplay();
    updateTrendsDisplay();
    updateIndicatorsDisplay();
    
    // 显示详细报告部分
    ui->dimensionsWidget->setVisible(true);
    ui->trendsWidget->setVisible(true);
    ui->indicatorsWidget->setVisible(true);
    ui->viewHistoryButton->setVisible(false);
}

void AssessmentReportPage::updateScoreDisplay()
{
    ui->scoreLabel->setText(QString::number(m_currentScore, 'f', 1));
    
    // 更新进度条
    int percentage = static_cast<int>((m_currentScore / 27.0f) * 100);  // PHQ-9最高27分
    ui->scoreProgressBar->setValue(percentage);
    
    // 根据分数设置颜色
    QString color = getRiskLevelColor(m_riskLevel);
    ui->scoreProgressBar->setStyleSheet(QString(
        "QProgressBar { border: 2px solid #E0E0E0; border-radius: 5px; text-align: center; }"
        "QProgressBar::chunk { background-color: %1; }"
    ).arg(color));
}

void AssessmentReportPage::updateRiskLevelDisplay()
{
    QString levelText = getRiskLevelText(m_riskLevel);
    QString color = getRiskLevelColor(m_riskLevel);
    
    ui->riskLevelLabel->setText(levelText);
    ui->riskLevelLabel->setStyleSheet(QString("QLabel { color: %1; font-size: 20px; font-weight: bold; }").arg(color));
}

void AssessmentReportPage::updateSuggestionDisplay()
{
    ui->suggestionText->setText(m_suggestion);
}

void AssessmentReportPage::updateDimensionsDisplay()
{
    // 清空现有内容
    QLayout* layout = ui->dimensionsContent->layout();
    if (layout) {
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
    } else {
        layout = new QVBoxLayout(ui->dimensionsContent);
        ui->dimensionsContent->setLayout(layout);
    }
    
    // 添加维度卡片
    for (const DimensionData& dim : m_dimensions) {
        QWidget* card = new QWidget();
        card->setStyleSheet("QWidget { background-color: white; border-radius: 8px; padding: 12px; }");
        
        QHBoxLayout* cardLayout = new QHBoxLayout(card);
        
        QLabel* nameLabel = new QLabel(dim.name);
        nameLabel->setStyleSheet("QLabel { font-size: 14px; color: #333333; }");
        
        QLabel* scoreLabel = new QLabel(QString::number(dim.score, 'f', 1));
        scoreLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; color: #07C160; }");
        
        cardLayout->addWidget(nameLabel);
        cardLayout->addStretch();
        cardLayout->addWidget(scoreLabel);
        
        layout->addWidget(card);
    }
}

void AssessmentReportPage::updateTrendsDisplay()
{
    // 简化版：显示文本列表
    QLayout* layout = ui->trendsContent->layout();
    if (layout) {
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
    } else {
        layout = new QVBoxLayout(ui->trendsContent);
        ui->trendsContent->setLayout(layout);
    }
    
    for (const TrendData& trend : m_trends) {
        QWidget* card = new QWidget();
        card->setStyleSheet("QWidget { background-color: white; border-radius: 8px; padding: 12px; }");
        
        QHBoxLayout* cardLayout = new QHBoxLayout(card);
        
        QLabel* dateLabel = new QLabel(trend.date);
        dateLabel->setStyleSheet("QLabel { font-size: 14px; color: #666666; }");
        
        QLabel* scoreLabel = new QLabel(QString::number(trend.score, 'f', 1));
        scoreLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; color: #4A90E2; }");
        
        cardLayout->addWidget(dateLabel);
        cardLayout->addStretch();
        cardLayout->addWidget(scoreLabel);
        
        layout->addWidget(card);
    }
}

void AssessmentReportPage::updateIndicatorsDisplay()
{
    QLayout* layout = ui->indicatorsContent->layout();
    if (layout) {
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
    } else {
        layout = new QVBoxLayout(ui->indicatorsContent);
        ui->indicatorsContent->setLayout(layout);
    }
    
    for (const IndicatorData& ind : m_indicators) {
        QWidget* card = new QWidget();
        card->setStyleSheet("QWidget { background-color: white; border-radius: 8px; padding: 12px; }");
        
        QVBoxLayout* cardLayout = new QVBoxLayout(card);
        
        QLabel* nameLabel = new QLabel(ind.name);
        nameLabel->setStyleSheet("QLabel { font-size: 14px; font-weight: bold; color: #333333; }");
        
        QHBoxLayout* valuesLayout = new QHBoxLayout();
        
        QLabel* currentLabel = new QLabel(QString("当前: %1").arg(ind.currentValue, 0, 'f', 1));
        currentLabel->setStyleSheet("QLabel { font-size: 13px; color: #666666; }");
        
        QLabel* avgLabel = new QLabel(QString("平均: %1").arg(ind.avgValue, 0, 'f', 1));
        avgLabel->setStyleSheet("QLabel { font-size: 13px; color: #999999; }");
        
        valuesLayout->addWidget(currentLabel);
        valuesLayout->addStretch();
        valuesLayout->addWidget(avgLabel);
        
        cardLayout->addWidget(nameLabel);
        cardLayout->addLayout(valuesLayout);
        
        layout->addWidget(card);
    }
}

QString AssessmentReportPage::getRiskLevelText(const QString& level)
{
    if (level == "green") return "状态良好";
    if (level == "yellow") return "轻度关注";
    if (level == "orange") return "中度警示";
    if (level == "red") return "需要帮助";
    return "未知";
}

QString AssessmentReportPage::getRiskLevelColor(const QString& level)
{
    if (level == "green") return "#07C160";
    if (level == "yellow") return "#FF9500";
    if (level == "orange") return "#FF6B00";
    if (level == "red") return "#FF3B30";
    return "#999999";
}

void AssessmentReportPage::onBackClicked()
{
    emit backRequested();
}

void AssessmentReportPage::onViewHistoryClicked()
{
    loadDetailedReport(7);
}