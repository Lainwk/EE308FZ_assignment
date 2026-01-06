#include "homepage.h"
#include "ui_homepage.h"
#include "dailyassessmentpage.h"
#include "assessmentreportpage.h"
#include "assessmentcache.h"
#include "frontclient.h"
#include "trendchartwidget.h"
#include <QVBoxLayout>
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
#include <cstring>
#include <algorithm>

HomePage::HomePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomePage),
    m_assessmentPage(nullptr),
    m_reportPage(nullptr),
    m_historyListWidget(nullptr),
    m_trendChart(nullptr),
    m_checkTimer(nullptr),
    m_currentRiskLevel("general")  // ✅ 初始化风险等级
{
    ui->setupUi(this);
    
    // ✅ Android平台特殊处理
#ifdef Q_OS_ANDROID
    // 设置Android平台的字体和样式
    QFont androidFont;
    androidFont.setFamily("Roboto");
    androidFont.setPixelSize(14);
    this->setFont(androidFont);
    
    // 确保所有标签都有明确的字体大小和颜色
    ui->labelUserName->setStyleSheet("font-size: 16px; font-weight: bold; color: #000000;");
    ui->label_4->setStyleSheet("font-size: 14px; color: #666666;");
    ui->lbScore->setStyleSheet("font-size: 18px; font-weight: bold; color: #2563EB; background-color: #EFF6FF; border-radius: 12px;");
    ui->label->setStyleSheet("font-size: 16px; font-weight: bold; color: #000000;");
    ui->labelQuickTestDesc->setStyleSheet("font-size: 14px; color: #FFFFFF;");
    ui->label_3->setStyleSheet("font-size: 16px; font-weight: bold; color: #000000;");
    ui->label_2->setStyleSheet("font-size: 14px; color: #666666;");
    ui->label_5->setStyleSheet("font-size: 12px; color: #666666;");
    ui->label_6->setStyleSheet("font-size: 12px; color: #666666;");
    ui->label_7->setStyleSheet("font-size: 16px; font-weight: bold; color: #000000;");
    ui->labelSuggestion->setStyleSheet("font-size: 14px; color: #333333; line-height: 1.5;");
    
    // 设置进度条最小高度
    ui->progressMood->setMinimumHeight(12);
    
    // Android平台：彻底移除测评区域的白色框
    // 强制设置groupQuickTest为透明，只显示渐变背景
    ui->groupQuickTest->setStyleSheet(
        "QGroupBox#groupQuickTest { "
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #4388F4, stop:1 #2ecc71); "
        "border: none; "
        "border-radius: 12px; "
        "}"
    );
    
    // 确保标签背景透明
    ui->label->setStyleSheet("QLabel { color: white; font-size: 20px; font-weight: bold; background: transparent; }");
    ui->labelQuickTestDesc->setStyleSheet("QLabel { color: white; font-size: 14px; background: transparent; }");
    
    qDebug() << "[HomePage] Android-specific styles applied with gradient background";
#endif
    
    // ✅ 初始化tips库
    initializeTipsLibrary();
    
    setupPages();
    setupHistoryList();
    setupTrendChart();
    
    // 连接信号槽
    connect(ui->btnStartTest, &QPushButton::clicked, this, &HomePage::onStartTestClicked);
    
    // 创建定时器,每5分钟检查一次今日测评状态
    m_checkTimer = new QTimer(this);
    connect(m_checkTimer, SIGNAL(timeout()), this, SLOT(checkTodayAssessmentStatus()));
    m_checkTimer->start(300000); // 5分钟 = 300000毫秒
    
    // ✅ 显示初始tip
    updateDailyTip();
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
    
    // 加载最近的测评历史
    loadRecentHistory(5);
    
    // 加载趋势图数据
    loadTrendData(7);
    
    // 检查今日测评状态
    checkTodayAssessmentStatusInternal(false);
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
    m_historyListWidget->setMaximumHeight(150);
    // Android平台设置黑色文字
#ifdef Q_OS_ANDROID
    m_historyListWidget->setStyleSheet(
        "QListWidget { border: 1px solid #ccc; background: white; border-radius:8px; }"
        "QListWidget::item { padding: 8px; color: #000000; font-size: 14px; }"
    );
#else
    m_historyListWidget->setStyleSheet(
        "QListWidget { border: 1px solid #ccc; background: white; border-radius:8px; }"
        "QListWidget::item { padding: 8px; }"
    );
#endif
    
    // 添加到趋势组的布局中
    QVBoxLayout* trendLayout = qobject_cast<QVBoxLayout*>(ui->groupTrend->layout());
    if (trendLayout) {
        // 移除占位符
        QLabel* placeholder = ui->labelTrendPlaceholder;
        if (placeholder) {
            trendLayout->removeWidget(placeholder);
            placeholder->hide();
        }
        
        // 添加标题 - Android平台使用黑色文字
        QLabel* historyTitle = new QLabel("最近测评记录", this);
#ifdef Q_OS_ANDROID
        historyTitle->setStyleSheet("font-weight: bold; color: #000000; font-size: 16px;");
#else
        historyTitle->setStyleSheet("font-weight: bold; color: #333;");
#endif
        trendLayout->addWidget(historyTitle);
        
        // 添加列表
        trendLayout->addWidget(m_historyListWidget);
        
        // 添加"查看更多"按钮
        QHBoxLayout *hLayout = new QHBoxLayout();
        QPushButton* btnViewMore = new QPushButton("查看全部历史", this);
        btnViewMore->setFixedSize(150,30);
        connect(btnViewMore, &QPushButton::clicked, this, &HomePage::onViewMoreHistoryClicked);
        hLayout->addStretch();
        hLayout->addWidget(btnViewMore);
        trendLayout->addLayout(hLayout);
        btnViewMore->setStyleSheet("background: #07C160;color: rgb(255, 255, 255);border-radius: 8px");
    }
    
    // 连接列表项点击事件
    connect(m_historyListWidget, &QListWidget::itemClicked, 
            this, &HomePage::onHistoryItemClicked);
}

