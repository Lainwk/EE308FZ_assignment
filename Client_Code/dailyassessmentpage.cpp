#include "dailyassessmentpage.h"
#include "ui_dailyassessmentpage.h"
#include <QMessageBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QFrame>
#include <QDebug>
#include <QSettings>
#include <QDate>
#include <QTimer>
#include <QPointer>

DailyAssessmentPage::DailyAssessmentPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DailyAssessmentPage),
    m_currentQuestionIndex(0),
    m_currentButtonGroup(nullptr),
    m_hasCompletedToday(false)
{
    ui->setupUi(this);
    setupUI();
}

DailyAssessmentPage::~DailyAssessmentPage()
{
    clearCurrentQuestion();
    delete ui;
}

void DailyAssessmentPage::setupUI()
{
    // 连接信号槽
    connect(ui->submitButton, &QPushButton::clicked, this, &DailyAssessmentPage::onSubmitClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &DailyAssessmentPage::onBackClicked);
    
    // 初始隐藏提交按钮
    ui->submitButton->setVisible(false);
    ui->submitButton->setText("开始测评");
}

void DailyAssessmentPage::setUserId(const QString& userId)
{
    m_userId = userId;
}

bool DailyAssessmentPage::loadQuestionnaire()
{
    qDebug() << "[DailyAssessmentPage] Loading questionnaire for user:" << m_userId;
    
    // 仅依赖服务端状态，不使用本地标记拦截
    
    // 构造请求
    PULL_QUESTIONNAIRE_REQ req;
    memset(&req, 0, sizeof(req));
    strncpy(req.user_id, m_userId.toUtf8().constData(), sizeof(req.user_id) - 1);
    // 使用当日日期，便于后端判定“已完成”状态
    QDate today = QDate::currentDate();
    QByteArray dateStr = today.toString("yyyy-MM-dd").toUtf8();
    strncpy(req.date, dateStr.constData(), sizeof(req.date) - 1);
    
    QByteArray reqBody(reinterpret_cast<const char*>(&req), sizeof(req));
    
    // 发送请求并接收响应
    HEAD respHead;
    QByteArray respBody = FrontClient::instance().sendAndReceive(PULL_QUESTIONNAIRE, reqBody, &respHead);
    
    if (respBody.isEmpty()) {
        showMessage("错误", "无法连接到服务器");
        return false;
    }
    
    // 解析响应头
    if (respBody.size() < static_cast<int>(sizeof(PULL_QUESTIONNAIRE_RESP))) {
        showMessage("错误", "服务器响应数据不完整");
        return false;
    }
    
    PULL_QUESTIONNAIRE_RESP* resp = reinterpret_cast<PULL_QUESTIONNAIRE_RESP*>(respBody.data());
    
    qDebug() << "[DailyAssessmentPage] Response - status:" << resp->status_code
             << "questionnaire_id:" << resp->questionnaire_id
             << "code:" << QString::fromUtf8(resp->questionnaire_code)
             << "title:" << QString::fromUtf8(resp->title)
             << "question_count:" << resp->question_count
             << "total_data_len:" << resp->total_data_len;
    
    if (resp->status_code != 0) {
        if (resp->status_code == 1) {
            m_hasCompletedToday = true;
            showMessage("提示", "您今天已经完成了心理测评！");
        } else {
            showMessage("错误", "加载问卷失败");
        }
        return false;
    }
    
    // 保存问卷信息
    m_questionnaireId = resp->questionnaire_id;
    m_questionnaireType = QString::fromUtf8(resp->questionnaire_code);
    
    // 解析问题列表
    m_questions.clear();
    int offset = sizeof(PULL_QUESTIONNAIRE_RESP);
    
    for (int i = 0; i < resp->question_count; i++) {
        if (offset + static_cast<int>(sizeof(QUESTION_ITEM)) > respBody.size()) {
            qDebug() << "[DailyAssessmentPage] ERROR: Not enough data for QUESTION_ITEM" << i;
            showMessage("错误", "问卷数据解析失败");
            return false;
        }
        
        QUESTION_ITEM* item = reinterpret_cast<QUESTION_ITEM*>(respBody.data() + offset);
        offset += sizeof(QUESTION_ITEM);
        
        // 读取题干
        if (offset + item->stem_len > respBody.size()) {
            qDebug() << "[DailyAssessmentPage] ERROR: Not enough data for stem" << i;
            showMessage("错误", "问卷数据解析失败");
            return false;
        }
        QString stem = QString::fromUtf8(respBody.data() + offset, item->stem_len);
        offset += item->stem_len;
        
        // 读取选项JSON
        if (offset + item->options_len > respBody.size()) {
            qDebug() << "[DailyAssessmentPage] ERROR: Not enough data for options" << i;
            showMessage("错误", "问卷数据解析失败");
            return false;
        }
        QString optionsJson = QString::fromUtf8(respBody.data() + offset, item->options_len);
        offset += item->options_len;
        
        // 解析选项JSON (格式: [{"code":"A","label":"选项1","score":1.0},...]
        QStringList options;
        QStringList optionCodes;
        QList<float> optionScores;
        
        // 简单的JSON解析 - 注意：数据库中使用的是 "label" 而不是 "text"
        int pos = 0;
        while ((pos = optionsJson.indexOf("\"label\":\"", pos)) != -1) {
            pos += 9;  // 跳过 "label":"
            int endPos = optionsJson.indexOf("\"", pos);
            if (endPos != -1) {
                options.append(optionsJson.mid(pos, endPos - pos));
            }
            pos = endPos;
        }
        
        pos = 0;
        while ((pos = optionsJson.indexOf("\"code\":\"", pos)) != -1) {
            pos += 8;  // 跳过 "code":"
            int endPos = optionsJson.indexOf("\"", pos);
            if (endPos != -1) {
                optionCodes.append(optionsJson.mid(pos, endPos - pos));
            }
            pos = endPos;
        }
        
        pos = 0;
        while ((pos = optionsJson.indexOf("\"score\":", pos)) != -1) {
            pos += 8;  // 跳过 "score":
            int endPos = optionsJson.indexOf(",", pos);
            if (endPos == -1) endPos = optionsJson.indexOf("}", pos);
            if (endPos != -1) {
                optionScores.append(optionsJson.mid(pos, endPos - pos).toFloat());
            }
            pos = endPos;
        }
        
        Question q;
        q.id = item->question_id;
        q.text = stem;
        q.options = options;
        q.optionCodes = optionCodes;
        q.optionScores = optionScores;
        q.dimensionCode = QString::fromUtf8(item->dimension_code);
        
        m_questions.append(q);
        
        qDebug() << "[DailyAssessmentPage] Question" << i+1 << ":" << q.text
                 << "options:" << q.options.size();
    }
    
    // 初始化答案数组
    m_answers.clear();
    m_answers.resize(m_questions.size());
    for (int i = 0; i < m_answers.size(); i++) {
        m_answers[i] = -1;  // -1表示未选择
    }
    
    // 设置标题
    QString title;
    if (m_questionnaireType == "PHQ9") {
        title = "PHQ-9 抑郁症筛查量表";
    } else if (m_questionnaireType == "SAS") {
        title = "SAS 焦虑自评量表";
    } else {
        title = "每日心理快照";
    }
    ui->titleLabel->setText(title);
    
    // 重置当前题目索引
    m_currentQuestionIndex = 0;
    
    // 清空滚动区域，显示欢迎信息
    QWidget* welcomeContent = new QWidget();
    QVBoxLayout* welcomeLayout = new QVBoxLayout(welcomeContent);
    welcomeLayout->setSpacing(20);
    welcomeLayout->setContentsMargins(16, 40, 16, 16);
    
    QLabel* welcomeLabel = new QLabel(QString("准备开始测评\n\n本次测评共 %1 道题目\n请根据您的真实感受作答").arg(m_questions.size()));
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setWordWrap(true);
    welcomeLabel->setStyleSheet("QLabel { font-size: 18px; color: #666666; line-height: 2.0; }");
    welcomeLayout->addStretch();
    welcomeLayout->addWidget(welcomeLabel);
    welcomeLayout->addStretch();
    
    ui->scrollArea->setWidget(welcomeContent);
    ui->scrollArea->setWidgetResizable(true);
    
    // 显示开始按钮
    ui->submitButton->setVisible(true);
    ui->submitButton->setText("开始测评");
    ui->submitButton->setEnabled(true);
    
    // 初始化进度显示
    ui->progressLabel->setText(QString("已完成 0/%1 题").arg(m_questions.size()));
    
    return true;
}

