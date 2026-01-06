
#include "homepage.h"
#include "ui_homepage.h"
#include "dailyassessmentpage.h"
#include "assessmentreportpage.h"
#include "trendchartwidget.h"
#include "assessmentcache.h"
#include "frontclient.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include <QDialog>
#include <QDialogButtonBox>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QTimer>
#include <QComboBox>
#include <QGroupBox>
#include <cstring>

HomePage::HomePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomePage),
    m_assessmentPage(nullptr),
    m_reportPage(nullptr),
    m_historyListWidget(nullptr),
    m_trendChart(nullptr),
    m_checkTimer(nullptr)
{
    ui->setupUi(this);
    setupPages();
    setupHistoryList();
    setupTrendChart();
    
    // 连接信号槽
    connect(ui->btnStartTest, &QPushButton::clicked, this, &HomePage::onStartTestClicked);
    connect(ui->btnHistory, &QPushButton::clicked, this, &HomePage::onHistoryClicked);
    
    // 创建定时器，每5分钟检查一次今日测评状态
    m_checkTimer = new QTimer(this);
    connect(m_checkTimer, &QTimer::timeout, this, &HomePage::checkTodayAssessmentStatus);
    m_checkTimer->start(300000); // 5分钟 = 300000毫秒
}

HomePage::~HomePage()
{
    delete ui;
}

void HomePage::setUserInfo(const QString& userId, const QString& userName)
{
    m_userId = userId;
    m_userName = userName;
    
    // 更新UI显示
    ui->labelUserName->setText(m_userName);
    
    qDebug() << "[HomePage] User info set - ID:" << m_userId << "Name:" << m_userName;
    
    // 加载最近的测评历史（优先使用缓存）
    loadRecentHistory(5, false);
    
    // 加载趋势数据
    loadTrendData(7);
    
    // 检查今日测评状态（优先使用缓存）
    checkTodayAssessmentStatus(false);
}

void HomePage::setupPages()
{
    // 创建测评页面
    m_assessmentPage = new DailyAssessmentPage(this);
    m_assessmentPage->hide();
    connect(m_assessmentPage, &DailyAssessmentPage::assessmentCompleted,
            this, &HomePage::onAssessmentCompleted);
    connect(m_assessmentPage, &DailyAssessmentPage::backRequested,
            this, &HomePage::onBackToHome);
    
    // 创建报告页面
    m_reportPage = new AssessmentReportPage(this);
    m_reportPage->hide();
    connect(m_reportPage, &AssessmentReportPage::backRequested,
            this, &HomePage::onBackToHome);
}

void HomePage::setupHistoryList()
{
    // 在情绪趋势区域添加历史记录列表
    m_historyListWidget = new QListWidget(this);
    m_historyListWidget->setMaximumHeight(120);
    m_historyListWidget->setStyleSheet(
        "QListWidget { border: 1px solid #ccc; background: white; font-size: 11px; }"
        "QListWidget::item { padding: 6px; border-bottom: 1px solid #eee; }"
        "QListWidget::item:hover { background: #f0f0f0; }"
    );
    
    // 添加到趋势组的布局中
    QVBoxLayout* trendLayout = qobject_cast<QVBoxLayout*>(ui->groupTrend->layout());
    if (trendLayout) {
        // 移除占位符
        QLabel* placeholder = ui->labelTrendPlaceholder;
        if (placeholder) {
            trendLayout->removeWidget(placeholder);
            placeholder->hide();
        }
        
        // 添加标题
        QLabel* historyTitle = new QLabel("最近测评记录", this);
        historyTitle->setStyleSheet("font-weight: bold; color: #333; font-size: 12px;");
        trendLayout->addWidget(historyTitle);
        
        // 添加列表
        trendLayout->addWidget(m_historyListWidget);
        
        // 添加"查看更多"按钮
        QPushButton* btnViewMore = new QPushButton("查看全部历史", this);
        btnViewMore->setMaximumWidth(150);
        btnViewMore->setStyleSheet("font-size: 11px;");
        connect(btnViewMore, &QPushButton::clicked, this, &HomePage::onViewMoreHistoryClicked);
        trendLayout->addWidget(btnViewMore);
    }
    
    // 连接列表项点击事件
    connect(m_historyListWidget, &QListWidget::itemClicked, 
            this, &HomePage::onHistoryItemClicked);
}