void HomePage::showHomePage()
{
    // 隐藏其他页面,显示首页内容
    if (m_assessmentPage) m_assessmentPage->hide();
    if (m_reportPage) m_reportPage->hide();
    this->show();
    qDebug() << "[HomePage] Showing home page";
    
    // 只在缓存失效时才刷新历史记录
    if (!AssessmentCache::instance().hasHistoryCache(m_userId)) {
        qDebug() << "[HomePage] No cache found, loading history";
        loadRecentHistory(5);
    } else {
        qDebug() << "[HomePage] Using existing cache";
    }
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
    qDebug() << "[HomePage] Current user ID:" << m_userId;
    qDebug() << "[HomePage] Current user name:" << m_userName;
    
    if (m_userId.isEmpty()) {
        qDebug() << "[HomePage] User ID is empty, showing login warning";
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
    
    // ✅ 更新当前风险等级
    m_currentRiskLevel = riskLevel;
    
    // 显示即时报告
    m_reportPage->setUserId(m_userId);
    m_reportPage->showInstantReport(score, riskLevel, suggestion);
    showReportPage();
    
    // 更新首页的情绪指数
    // 每日测评: 2题,每题0-3分,总分0-6
    // 分数越低越好: 0分=100%, 6分=0%
    int moodValue = calculateMoodFromRecentHistory(riskLevel);
    ui->lbScore->setText(QString::number(moodValue));
        auto makeGradient = [](const QString& start, const QString& end) {
            return QString(
                "QProgressBar { border: 0; }"
                "QProgressBar::chunk {"
                "  border-radius: 5px;"
                "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                "    stop:0 %1, stop:1 %2);"
                "}"
            ).arg(start, end);
        };

        QString progressStyle;
        if (moodValue >= 85) {
            progressStyle = makeGradient("#4ED96E", "#07C160"); // 绿系渐变
        } else if (moodValue >= 60) {
            progressStyle = makeGradient("#FFD54F", "#FFC107"); // 黄系渐变
        } else if (moodValue >= 30) {
            progressStyle = makeGradient("#FFB74D", "#FF9800"); // 橙系渐变
        } else {
            progressStyle = makeGradient("#FF6F61", "#F44336"); // 红系渐变
        }
        ui->progressMood->setValue(moodValue);
        ui->progressMood->setStyleSheet(progressStyle);
    
    qDebug() << "[HomePage] Updated mood value (7-day avg):" << moodValue << "Risk:" << riskLevel;
    
    // ✅ 更新建议 - 优先显示测评建议,如果为空则显示每日tip
    if (!suggestion.isEmpty()) {
        ui->labelSuggestion->setText(suggestion);
    } else {
        updateDailyTip();
    }
    
    // 清除缓存并强制刷新
    AssessmentCache::instance().clearUser(m_userId);
    loadRecentHistory(5, true);
    updateTrendChart();  // 更新趋势图
    checkTodayAssessmentStatusInternal(true);
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
    
    // 这里可以显示该记录的详细信息
    QMessageBox::information(this, "历史记录", 
        QString("记录ID: %1\n%2").arg(recordId).arg(item->text()));
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
    qDebug() << "[HomePage] Loading from server...";
    if (AssessmentCache::instance().loadHistoryFromServer(m_userId, limit)) {
        // ✅ 修复: 直接使用缓存,不要递归调用!
        QVector<CachedAssessmentRecord> cachedRecords =
            AssessmentCache::instance().getHistoryCache(m_userId);
        
        qDebug() << "[HomePage] Server load successful, cached records:" << cachedRecords.size();
        
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
    
    // 如果缓存加载失败,使用原来的直接加载方式
    qDebug() << "[HomePage] Cache load failed, using direct load";
    ASSESSMENT_HISTORY_REQ req;
    memset(&req, 0, sizeof(req));
    strncpy(req.user_id, m_userId.toStdString().c_str(), sizeof(req.user_id) - 1);
    req.limit = limit;
    req.offset = 0;
    
    QByteArray reqData((const char*)&req, sizeof(req));
    
    // 发送请求并接收响应
    HEAD respHead;
    QByteArray respBody = FrontClient::instance().sendAndReceive(
        ASSESSMENT_HISTORY, reqData, &respHead);
    
    if (respBody.isEmpty()) {
        qDebug() << "[HomePage] Failed to get history response";
        return;
    }
    
    // 解析响应
    if (respBody.size() < (int)sizeof(ASSESSMENT_HISTORY_RESP)) {
        qDebug() << "[HomePage] Invalid response size";
        return;
    }
    
    ASSESSMENT_HISTORY_RESP* resp = (ASSESSMENT_HISTORY_RESP*)respBody.data();
    qDebug() << "[HomePage] History response - status:" << resp->status_code 
             << "count:" << resp->record_count;
    
    if (resp->status_code != 0) {
        qDebug() << "[HomePage] History query failed, status:" << resp->status_code;
        return;
    }
    
    // 解析记录列表
    QVector<QPair<QString, QString>> historyData;
    const char* ptr = respBody.data() + sizeof(ASSESSMENT_HISTORY_RESP);
    
    for (int i = 0; i < resp->record_count; i++) {
        if (ptr + sizeof(ASSESSMENT_RECORD_ITEM) > respBody.data() + respBody.size()) {
            qDebug() << "[HomePage] Buffer overflow when parsing record" << i;
            break;
        }
        
        ASSESSMENT_RECORD_ITEM* item = (ASSESSMENT_RECORD_ITEM*)ptr;
        
        QString timeStr = QString::fromUtf8(item->completed_time);
        QString infoStr = QString("评分: %1 | %2")
            .arg(item->total_score, 0, 'f', 1)
            .arg(QString::fromUtf8(item->risk_level));
        
        historyData.append(qMakePair(timeStr, infoStr));
        
        ptr += sizeof(ASSESSMENT_RECORD_ITEM);
    }
    
    // 更新UI
    updateHistoryList(historyData);
}

void HomePage::updateHistoryList(const QVector<QPair<QString, QString>>& historyData)
{
    if (!m_historyListWidget) return;
    
    m_historyListWidget->clear();
    
    if (historyData.isEmpty()) {
        QListWidgetItem* item = new QListWidgetItem("暂无测评记录");
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
#ifdef Q_OS_ANDROID
        item->setForeground(QColor("#666666"));  // Android上使用深灰色
#else
        item->setForeground(QColor("#999"));
#endif
        m_historyListWidget->addItem(item);
        return;
    }
    
    for (int i = 0; i < historyData.size(); i++) {
        const auto& record = historyData[i];
        QString displayText = QString("%1\n%2")
            .arg(record.first)  // 时间
            .arg(record.second); // 信息
        
        QListWidgetItem* item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, i + 1); // 存储记录ID
#ifdef Q_OS_ANDROID
        // Android平台确保文字为黑色
        item->setForeground(QColor("#000000"));
#endif
        m_historyListWidget->addItem(item);
    }
    
    qDebug() << "[HomePage] History list updated with" << historyData.size() << "records";
}

