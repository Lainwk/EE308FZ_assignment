#include "reminderservice.h"
#include "frontclient.h"
#include "localstore.h"

#include <QSettings>
#include <QDebug>
#include <QDateTime>
#include <QProcess>
#include <QCoreApplication>
#include <QRandomGenerator>
#include <QStringList>

// Windows Toast通知相关头文件
#ifdef Q_OS_WIN
#include <windows.h>
#include <shobjidl.h>
#include <propvarutil.h>
#include <propkey.h>
#include <wrl/client.h>
using namespace Microsoft::WRL;
#endif

ReminderService::ReminderService(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_enabled(true)
    , m_isRunning(false)
{
    // 从配置中读取启用状态
    QSettings settings("MindCarrer", "ReminderService");
    m_enabled = settings.value("enabled", true).toBool();
    
    // 设置定时器为5分钟检查一次（300000毫秒）
    // 实际提醒只在整点触发
    m_timer->setInterval(300000);
    
    // 连接定时器信号
    connect(m_timer, &QTimer::timeout, this, &ReminderService::onTimerTimeout);
    
    // 初始化可爱的提醒消息
    m_reminderMessages << QStringLiteral("嘿！记得做今天的心理测评哦~ (◕‿◕)✿")
                       << QStringLiteral("小主人，该关注一下自己的心理健康啦！(｡♥‿♥｡)")
                       << QStringLiteral("今日测评还没完成呢，快来看看吧！(ﾉ◕ヮ◕)ﾉ*:･ﾟ✧")
                       << QStringLiteral("别忘了今天的心理测评哦，关心自己很重要！(◠‿◠✿)")
                       << QStringLiteral("温馨提示：今日测评等你来完成呢~ (｡･ω･｡)ﾉ♡")
                       << QStringLiteral("嗨！花几分钟做个测评，了解自己的心理状态吧！(◕‿◕)♡")
                       << QStringLiteral("今天的心理测评还在等你哦，快来吧！(ﾉ´ヮ`)ﾉ*: ･ﾟ")
                       << QStringLiteral("记得关注自己的心理健康，今日测评走起！(｡•̀ᴗ-)✧")
                       << QStringLiteral("小可爱，该做今天的心理测评啦~ (づ｡◕‿‿◕｡)づ")
                       << QStringLiteral("别忘了今日测评哦，了解自己才能更好地成长！(◕‿◕✿)")
                       << QStringLiteral("嘿！今天的心理测评还没做呢，快来完成吧！(ﾉ◕ヮ◕)ﾉ")
                       << QStringLiteral("温馨提醒：今日测评时间到啦~ (´｡• ᵕ •｡`) ♡")
                       << QStringLiteral("小主，该做心理测评了，关心自己从现在开始！(◠‿◠)")
                       << QStringLiteral("今天的测评还没完成哦，快来看看自己的心理状态吧！(｡･ω･｡)")
                       << QStringLiteral("别忘了关注自己的心理健康，今日测评等你来！(◕‿◕)ノ");
    
    qDebug() << "[ReminderService] Service initialized, enabled:" << m_enabled
             << ", check interval: 5 minutes, reminder on the hour";
}

ReminderService::~ReminderService()
{
    stop();
}

ReminderService& ReminderService::instance()
{
    static ReminderService instance;
    return instance;
}

void ReminderService::start(const QString& userId)
{
    if (userId.isEmpty()) {
        qWarning() << "[ReminderService] Cannot start with empty userId";
        return;
    }
    
    if (m_isRunning && m_userId == userId) {
        qDebug() << "[ReminderService] Already running for user:" << userId;
        return;
    }
    
    m_userId = userId;
    m_isRunning = true;
    
    if (m_enabled) {
        m_timer->start();
        qDebug() << "[ReminderService] Started for user:" << userId << ", check interval: 1 hour";
        
        // 启动后立即执行一次检查
        onTimerTimeout();
    } else {
        qDebug() << "[ReminderService] Service is disabled, not starting timer";
    }
}

void ReminderService::stop()
{
    if (!m_isRunning) {
        return;
    }
    
    m_timer->stop();
    m_userId.clear();
    m_isRunning = false;
    
    qDebug() << "[ReminderService] Service stopped";
}

bool ReminderService::isEnabled() const
{
    return m_enabled;
}

void ReminderService::setEnabled(bool enabled)
{
    if (m_enabled == enabled) {
        return;
    }
    
    m_enabled = enabled;
    
    // 保存到配置
    QSettings settings("MindCarrer", "ReminderService");
    settings.setValue("enabled", enabled);
    
    qDebug() << "[ReminderService] Enabled state changed to:" << enabled;
    
    // 如果正在运行，根据新状态启动或停止定时器
    if (m_isRunning) {
        if (enabled) {
            m_timer->start();
            qDebug() << "[ReminderService] Timer restarted";
        } else {
            m_timer->stop();
            qDebug() << "[ReminderService] Timer stopped";
        }
    }
}

void ReminderService::onTimerTimeout()
{
    if (!m_enabled || m_userId.isEmpty()) {
        return;
    }
    
    // 检查是否为整点
    if (!isOnTheHour()) {
        return;
    }
    
    QDateTime now = QDateTime::currentDateTime();
    int hour = now.time().hour();
    
    // 只在工作时间（9:00-21:00）的整点检查
    if (hour < 9 || hour > 21) {
        qDebug() << "[ReminderService] Outside reminder hours (9:00-21:00), skipping check";
        return;
    }
    
    qDebug() << "[ReminderService] On the hour check for user:" << m_userId;
    
    bool completed = checkTodayAssessment();
    
    emit checkCompleted(completed);
    
    if (!completed) {
        // 未完成测评，显示可爱的提醒
        QString message = getRandomReminderMessage();
        
        showWindowsToast(
            QStringLiteral("心屿 - 每日测评提醒 💝"),
            message
        );
        qDebug() << "[ReminderService] Reminder shown to user:" << message;
    } else {
        qDebug() << "[ReminderService] User has completed today's assessment";
    }
}

bool ReminderService::checkTodayAssessment()
{
    CHECK_TODAY_ASSESSMENT_REQ req;
    memset(&req, 0, sizeof(req));
    
    QByteArray userIdBytes = m_userId.toUtf8();
    strncpy(req.user_id, userIdBytes.constData(), sizeof(req.user_id) - 1);
    req.user_id[sizeof(req.user_id) - 1] = '\0';
    
    QByteArray body(reinterpret_cast<const char*>(&req), sizeof(req));
    HEAD respHead;
    QByteArray respBody = FrontClient::instance().sendAndReceive(CHECK_TODAY_ASSESSMENT, body, &respHead);
    
    if (respBody.isEmpty() || respBody.size() < static_cast<int>(sizeof(CHECK_TODAY_ASSESSMENT_RESP))) {
        qWarning() << "[ReminderService] Invalid response from server";
        return false;
    }
    
    const CHECK_TODAY_ASSESSMENT_RESP* resp = reinterpret_cast<const CHECK_TODAY_ASSESSMENT_RESP*>(respBody.constData());
    
    if (resp->status_code != 0) {
        qWarning() << "[ReminderService] Server returned error, status_code:" << resp->status_code;
        return false;
    }
    
    bool completed = (resp->completed == 1);
    
    if (completed) {
        QString lastTime = QString::fromLatin1(resp->last_completed_time).trimmed();
        float lastScore = resp->last_score;
        qDebug() << "[ReminderService] Last assessment:" << lastTime << ", score:" << lastScore;
    }
    
    return completed;
}

void ReminderService::showWindowsToast(const QString& title, const QString& message)
{
#ifdef Q_OS_WIN
    // Windows 10/11 Toast通知实现
    // 使用PowerShell脚本显示Toast通知（简化实现）
    QString script = QString(
        "$app = '{00000000-0000-0000-0000-000000000001}\\MindCarrer\\MindCarrer.exe';"
        "[Windows.UI.Notifications.ToastNotificationManager, Windows.UI.Notifications, ContentType = WindowsRuntime] | Out-Null;"
        "[Windows.Data.Xml.Dom.XmlDocument, Windows.Data.Xml.Dom.XmlDocument, ContentType = WindowsRuntime] | Out-Null;"
        "$template = @\"<toast><visual><binding template='ToastText02'><text id='1'>%1</text><text id='2'>%2</text></binding></visual></toast>\"@;"
        "$xml = New-Object Windows.Data.Xml.Dom.XmlDocument;"
        "$xml.LoadXml($template);"
        "$toast = New-Object Windows.UI.Notifications.ToastNotification $xml;"
        "[Windows.UI.Notifications.ToastNotificationManager]::CreateToastNotifier($app).Show($toast);"
    ).arg(title, message);
    
    QProcess process;
    QStringList args;
    args << "-NoProfile" << "-ExecutionPolicy" << "Bypass" << "-Command" << script;
    
    process.start("powershell.exe", args);
    
    if (!process.waitForStarted(3000)) {
        qWarning() << "[ReminderService] Failed to start PowerShell for toast notification";
        
        // 降级方案：使用Windows消息框
        QString fullMessage = title + "\n\n" + message;
        QByteArray titleBytes = title.toLocal8Bit();
        QByteArray messageBytes = fullMessage.toLocal8Bit();
        
        MessageBoxA(NULL, messageBytes.constData(), titleBytes.constData(), MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
        return;
    }
    
    if (!process.waitForFinished(5000)) {
        qWarning() << "[ReminderService] PowerShell process timeout";
        process.kill();
        
        // 降级方案：使用Windows消息框
        QString fullMessage = title + "\n\n" + message;
        QByteArray titleBytes = title.toLocal8Bit();
        QByteArray messageBytes = fullMessage.toLocal8Bit();
        
        MessageBoxA(NULL, messageBytes.constData(), titleBytes.constData(), MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
        return;
    }
    
    int exitCode = process.exitCode();
    if (exitCode != 0) {
        QString error = QString::fromLocal8Bit(process.readAllStandardError());
        qWarning() << "[ReminderService] PowerShell error:" << error;
        
        // 降级方案：使用Windows消息框
        QString fullMessage = title + "\n\n" + message;
        QByteArray titleBytes = title.toLocal8Bit();
        QByteArray messageBytes = fullMessage.toLocal8Bit();
        
        MessageBoxA(NULL, messageBytes.constData(), titleBytes.constData(), MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
    } else {
        qDebug() << "[ReminderService] Toast notification shown successfully";
    }
    
#else
    // 非Windows平台，输出日志
    qDebug() << "[ReminderService] Toast notification (non-Windows):" << title << "-" << message;
#endif
}

QString ReminderService::getRandomReminderMessage() const
{
    if (m_reminderMessages.isEmpty()) {
        return QStringLiteral("您今天还没有完成每日心理测评，记得关注自己的心理健康哦！(◕‿◕)");
    }
    
    int index = QRandomGenerator::global()->bounded(m_reminderMessages.size());
    return m_reminderMessages.at(index);
}

bool ReminderService::isOnTheHour() const
{
    QTime now = QTime::currentTime();
    // 检查是否在整点的前后5分钟内（避免错过整点）
    int minute = now.minute();
    return (minute >= 0 && minute < 5) || (minute >= 55 && minute < 60);
}