void HomePage::setupTrendChart()
{
    // 创建趋势图控件
    m_trendChart = new TrendChartWidget(this);
    m_trendChart->setMinimumHeight(180);
    
    // 创建一个新的GroupBox来放置趋势图
    QGroupBox* chartGroup = new QGroupBox("情绪趋势图", this);
    QVBoxLayout* chartLayout = new QVBoxLayout(chartGroup);
    
    // 添加时间范围选择
    QHBoxLayout* controlLayout = new QHBoxLayout();
    QLabel* rangeLabel = new QLabel("时间范围:", this);
    rangeLabel->setStyleSheet("font-size: 11px;");
    QComboBox* rangeCombo = new QComboBox(this);
    rangeCombo->addItem("最近7天", 7);
    rangeCombo->addItem("最近30天", 30);
    rangeCombo->setStyleSheet("font-size: 11px;");
    connect(rangeCombo, SIGNAL(currentIndexChanged(int)), 
            this, SLOT(onTimeRangeChanged(int)));
    
    controlLayout->addWidget(rangeLabel);
    controlLayout->addWidget(rangeCombo);
    controlLayout->addStretch();
    
    chartLayout->addLayout(controlLayout);
    chartLayout->addWidget(m_trendChart);
    
    // 将趋势图组添加到主布局
    QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(this->layout());
    if (mainLayout) {
        // 在"功能入口"之前插入
        int insertIndex = mainLayout->indexOf(ui->groupFunctions);
        if (insertIndex >= 0) {
            mainLayout->insertWidget(insertIndex, chartGroup);
        } else {
            mainLayout->addWidget(chartGroup);
        }
    }
}

void HomePage::showHomePage()
{
    // 隐藏其他页面，显示首页内容
    if (m_assessmentPage) m_assessmentPage->hide();
    if (m_reportPage) m_reportPage->hide();
    this->show();
    qDebug() << "[HomePage] Showing home page";
    
    // 刷新历史记录和趋势图
    loadRecentHistory(5, false);
    loadTrendData(7);
}

void HomePage::showAssessmentPage()
{
    // 设置测评页面的大小和位置与HomePage一致
    m_assessmentPage->setGeometry(this->geometry());
    m_assessmentPage->show();
    m_assessmentPage->raise();
    qDebug() << "[HomePage] Showing assessment page";
}

void HomePage::showReportPage()
{
    // 设置报告页面的大小和位置与HomePage一致
    m_reportPage->setGeometry(this->geometry());
    m_reportPage->show();
    m_reportPage->raise();
    qDebug() << "[HomePage] Showing report page";
}

void HomePage::onStartTestClicked()
{
    qDebug() << "[HomePage] Start test clicked";
    
    if (m_userId.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先登录");
        return;
    }
    
    // 设置用户ID并加载问卷
    m_assessmentPage->setUserId(m_userId);
    
    if (m_assessmentPage->loadQuestionnaire()) {
        showAssessmentPage();
    }
}

void HomePage::onHistoryClicked()
{
    qDebug() << "[HomePage] History clicked";
    
    if (m_userId.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先登录");
        return;
    }
    
    // 显示完整历史记录对话框
    showFullHistoryDialog();
}

void HomePage::onAssessmentCompleted(float score, const QString& riskLevel, const QString& suggestion)
{
    qDebug() << "[HomePage] Assessment completed - Score:" << score << "Risk:" << riskLevel;
    
    // 显示即时报告
    m_reportPage->setUserId(m_userId);
    m_reportPage->showInstantReport(score, riskLevel, suggestion);
    showReportPage();
    
    // 更新首页的情绪指数
    int moodValue = static_cast<int>(100.0f - (score / 27.0f * 100.0f));
    ui->progressMood->setValue(moodValue);
    
    // 更新建议
    ui->labelSuggestion->setText(suggestion);
    
    // 清除缓存并强制刷新
    AssessmentCache::instance().clearUser(m_userId);
    loadRecentHistory(5, true);
    loadTrendData(7);
    checkTodayAssessmentStatus(true);
}

