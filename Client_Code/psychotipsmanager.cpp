#include "psychotipsmanager.h"
#include <QSettings>
#include <QDebug>
#include <QRandomGenerator>
#include <QProcess>

// Windows Toast通知相关头文件
#ifdef Q_OS_WIN
#include <windows.h>
#endif

PsychoTipsManager::PsychoTipsManager(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_enabled(true)
    , m_isRunning(false)
{
    // 从配置中读取启用状态和最后推送时间
    QSettings settings("MindCarrer", "PsychoTipsManager");
    m_enabled = settings.value("enabled", true).toBool();
    m_lastPushTime = settings.value("lastPushTime", QDateTime()).toDateTime();
    
    // 初始化心理小tips数据
    initializeTips();
    
    // 设置定时器为10分钟检查一次（600000毫秒）
    // 实际推送间隔由shouldPushTip()控制为12小时
    m_timer->setInterval(600000);
    
    // 连接定时器信号
    connect(m_timer, &QTimer::timeout, this, &PsychoTipsManager::onTimerTimeout);
    
    qDebug() << "[PsychoTipsManager] Service initialized, enabled:" << m_enabled
             << ", last push:" << m_lastPushTime.toString("yyyy-MM-dd hh:mm:ss");
}

PsychoTipsManager::~PsychoTipsManager()
{
    stop();
}

PsychoTipsManager& PsychoTipsManager::instance()
{
    static PsychoTipsManager instance;
    return instance;
}

void PsychoTipsManager::start()
{
    if (m_isRunning) {
        qDebug() << "[PsychoTipsManager] Already running";
        return;
    }
    
    m_isRunning = true;
    
    if (m_enabled) {
        m_timer->start();
        qDebug() << "[PsychoTipsManager] Started, check interval: 10 minutes, push interval: 12 hours";
        
        // 启动后立即检查是否需要推送
        onTimerTimeout();
    } else {
        qDebug() << "[PsychoTipsManager] Service is disabled, not starting timer";
    }
}

void PsychoTipsManager::stop()
{
    if (!m_isRunning) {
        return;
    }
    
    m_timer->stop();
    m_isRunning = false;
    
    qDebug() << "[PsychoTipsManager] Service stopped";
}

bool PsychoTipsManager::isEnabled() const
{
    return m_enabled;
}

void PsychoTipsManager::setEnabled(bool enabled)
{
    if (m_enabled == enabled) {
        return;
    }
    
    m_enabled = enabled;
    
    // 保存到配置
    QSettings settings("MindCarrer", "PsychoTipsManager");
    settings.setValue("enabled", enabled);
    
    qDebug() << "[PsychoTipsManager] Enabled state changed to:" << enabled;
    
    // 如果正在运行，根据新状态启动或停止定时器
    if (m_isRunning) {
        if (enabled) {
            m_timer->start();
            qDebug() << "[PsychoTipsManager] Timer restarted";
        } else {
            m_timer->stop();
            qDebug() << "[PsychoTipsManager] Timer stopped";
        }
    }
}

QString PsychoTipsManager::getRandomTip() const
{
    if (m_tips.isEmpty()) {
        return QStringLiteral("保持积极的心态，关注自己的心理健康！");
    }
    
    int index = QRandomGenerator::global()->bounded(m_tips.size());
    return m_tips.at(index);
}

void PsychoTipsManager::onTimerTimeout()
{
    if (!m_enabled) {
        return;
    }
    
    // 检查是否应该推送tips
    if (!shouldPushTip()) {
        return;
    }
    
    // 获取随机tips
    QString tip = getRandomTip();
    
    qDebug() << "[PsychoTipsManager] Pushing tip:" << tip;
    
    // 显示通知
    showWindowsToast(
        QStringLiteral("心屿 - 每日心理小贴士 💡"),
        tip
    );
    
    // 更新最后推送时间
    updateLastPushTime();
    
    // 发射信号
    emit tipPushed(tip);
}

void PsychoTipsManager::initializeTips()
{
    m_tips.clear();
    
    // 添加心理健康小贴士（包含可爱的颜文字）
    m_tips << QStringLiteral("每天给自己一个微笑，心情会更好哦！(◕‿◕)");
    m_tips << QStringLiteral("适当的运动可以释放压力，让心情变得愉悦～ ٩(◕‿◕｡)۶");
    m_tips << QStringLiteral("学会倾听自己的内心，了解真实的感受很重要呢！(｡♥‿♥｡)");
    m_tips << QStringLiteral("保持规律的作息，充足的睡眠是心理健康的基础！(｡･ω･｡)");
    m_tips << QStringLiteral("与朋友分享快乐，倾诉烦恼，你不是一个人在战斗！(づ｡◕‿‿◕｡)づ");
    m_tips << QStringLiteral("学会接纳不完美的自己，每个人都是独一无二的！(ﾉ◕ヮ◕)ﾉ*:･ﾟ✧");
    m_tips << QStringLiteral("深呼吸，放松心情，给自己一点时间和空间～ (´｡• ᵕ •｡`)");
    m_tips << QStringLiteral("培养一个兴趣爱好，让生活更加丰富多彩！(ﾉ´ヮ`)ﾉ*: ･ﾟ");
    m_tips << QStringLiteral("学会说"不"，保护自己的边界很重要哦！(｡•̀ᴗ-)✧");
    m_tips << QStringLiteral("感恩生活中的小确幸，积极的心态带来好运气！(◠‿◠)");
    m_tips << QStringLiteral("适当的独处时间，可以帮助你更好地认识自己～ (｡･ω･｡)ﾉ♡");
    m_tips << QStringLiteral("遇到困难时，记得寻求帮助，勇敢不是独自承受！(っ˘̩╭╮˘̩)っ");
    m_tips << QStringLiteral("保持好奇心，探索新事物，让生活充满惊喜！(ﾉ◕ヮ◕)ﾉ");
    m_tips << QStringLiteral("学会放下过去，活在当下，珍惜眼前的美好！(◕‿◕✿)");
    m_tips << QStringLiteral("给自己设定小目标，完成后记得奖励自己哦！(｡♥‿♥｡)");
    m_tips << QStringLiteral("多接触大自然，阳光和绿色能治愈心灵～ ☀(◕‿◕)");
    m_tips << QStringLiteral("学会自我关怀，像对待好朋友一样对待自己！(づ￣ ³￣)づ");
    m_tips << QStringLiteral("保持乐观，相信明天会更好，你值得拥有幸福！(ﾉ◕ヮ◕)ﾉ*:･ﾟ✧");
    m_tips << QStringLiteral("适度使用社交媒体，避免过度比较带来的焦虑～ (｡•́︿•̀｡)");
    m_tips << QStringLiteral("培养感恩的心，记录每天的三件好事！(◕‿◕)♡");
    m_tips << QStringLiteral("学会情绪管理，接纳自己的情绪是成长的第一步！(｡･ω･｡)");
    m_tips << QStringLiteral("保持社交联系，良好的人际关系是心理健康的支柱！(◠‿◠✿)");
    m_tips << QStringLiteral("尝试冥想或正念练习，让心灵得到平静～ (´｡• ω •｡`)");
    m_tips << QStringLiteral("不要害怕失败，每次挫折都是成长的机会！(ง •̀_•́)ง");
    m_tips << QStringLiteral("保持幽默感，笑一笑，十年少！(◕‿◕)ノ");
    m_tips << QStringLiteral("学会时间管理，合理安排工作和休息时间～ (｡♥‿♥｡)");
    m_tips << QStringLiteral("培养同理心，理解他人也是理解自己的过程！(◕‿◕✿)");
    m_tips << QStringLiteral("保持好奇心和学习热情，终身学习让生活更精彩！(ﾉ◕ヮ◕)ﾉ");
    m_tips << QStringLiteral("学会放松技巧，如渐进性肌肉放松，缓解身心压力～ (｡･ω･｡)");
    m_tips << QStringLiteral("建立健康的生活习惯，规律作息、均衡饮食很重要！(◠‿◠)");
    
    qDebug() << "[PsychoTipsManager] Initialized" << m_tips.size() << "tips";
}

void PsychoTipsManager::showWindowsToast(const QString& title, const QString& message)
{
#ifdef Q_OS_WIN
    // Windows 10/11 Toast通知实现
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
        qWarning() << "[PsychoTipsManager] Failed to start PowerShell for toast notification";
        
        // 降级方案：使用Windows消息框
        QString fullMessage = title + "\n\n" + message;
        QByteArray titleBytes = title.toLocal8Bit();
        QByteArray messageBytes = fullMessage.toLocal8Bit();
        
        MessageBoxA(NULL, messageBytes.constData(), titleBytes.constData(), MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
        return;
    }
    
    if (!process.waitForFinished(5000)) {
        qWarning() << "[PsychoTipsManager] PowerShell process timeout";
        process.kill();
        return;
    }
    
    int exitCode = process.exitCode();
    if (exitCode != 0) {
        QString error = QString::fromLocal8Bit(process.readAllStandardError());
        qWarning() << "[PsychoTipsManager] PowerShell error:" << error;
    } else {
        qDebug() << "[PsychoTipsManager] Toast notification shown successfully";
    }
    
#else
    // 非Windows平台，输出日志
    qDebug() << "[PsychoTipsManager] Toast notification (non-Windows):" << title << "-" << message;
#endif
}

bool PsychoTipsManager::shouldPushTip() const
{
    // 如果从未推送过，应该推送
    if (!m_lastPushTime.isValid()) {
        return true;
    }
    
    // 计算距离上次推送的时间（小时）
    qint64 hoursSinceLastPush = m_lastPushTime.secsTo(QDateTime::currentDateTime()) / 3600;
    
    // 如果超过12小时，应该推送
    if (hoursSinceLastPush >= 12) {
        qDebug() << "[PsychoTipsManager] Should push tip, hours since last push:" << hoursSinceLastPush;
        return true;
    }
    
    return false;
}

void PsychoTipsManager::updateLastPushTime()
{
    m_lastPushTime = QDateTime::currentDateTime();
    
    // 保存到配置
    QSettings settings("MindCarrer", "PsychoTipsManager");
    settings.setValue("lastPushTime", m_lastPushTime);
    
    qDebug() << "[PsychoTipsManager] Updated last push time:" << m_lastPushTime.toString("yyyy-MM-dd hh:mm:ss");
}