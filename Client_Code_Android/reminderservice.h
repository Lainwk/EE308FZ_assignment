#ifndef REMINDERSERVICE_H
#define REMINDERSERVICE_H

#include <QObject>
#include <QTimer>
#include <QString>
#include <QStringList>

/**
 * @brief 消息提醒服务类
 * 
 * 单例模式，负责定期检查用户是否完成每日测评，
 * 并显示通知提醒用户。
 * - 每5分钟检查一次
 * - 只在整点触发提醒
 * - 工作时间：9:00-21:00
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
     * 启动定时器，每5分钟检查一次，在整点时检查用户是否完成每日测评。
     * 如果未完成，则显示通知。
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
     * 每5分钟触发一次，在整点时检查用户是否完成每日测评。
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
     * @brief 显示通知
     * @param title 通知标题
     * @param message 通知内容
     * 
     * 根据平台显示相应的通知（Android或其他平台）。
     */
    void showNotification(const QString& title, const QString& message);

    /**
     * @brief 获取随机提醒消息
     * @return QString 随机选择的提醒消息
     */
    QString getRandomReminderMessage() const;

    /**
     * @brief 检查是否为整点
     * @return bool true表示当前为整点（前后5分钟内）
     */
    bool isOnTheHour() const;

    QTimer* m_timer;                    // 定时器
    QString m_userId;                   // 当前用户ID
    bool m_enabled;                     // 是否启用提醒
    bool m_isRunning;                   // 服务是否正在运行
    QStringList m_reminderMessages;     // 提醒消息列表
};

#endif // REMINDERSERVICE_H