void DailyAssessmentPage::displayCurrentQuestion()
{
    clearCurrentQuestion();
    
    if (m_currentQuestionIndex >= m_questions.size()) {
        return;
    }
    
    const Question& q = m_questions[m_currentQuestionIndex];
    
    // 创建滚动区域内容
    QWidget* scrollContent = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(scrollContent);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    
    // 问题卡片
    QWidget* questionCard = new QWidget();
    questionCard->setStyleSheet("QWidget { background-color: white; border-radius: 8px; padding: 20px; }");
    QVBoxLayout* cardLayout = new QVBoxLayout(questionCard);
    cardLayout->setSpacing(16);
    
    // 问题编号和文本
    QLabel* questionLabel = new QLabel(QString("第 %1 题 / 共 %2 题\n\n%3")
        .arg(m_currentQuestionIndex + 1)
        .arg(m_questions.size())
        .arg(q.text));
    questionLabel->setWordWrap(true);
    questionLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; color: #333333; line-height: 1.5; }");
    cardLayout->addWidget(questionLabel);
    
    // 添加分隔线
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("QFrame { background-color: #EEEEEE; max-height: 1px; }");
    cardLayout->addWidget(line);
    
    // 选项按钮组
    m_currentButtonGroup = new QButtonGroup(this);
    m_currentButtonGroup->setExclusive(true);
    
    for (int j = 0; j < q.options.size(); j++) {
        QPushButton* optionBtn = new QPushButton(QString("%1. %2").arg(char('A' + j)).arg(q.options[j]));
        optionBtn->setMinimumHeight(50);
        optionBtn->setStyleSheet(
            "QPushButton {"
            "   background-color: #F8F8F8;"
            "   color: #333333;"
            "   border: 2px solid #E0E0E0;"
            "   border-radius: 8px;"
            "   padding: 12px;"
            "   font-size: 15px;"
            "   text-align: left;"
            "}"
            "QPushButton:hover {"
            "   background-color: #E8F5E9;"
            "   border-color: #07C160;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #C8E6C9;"
            "}"
        );
        
        // 连接点击信号
        connect(optionBtn, &QPushButton::clicked, this, [this, j]() {
            onOptionClicked(j);
        });
        
        cardLayout->addWidget(optionBtn);
    }
    
    mainLayout->addWidget(questionCard);
    mainLayout->addStretch();
    
    // 设置滚动区域
    ui->scrollArea->setWidget(scrollContent);
    ui->scrollArea->setWidgetResizable(true);
    
    // 更新进度
    ui->progressLabel->setText(QString("已完成 %1/%2 题")
        .arg(m_currentQuestionIndex)
        .arg(m_questions.size()));
    
    // 隐藏提交按钮（在答题过程中）
    ui->submitButton->setVisible(false);
}

