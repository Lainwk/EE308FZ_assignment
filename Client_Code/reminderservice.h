#ifndef REMINDERSERVICE_H
#define REMINDERSERVICE_H

#include <QObject>
#include <QTimer>
#include <QString>

/**
 * @brief 消息提醒服务类
 * 
 * 单例模式，负责定时检查用户是否完成每日测评，
 * 并在Windows系统上显示Toast通知提醒用户。
 */
class ReminderService : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 获取单例实例
     * @return ReminderService& 单例引用
     */
    static ReminderService& instance();

    /**
     * @brief 启动提醒服务
     * @param userId 用户ID
     * 
     * 启动定时器，每小时检查一次用户是否完成每日测评。
     * 如果未完成，则显示Windows Toast通知。
     */
    void start(const QString& userId);

    /**
     * @brief 停止提醒服务
     * 
     * 停止定时器，清除用户ID。
     * 通常在用户登出时调用。
     */
    void stop();

    /**
     * @brief 检查提醒功能是否启用
     * @return bool true表示启用，false表示禁用
     */
    bool isEnabled() const;

    /**
     * @brief 设置提醒功能启用状态
     * @param enabled true启用，false禁用
     * 
     * 设置会保存到本地配置中。
     */
    void setEnabled(bool enabled);

signals:
    /**
     * @brief 检查完成信号
     * @param completed 是否已完成测评
     * 
     * 当检查完成后发出此信号，通知UI更新状态。
     */
    void checkCompleted(bool completed);

private slots:
    /**
     * @brief 定时器触发时的检查槽函数
     * 
     * 每小时触发一次，检查用户是否完成每日测评。
     */
    void onTimerTimeout();

private:
    // 私有构造函数（单例模式）
    explicit ReminderService(QObject *parent = nullptr);
    ~ReminderService();

    // 禁止拷贝和赋值
    ReminderService(const ReminderService&) = delete;
    ReminderService& operator=(const ReminderService&) = delete;

    /**
     * @brief 检查用户是否完成今日测评
     * @return bool true表示已完成，false表示未完成
     * 
     * 调用CHECK_TODAY_ASSESSMENT API检查服务器记录。
     */
    bool checkTodayAssessment();

    /**
     * @brief 显示Windows Toast通知
     * @param title 通知标题
     * @param message 通知内容
     * 
     * 使用Windows 10/11的Toast通知API显示系统通知。
     */
    void showWindowsToast(const QString& title, const QString& message);

    /**
     * @brief 获取随机的可爱提醒消息
     * @return QString 提醒消息
     */
    QString getRandomReminderMessage() const;

    /**
     * @brief 检查当前是否为整点
     * @return bool true表示是整点，false表示不是
     */
    bool isOnTheHour() const;

    QTimer* m_timer;           // 定时器
    QString m_userId;          // 当前用户ID
    bool m_enabled;            // 是否启用提醒
    bool m_isRunning;          // 服务是否正在运行
    QStringList m_reminderMessages;  // 可爱的提醒消息列表
};

#endif // REMINDERSERVICE_H