void HomePage::showFullHistoryDialog()
{
    if (m_userId.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先登录");
        return;
    }
    
    qDebug() << "[HomePage] Showing full history dialog";
    
    // 创建对话框，宽度与主页一致
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("测评历史记录");
    dialog->resize(this->width(), static_cast<int>(this->height() * 0.8));
    dialog->setMinimumWidth(this->width());
    dialog->setMaximumWidth(this->width());
    
    QVBoxLayout* layout = new QVBoxLayout(dialog);
    
    // 添加标题
    QLabel* title = new QLabel("全部测评历史", dialog);
    title->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    layout->addWidget(title);
    
    // 创建列表
    QListWidget* listWidget = new QListWidget(dialog);
    listWidget->setStyleSheet(
        "QListWidget { border: 1px solid #ccc; background: white; }"
        "QListWidget::item { padding: 12px; border-bottom: 1px solid #eee; }"
        "QListWidget::item:hover { background: #f0f0f0; }"
    );
    layout->addWidget(listWidget);
    
    // 加载全部历史记录
    ASSESSMENT_HISTORY_REQ req;
    memset(&req, 0, sizeof(req));
    strncpy(req.user_id, m_userId.toStdString().c_str(), sizeof(req.user_id) - 1);
    req.limit = 0;  // 0表示全部
    req.offset = 0;
    
    QByteArray reqData((const char*)&req, sizeof(req));
    HEAD respHead;
    QByteArray respBody = FrontClient::instance().sendAndReceive(
        ASSESSMENT_HISTORY, reqData, &respHead);
    
    if (!respBody.isEmpty() && respBody.size() >= (int)sizeof(ASSESSMENT_HISTORY_RESP)) {
        ASSESSMENT_HISTORY_RESP* resp = (ASSESSMENT_HISTORY_RESP*)respBody.data();
        
        if (resp->status_code == 0 && resp->record_count > 0) {
            const char* ptr = respBody.data() + sizeof(ASSESSMENT_HISTORY_RESP);
            
            for (int i = 0; i < resp->record_count; i++) {
                if (ptr + sizeof(ASSESSMENT_RECORD_ITEM) > respBody.data() + respBody.size()) {
                    break;
                }
                
                ASSESSMENT_RECORD_ITEM* item = (ASSESSMENT_RECORD_ITEM*)ptr;
                
                QString displayText = QString("时间: %1\n评分: %2 | 风险等级: %3\n建议: %4")
                    .arg(QString::fromUtf8(item->completed_time))
                    .arg(item->total_score, 0, 'f', 1)
                    .arg(QString::fromUtf8(item->risk_level))
                    .arg(QString::fromUtf8(item->suggestion));
                
                QListWidgetItem* listItem = new QListWidgetItem(displayText);
                listItem->setData(Qt::UserRole, item->record_id);
                listWidget->addItem(listItem);
                
                ptr += sizeof(ASSESSMENT_RECORD_ITEM);
            }
        } else {
            QListWidgetItem* item = new QListWidgetItem("暂无历史记录");
            item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
            listWidget->addItem(item);
        }
    } else {
        QListWidgetItem* item = new QListWidgetItem("加载失败");
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        listWidget->addItem(item);
    }
    
    // 添加关闭按钮
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, dialog);
    connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::accept);
    layout->addWidget(buttonBox);
    
    dialog->exec();
    delete dialog;
}