void DailyAssessmentPage::clearCurrentQuestion()
{
    // 清理按钮组
    if (m_currentButtonGroup) {
        delete m_currentButtonGroup;
        m_currentButtonGroup = nullptr;
    }
    
    // 清理滚动区域内容
    if (ui->scrollArea->widget()) {
        delete ui->scrollArea->widget();
    }
}

void DailyAssessmentPage::onOptionClicked(int optionIndex)
{
    qDebug() << "[DailyAssessmentPage] Option clicked:" << optionIndex
             << "for question" << m_currentQuestionIndex;
    
    // 保存答案
    m_answers[m_currentQuestionIndex] = optionIndex;
    
    // 移动到下一题
    m_currentQuestionIndex++;
    
    // 更新进度
    ui->progressLabel->setText(QString("已完成 %1/%2 题")
        .arg(m_currentQuestionIndex)
        .arg(m_questions.size()));
    
    // 检查是否完成所有题目
    if (m_currentQuestionIndex >= m_questions.size()) {
        // 所有题目已完成，显示确认对话框
        showConfirmDialog();
    } else {
        // 显示下一题
        displayCurrentQuestion();
    }
}

void DailyAssessmentPage::showNextQuestion()
{
    if (m_currentQuestionIndex < m_questions.size()) {
        displayCurrentQuestion();
    }
}