void HomePage::onBackToHome()
{
    qDebug() << "[HomePage] Back to home";
    showHomePage();
}

void HomePage::onViewMoreHistoryClicked()
{
    qDebug() << "[HomePage] View more history clicked";
    showFullHistoryDialog();
}

void HomePage::onHistoryItemClicked(QListWidgetItem* item)
{
    if (!item) return;
    
    // 从item的data中获取记录ID
    int recordId = item->data(Qt::UserRole).toInt();
    qDebug() << "[HomePage] History item clicked, record ID:" << recordId;
    
    // 显示详细信息对话框
    showRecordDetailDialog(recordId);
}

void HomePage::onTimeRangeChanged(int index)
{
    QComboBox* combo = qobject_cast<QComboBox*>(sender());
    if (!combo) return;
    
    int days = combo->itemData(index).toInt();
    qDebug() << "[HomePage] Time range changed to" << days << "days";
    
    loadTrendData(days);
}

void HomePage::checkTodayAssessmentStatus(bool forceRefresh)
{
    if (m_userId.isEmpty()) {
        return;
    }
    
    qDebug() << "[HomePage] Checking today's assessment status, forceRefresh:" << forceRefresh;
    
    // 优先使用缓存
    if (!forceRefresh && AssessmentCache::instance().hasTodayStatusCache(m_userId)) {
        CachedTodayStatus status = AssessmentCache::instance().getTodayStatusCache(m_userId);
        qDebug() << "[HomePage] Using cached today status";
        updateTodayAssessmentStatus(status.completed, status.lastCompletedTime, status.lastScore);
        
        if (!status.completed) {
            showTodayReminderIfNeeded();
        }
        return;
    }
    
    // 从服务器加载
    if (AssessmentCache::instance().loadTodayStatusFromServer(m_userId)) {
        CachedTodayStatus status = AssessmentCache::instance().getTodayStatusCache(m_userId);
        updateTodayAssessmentStatus(status.completed, status.lastCompletedTime, status.lastScore);
        
        if (!status.completed) {
            showTodayReminderIfNeeded();
        }
    }
}

void HomePage::loadRecentHistory(int limit, bool forceRefresh)
{
    if (m_userId.isEmpty()) {
        qDebug() << "[HomePage] Cannot load history: user ID is empty";
        return;
    }
    
    qDebug() << "[HomePage] Loading recent history, limit:" << limit << "forceRefresh:" << forceRefresh;
    
    // 优先使用缓存
    if (!forceRefresh && AssessmentCache::instance().hasHistoryCache(m_userId)) {
        QVector<CachedAssessmentRecord> cachedRecords = 
            AssessmentCache::instance().getHistoryCache(m_userId);
        
        qDebug() << "[HomePage] Using cached history, count:" << cachedRecords.size();
        
        // 转换为显示格式
        QVector<QPair<QString, QString>> historyData;
        int count = qMin(limit, cachedRecords.size());
        for (int i = 0; i < count; i++) {
            const CachedAssessmentRecord& record = cachedRecords[i];
            QString timeStr = record.completedTime;
            QString infoStr = QString("评分: %1 | %2")
                .arg(record.totalScore, 0, 'f', 1)
                .arg(record.riskLevel);
            historyData.append(qMakePair(timeStr, infoStr));
        }
        
        updateHistoryList(historyData);
        return;
    }
    
    // 从服务器加载并缓存
    if (AssessmentCache::instance().loadHistoryFromServer(m_userId, limit)) {
        // 递归调用，这次会使用缓存
        loadRecentHistory(limit, false);
    }
}

void HomePage::updateHistoryList(const QVector<QPair<QString, QString>>& historyData)
{
    if (!m_historyListWidget) return;
    
    m_historyListWidget->clear();
    
    if (historyData.isEmpty()) {
        QListWidgetItem* item = new QListWidgetItem("暂无测评记录");
        item->setFlags