void HomePage::checkTodayAssessmentStatus()
{
    // 无参数版本,用于定时器调用
    checkTodayAssessmentStatusInternal(false);
}

void HomePage::checkTodayAssessmentStatusInternal(bool forceRefresh)
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
        return;
    }
    
    // 如果缓存加载失败,使用原来的直接加载方式
    
    // 构造请求
    CHECK_TODAY_ASSESSMENT_REQ req;
    memset(&req, 0, sizeof(req));
    strncpy(req.user_id, m_userId.toStdString().c_str(), sizeof(req.user_id) - 1);
    
    QByteArray reqData((const char*)&req, sizeof(req));
    
    // 发送请求并接收响应
    HEAD respHead;
    QByteArray respBody = FrontClient::instance().sendAndReceive(
        CHECK_TODAY_ASSESSMENT, reqData, &respHead);
    
    if (respBody.isEmpty() || respBody.size() < (int)sizeof(CHECK_TODAY_ASSESSMENT_RESP)) {
        qDebug() << "[HomePage] Failed to check today's assessment";
        return;
    }
    
    CHECK_TODAY_ASSESSMENT_RESP* resp = (CHECK_TODAY_ASSESSMENT_RESP*)respBody.data();
    
    qDebug() << "[HomePage] Today assessment check - status:" << resp->status_code
             << "completed:" << resp->completed;
    
    if (resp->status_code == 0) {
        bool completed = (resp->completed == 1);
        QString lastTime = QString::fromUtf8(resp->last_completed_time);
        float lastScore = resp->last_score;
        
        updateTodayAssessmentStatus(completed, lastTime, lastScore);
        
        // 如果未完成,显示提醒
        if (!completed) {
            showTodayReminderIfNeeded();
        }
    }
}

