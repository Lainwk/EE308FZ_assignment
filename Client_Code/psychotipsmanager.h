#ifndef PSYCHOTIPSMANAGER_H
#define PSYCHOTIPSMANAGER_H

#include <QObject>
#include <QTimer>
#include <QString>
#include <QStringList>
#include <QDateTime>

/**
 * @brief 心理小tips管理类
 * 
 * 单例模式，负责管理和推送心理健康小贴士。
 * 每12小时推送一条随机的心理小tips。
 */
class PsychoTipsManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 获取单例实例
     * @return PsychoTipsManager& 单例引用
     */
    static PsychoTipsManager& instance();

    /**
     * @brief 启动tips推送服务
     * 
     * 启动定时器，每12小时推送一条心理小tips。
     */
    void start();

    /**
     * @brief 停止tips推送服务
     */
    void stop();

    /**
     * @brief 检查tips推送功能是否启用
     * @return bool true表示启用，false表示禁用
     */
    bool isEnabled() const;

    /**
     * @brief 设置tips推送功能启用状态
     * @param enabled true启用，false禁用
     */
    void setEnabled(bool enabled);

    /**
     * @brief 获取一条随机的心理小tips
     * @return QString 心理小tips内容
     */
    QString getRandomTip() const;

signals:
    /**
     * @brief tips推送信号
     * @param tip tips内容
     */
    void tipPushed(const QString& tip);

private slots:
    /**
     * @brief 定时器触发时的推送槽函数
     */
    void onTimerTimeout();

private:
    // 私有构造函数（单例模式）
    explicit PsychoTipsManager(QObject *parent = nullptr);
    ~PsychoTipsManager();

    // 禁止拷贝和赋值
    PsychoTipsManager(const PsychoTipsManager&) = delete;
    PsychoTipsManager& operator=(const PsychoTipsManager&) = delete;

    /**
     * @brief 初始化心理小tips数据
     */
    void initializeTips();

    /**
     * @brief 显示Windows Toast通知
     * @param title 通知标题
     * @param message 通知内容
     */
    void showWindowsToast(const QString& title, const QString& message);

    /**
     * @brief 检查是否应该推送tips
     * @return bool true表示应该推送，false表示不推送
     */
    bool shouldPushTip() const;

    /**
     * @brief 更新最后推送时间
     */
    void updateLastPushTime();

    QTimer* m_timer;                // 定时器
    QStringList m_tips;             // 心理小tips列表
    bool m_enabled;                 // 是否启用推送
    bool m_isRunning;               // 服务是否正在运行
    QDateTime m_lastPushTime;       // 最后推送时间
};

#endif // PSYCHOTIPSMANAGER_H