void DailyAssessmentPage::showConfirmDialog()
{
    clearCurrentQuestion();
    
    // 创建确认界面
    QWidget* confirmContent = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(confirmContent);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(16, 40, 16, 16);
    
    // 完成图标和文字
    QLabel* iconLabel = new QLabel("✓");
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("QLabel { font-size: 72px; color: #07C160; }");
    mainLayout->addWidget(iconLabel);
    
    QLabel* titleLabel = new QLabel("测评已完成");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("QLabel { font-size: 24px; font-weight: bold; color: #333333; }");
    mainLayout->addWidget(titleLabel);
    
    QLabel* descLabel = new QLabel(QString("您已完成全部 %1 道题目\n是否提交测评结果？").arg(m_questions.size()));
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("QLabel { font-size: 16px; color: #666666; line-height: 1.8; }");
    mainLayout->addWidget(descLabel);
    
    mainLayout->addStretch();
    
    // 按钮区域
    QWidget* buttonWidget = new QWidget();
    QVBoxLayout* buttonLayout = new QVBoxLayout(buttonWidget);
    buttonLayout->setSpacing(12);
    
    QPushButton* confirmBtn = new QPushButton("确认提交");
    confirmBtn->setMinimumHeight(44);
    confirmBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #07C160;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 22px;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #06AD56;"
        "}"
    );
    connect(confirmBtn, &QPushButton::clicked, this, [this]() {
        // 直接调用提交逻辑
        submitAssessment();
    });
    buttonLayout->addWidget(confirmBtn);
    
    QPushButton* cancelBtn = new QPushButton("重新作答");
    cancelBtn->setMinimumHeight(44);
    cancelBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #F8F8F8;"
        "   color: #666666;"
        "   border: 1px solid #E0E0E0;"
        "   border-radius: 22px;"
        "   font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #EEEEEE;"
        "}"
    );
    connect(cancelBtn, &QPushButton::clicked, this, [this]() {
        // 重置所有答案
        for (int i = 0; i < m_answers.size(); i++) {
            m_answers[i] = -1;
        }
        m_currentQuestionIndex = 0;
        displayCurrentQuestion();
    });
    buttonLayout->addWidget(cancelBtn);
    
    mainLayout->addWidget(buttonWidget);
    
    // 设置滚动区域
    ui->scrollArea->setWidget(confirmContent);
    ui->scrollArea->setWidgetResizable(true);
    
    // 隐藏底部按钮
    ui->submitButton->setVisible(false);
}

bool DailyAssessmentPage::validateAnswers()
{
    for (int i = 0; i < m_answers.size(); i++) {
        if (m_answers[i] == -1) {
            return false;
        }
    }
    return true;
}

void DailyAssessmentPage::onSubmitClicked()
{
    // 如果是开始按钮，显示第一题
    if (ui->submitButton->text() == "开始测评") {
        m_currentQuestionIndex = 0;
        displayCurrentQuestion();
        return;
    }
    
    // 否则调用提交逻辑
    submitAssessment();
}