void HomePage::updateTodayAssessmentStatus(bool completed, const QString& lastTime, float lastScore)
{
    if (completed) {
        // 更新UI显示已完成状态
        ui->labelQuickTestDesc->setText(
            QString("今日测评已完成!\n上次评分: %1\n完成时间: %2")
            .arg(lastScore, 0, 'f', 1)
            .arg(lastTime)
        );
        ui->btnStartTest->setText("重新测评");
        ui->btnStartTest->setStyleSheet("background-color: #90EE90;");

        // 用最新记录的风险等级刷新情绪指数/数字
        QString riskForMood = "green";
        if (!AssessmentCache::instance().hasHistoryCache(m_userId)) {
            AssessmentCache::instance().loadHistoryFromServer(m_userId, 1);
        }
        QVector<CachedAssessmentRecord> recs = AssessmentCache::instance().getHistoryCache(m_userId);
        if (!recs.isEmpty() && !recs.first().riskLevel.isEmpty()) {
            riskForMood = recs.first().riskLevel;
        }

        int moodValue = calculateMoodFromRecentHistory(riskForMood);
        QString progressStyle;
        if (moodValue >= 85) {
            progressStyle = "QProgressBar::chunk { background-color: #07C160; }";
        } else if (moodValue >= 60) {
            progressStyle = "QProgressBar::chunk { background-color: #FFC107; }";
        } else if (moodValue >= 30) {
            progressStyle = "QProgressBar::chunk { background-color: #FF9800; }";
        } else {
            progressStyle = "QProgressBar::chunk { background-color: #F44336; }";
        }
        ui->progressMood->setValue(moodValue);
        ui->progressMood->setStyleSheet(progressStyle);
        ui->lbScore->setText(QString::number(moodValue));
    } else {
        // 显示未完成状态
        ui->labelQuickTestDesc->setText("只需几分钟,了解你的当下情绪。");
        ui->btnStartTest->setText("开始今日测评");
        ui->btnStartTest->setStyleSheet("");
    }
    
    qDebug() << "[HomePage] Today assessment status updated - completed:" << completed;
}

void HomePage::showTodayReminderIfNeeded()
{
    // Suppress daily reminder popup; rely on UI entry points instead
    qDebug() << "[HomePage] Daily reminder suppressed (no popup shown)";
    return;

    // 检查今天是否已经显示过提醒
    QString todayKey = QDate::currentDate().toString("yyyy-MM-dd");
    QString lastReminderKey = QString("last_reminder_%1").arg(m_userId);
    
    // 这里可以使用LocalStore来记录上次提醒时间
    // 简化实现:每次启动只提醒一次
    static bool hasReminded = false;
    if (hasReminded) {
        return;
    }
    
    hasReminded = true;
    
    // 显示温馨提醒
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("温馨提醒");
    msgBox.setText("您今天还没有完成心理测评哦!");
    msgBox.setInformativeText("定期测评有助于更好地了解自己的情绪状态。");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setButtonText(QMessageBox::Ok, "立即测评");
    msgBox.setButtonText(QMessageBox::Cancel, "稍后再说");
    msgBox.setDefaultButton(QMessageBox::Ok);
    
    int ret = msgBox.exec();
    if (ret == QMessageBox::Ok) {
        onStartTestClicked();
    }
    
    qDebug() << "[HomePage] Today reminder shown";
}

void HomePage::setupTrendChart()
{
    qDebug() << "[HomePage] Setting up trend chart...";
    
    // 创建趋势图控件
    m_trendChart = new TrendChartWidget(this);
    m_trendChart->setMinimumHeight(200);
    m_trendChart->setTimeRange(7);  // 默认显示7天
    
    // 添加到趋势组的布局中
    QVBoxLayout* trendLayout = qobject_cast<QVBoxLayout*>(ui->groupTrend->layout());
    if (trendLayout) {
        // 在历史列表之前插入趋势图
        trendLayout->insertWidget(1, m_trendChart);
        qDebug() << "[HomePage] Trend chart added to layout";
    } else {
        qDebug() << "[HomePage] WARNING: groupTrend layout not found!";
    }
}

void HomePage::loadTrendData(int days)
{
    if (m_userId.isEmpty()) {
        qDebug() << "[HomePage] Cannot load trend data: user ID is empty";
        return;
    }
    
    if (!m_trendChart) {
        qDebug() << "[HomePage] WARNING: Trend chart not initialized!";
        return;
    }
    
    qDebug() << "[HomePage] Loading trend data for" << days << "days...";
    
    // 从缓存或服务器加载历史数据
    if (!AssessmentCache::instance().hasHistoryCache(m_userId)) {
        // 加载更多历史数据用于趋势图
        AssessmentCache::instance().loadHistoryFromServer(m_userId, days);
    }
    
    QVector<CachedAssessmentRecord> records =
        AssessmentCache::instance().getHistoryCache(m_userId);
    
    // 转换为趋势图数据格式
    QVector<TrendPoint> trendData;
    int count = qMin(days, records.size());
    
    for (int i = 0; i < count; i++) {
        const CachedAssessmentRecord& record = records[i];
        
        // 解析日期 "YYYY-MM-DD HH:MM:SS" -> "MM-DD"
        QString dateStr = record.completedTime;
        QStringList parts = dateStr.split(' ');
        if (!parts.isEmpty()) {
            QStringList dateParts = parts[0].split('-');
            if (dateParts.size() >= 3) {
                dateStr = QString("%1-%2").arg(dateParts[1]).arg(dateParts[2]);
            }
        }
        
        TrendPoint point(dateStr, record.totalScore, record.riskLevel);
        trendData.append(point);
    }
    
    // 反转数据(最旧的在前)
    std::reverse(trendData.begin(), trendData.end());
    
    // 更新趋势图
    m_trendChart->setTrendData(trendData);
    
    qDebug() << "[HomePage] Trend data loaded:" << trendData.size() << "points";
}