void DailyAssessmentPage::submitAssessment()
{
    if (!validateAnswers()) {
        showMessage("提示", "请完成所有问题后再提交");
        return;
    }
    
    qDebug() << "[DailyAssessmentPage] Submitting assessment...";
    
    // 构造请求
    int reqSize = sizeof(SUBMIT_ASSESSMENT_REQ) + m_answers.size() * sizeof(ANSWER_ITEM);
    QByteArray reqBody;
    reqBody.resize(reqSize);
    
    SUBMIT_ASSESSMENT_REQ* req = reinterpret_cast<SUBMIT_ASSESSMENT_REQ*>(reqBody.data());
    memset(req, 0, sizeof(SUBMIT_ASSESSMENT_REQ));
    
    strncpy(req->user_id, m_userId.toUtf8().constData(), sizeof(req->user_id) - 1);
    req->questionnaire_id = m_questionnaireId;
    strncpy(req->questionnaire_code, m_questionnaireType.toUtf8().constData(), sizeof(req->questionnaire_code) - 1);
    strncpy(req->assessment_tag, "DAILY", sizeof(req->assessment_tag) - 1);
    req->answer_count = m_answers.size();
    
    // 填充答案
    ANSWER_ITEM* answers = reinterpret_cast<ANSWER_ITEM*>(reqBody.data() + sizeof(SUBMIT_ASSESSMENT_REQ));
    for (int i = 0; i < m_answers.size(); i++) {
        const Question& q = m_questions[i];
        int selectedIdx = m_answers[i];
        
        answers[i].question_id = q.id;
        strncpy(answers[i].option_code, q.optionCodes[selectedIdx].toUtf8().constData(), sizeof(answers[i].option_code) - 1);
        answers[i].option_score = q.optionScores[selectedIdx];
        strncpy(answers[i].dimension_code, q.dimensionCode.toUtf8().constData(), sizeof(answers[i].dimension_code) - 1);
    }
    
    // 发送请求
    HEAD respHead;
    QByteArray respBody = FrontClient::instance().sendAndReceive(SUBMIT_ASSESSMENT, reqBody, &respHead);
    
    if (respBody.isEmpty()) {
        showMessage("错误", "无法连接到服务器");
        return;
    }
    
    if (respBody.size() < static_cast<int>(sizeof(SUBMIT_ASSESSMENT_RESP))) {
        showMessage("错误", "服务器响应数据不完整");
        return;
    }
    
    SUBMIT_ASSESSMENT_RESP* resp = reinterpret_cast<SUBMIT_ASSESSMENT_RESP*>(respBody.data());
    
    if (resp->status_code != 0) {
        if (resp->status_code == 1) {
            showMessage("提示", "您今天已经提交过评估了");
        } else {
            showMessage("错误", "提交失败，请重试");
        }
        return;
    }
    
    // 读取建议文本
    int offset = sizeof(SUBMIT_ASSESSMENT_RESP);
    QString suggestion;
    if (resp->suggestion_len > 0 && offset + resp->suggestion_len <= respBody.size()) {
        suggestion = QString::fromUtf8(respBody.data() + offset, resp->suggestion_len);
    }
    
    // 提交成功，标记今日已完成
    markTodayCompleted();
    
    QString riskLevel = QString::fromUtf8(resp->risk_level);
    float totalScore = resp->total_score;  // 保存分数到局部变量
    
    qDebug() << "[DailyAssessmentPage] Assessment completed! Score:" << totalScore
             << "Risk:" << riskLevel;
    
    // 显示完成消息（使用非阻塞方式）
    QMessageBox* msgBox = new QMessageBox(this);
    msgBox->setWindowTitle("完成测评");
    msgBox->setText(QString("测评已完成！\n总分：%1\n风险等级：%2")
        .arg(totalScore)
        .arg(riskLevel));
    msgBox->setIcon(QMessageBox::Information);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setModal(false);
    msgBox->show();
    
    // 1.5秒后自动关闭消息框并返回首页
    // 使用QPointer防止访问已删除的对象
    QPointer<QMessageBox> msgBoxPtr(msgBox);
    QTimer::singleShot(1500, this, [this, msgBoxPtr, totalScore, riskLevel, suggestion]() {
        // 安全检查:只有当msgBox还存在时才关闭
        if (msgBoxPtr && !msgBoxPtr.isNull()) {
            msgBoxPtr->close();
        }
        
        // 发送信号（可能用于显示报告页面）
        emit assessmentCompleted(totalScore, riskLevel, suggestion);
        
        // 返回首页
        emit backRequested();
    });
}

void DailyAssessmentPage::onBackClicked()
{
    emit backRequested();
}

void DailyAssessmentPage::showMessage(const QString& title, const QString& message)
{
    QMessageBox::information(this, title, message);
}

QString DailyAssessmentPage::getTodayKey()
{
    // 生成今日的唯一键：用户ID + 日期
    QDate today = QDate::currentDate();
    return QString("%1_%2").arg(m_userId).arg(today.toString("yyyyMMdd"));
}

void DailyAssessmentPage::markTodayCompleted()
{
    // 改为依赖服务端状态，避免本地标记导致误判
    m_hasCompletedToday = false;
}

bool DailyAssessmentPage::checkTodayCompleted()
{
    // 不使用本地标记，始终交由服务端判定
    qDebug() << "[DailyAssessmentPage] Skip local completion check, rely on server";
    return false;
}