void HomePage::updateTrendChart()
{
    if (!m_trendChart) {
        qDebug() << "[HomePage] WARNING: Trend chart not initialized!";
        return;
    }
    
    // 重新加载趋势数据
    loadTrendData(7);  // 默认7天
}

void HomePage::onTimeRangeChanged(int days)
{
    qDebug() << "[HomePage] Time range changed to" << days << "days";
    loadTrendData(days);
}

int HomePage::calculateMoodFromRecentHistory(const QString& currentRiskLevel)
{
    // 强制刷新最近记录，避免旧缓存拉低均值
    AssessmentCache::instance().clearHistoryCache(m_userId);
    AssessmentCache::instance().loadHistoryFromServer(m_userId, 1);  // 仅拉取最新一条

    auto riskToValue = [](const QString& risk) -> int {
        if (risk == "green") return 100;
        if (risk == "yellow") return 70;
        if (risk == "orange") return 40;
        if (risk == "red") return 15;
        return 40; // 默认偏中低
    };

    QVector<int> values;
    
    QVector<CachedAssessmentRecord> records =
        AssessmentCache::instance().getHistoryCache(m_userId);

    if (!records.isEmpty()) {
        for (int i = 0; i < records.size() && values.size() < 7; ++i) {
            values.append(riskToValue(records[i].riskLevel));
        }
    } else {
        // 如果服务器也没记录，则使用当前风险
        values.append(riskToValue(currentRiskLevel));
    }

    if (values.isEmpty()) return 50;
    int total = 0;
    for (int v : values) total += v;
    int avg = total / values.size();
    return qMax(0, qMin(100, avg));
}

// ✅ ============ 每日心理小tips功能实现 ============

void HomePage::initializeTipsLibrary()
{
    qDebug() << "[HomePage] Initializing tips library...";
    
    // 通用tips(未测评或无风险等级时使用)
    m_tipsLibrary["general"] = QStringList()
        << "💡 深呼吸可以帮助缓解焦虑,试试4-7-8呼吸法:吸气4秒,憋气7秒,呼气8秒。"
        << "💡 每天记录3件让你感恩的事,能有效提升幸福感。"
        << "💡 适度运动能释放内啡肽,改善心情,每天30分钟就够了!"
        << "💡 保持规律作息,有助于情绪稳定和身心健康。"
        << "💡 和朋友聊天是很好的情绪调节方式,不要独自承受压力。"
        << "💡 学会说'不',适当拒绝能减少不必要的压力。"
        << "💡 听听舒缓的音乐,让心灵得到片刻宁静。"
        << "💡 尝试冥想5分钟,专注于当下,放下杂念。"
        << "💡 写日记可以帮助整理思绪,释放情绪。"
        << "💡 亲近大自然,户外散步能有效缓解压力。";
    
    // 绿色(状态良好)
    m_tipsLibrary["green"] = QStringList()
        << "🌟 保持良好状态!继续坚持健康的生活习惯。"
        << "🌟 分享快乐能让快乐加倍,试着和朋友分享你的喜悦吧!"
        << "🌟 状态很好!可以尝试帮助身边需要关心的人。"
        << "🌟 继续保持积极心态,你做得很棒!"
        << "🌟 良好的情绪状态是最好的礼物,珍惜当下。"
        << "🌟 试着培养一个新的兴趣爱好,让生活更丰富。"
        << "🌟 保持感恩的心,记录生活中的美好瞬间。"
        << "🌟 你的积极能量会感染身边的人,继续发光发热!";
    
    // 黄色(轻度压力)
    m_tipsLibrary["yellow"] = QStringList()
        << "⚠️ 感到压力时,试试5分钟冥想放松,专注呼吸。"
        << "⚠️ 写下你的担忧,有助于理清思路,减轻心理负担。"
        << "⚠️ 适当休息很重要,不要让自己过度疲劳。"
        << "⚠️ 和信任的人聊聊天,倾诉能有效缓解压力。"
        << "⚠️ 尝试渐进式肌肉放松法,从头到脚依次放松。"
        << "⚠️ 给自己一些'我时间',做些让你快乐的事。"
        << "⚠️ 调整期待值,接受不完美也是一种智慧。"
        << "⚠️ 保证充足睡眠,睡眠不足会加重焦虑情绪。";
    
    // 橙色(中度压力)
    m_tipsLibrary["orange"] = QStringList()
        << "🔶 情绪低落时,听听舒缓的音乐,让心情平复。"
        << "🔶 和信任的人倾诉,不要独自承受,寻求支持很重要。"
        << "🔶 尝试正念练习,专注当下,不要过度担忧未来。"
        << "🔶 适度运动能有效改善情绪,即使是散步也有帮助。"
        << "🔶 给自己一些积极的心理暗示:'这只是暂时的,我能应对。'"
        << "🔶 避免过度使用社交媒体,减少不必要的信息刺激。"
        << "🔶 建立规律的作息,稳定的生活节奏有助于情绪稳定。"
        << "🔶 如果情况持续,考虑寻求专业心理咨询帮助。";
    
    // 红色(高度压力/风险)
    m_tipsLibrary["red"] = QStringList()
        << "🆘 请重视自己的情绪状态,强烈建议寻求专业心理咨询帮助。"
        << "🆘 危机时刻,请拨打心理援助热线:400-161-9995(24小时)"
        << "🆘 不要独自面对,立即联系家人、朋友或专业人士。"
        << "🆘 如有自伤或伤人想法,请立即就医或拨打急救电话120。"
        << "🆘 记住:寻求帮助是勇敢的表现,不是软弱。"
        << "🆘 专业心理咨询师能提供有效帮助,不要犹豫。"
        << "🆘 保持安全,远离可能伤害自己的物品和环境。"
        << "🆘 这种感觉会过去的,请给自己和专业人士一个机会。";
    
    qDebug() << "[HomePage] Tips library initialized with"
             << m_tipsLibrary.keys().size() << "categories";
}

QString HomePage::getDailyTip(const QString& category)
{
    // 检查分类是否存在
    if (!m_tipsLibrary.contains(category)) {
        qDebug() << "[HomePage] Category not found:" << category << ", using general";
        return getDailyTip("general");
    }
    
    const QStringList& tips = m_tipsLibrary[category];
    if (tips.isEmpty()) {
        return "保持积极心态,关注自己的情绪健康。";
    }
    
    // 根据日期选择tip(每天固定,但每个分类独立)
    int dayOfYear = QDate::currentDate().dayOfYear();
    int index = (dayOfYear + qHash(category)) % tips.size();
    
    qDebug() << "[HomePage] Selected tip from category:" << category
             << "index:" << index << "/" << tips.size();
    
    return tips[index];
}

void HomePage::updateDailyTip()
{
    QString tip = getDailyTip(m_currentRiskLevel);
    ui->labelSuggestion->setText(tip);
    
    qDebug() << "[HomePage] Daily tip updated for risk level:" << m_currentRiskLevel;
}
