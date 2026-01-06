#ifndef PSYCHOTIPSMANAGER_H
#define PSYCHOTIPSMANAGER_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QStringList>

/**
 * @brief 心理小贴士管理器
 * 
 * 负责定期推送心理健康小贴士
 * - 每12小时推送一次
 * - 包含30条精心设计的心理健康建议
 * - 支持启用/禁用功能
 */
class PsychoTipsManager : public QObject
{
    Q_OBJECT
    
public:
    /**
     * @brief 获取单例实例
     */
    static PsychoTipsManager& instance();
    
    /**
     * @brief 启动服务
     */
    void start();
    
    /**
     * @brief 停止服务
     */
    void stop();
    
    /**
     * @brief 检查服务是否启用
     */
    bool isEnabled() const;
    
    /**
     * @brief 设置服务启用状态
     * @param enabled 是否启用
     */
    void setEnabled(bool enabled);
    
    /**
     * @brief 获取随机的心理小贴士
     * @return 随机选择的贴士内容
     */
    QString getRandomTip() const;
    
signals:
    /**
     * @brief 推送了新的贴士
     * @param tip 贴士内容
     */
    void tipPushed(const QString& tip);
    
private:
    explicit PsychoTipsManager(QObject *parent = nullptr);
    ~PsychoTipsManager();
    
    // 禁止拷贝
    PsychoTipsManager(const PsychoTipsManager&) = delete;
    PsychoTipsManager& operator=(const PsychoTipsManager&) = delete;
    
    /**
     * @brief 定时器超时处理
     */
    void onTimerTimeout();
    
    /**
     * @brief 初始化心理小贴士数据
     */
    void initializeTips();
    
    /**
     * @brief 显示通知
     * @param title 通知标题
     * @param message 通知内容
     */
    void showNotification(const QString& title, const QString& message);
    
    /**
     * @brief 检查是否应该推送贴士
     * @return true表示应该推送
     */
    bool shouldPushTip() const;
    
    /**
     * @brief 更新最后推送时间
     */
    void updateLastPushTime();
    
private:
    QTimer* m_timer;                    // 定时器
    QStringList m_tips;                 // 心理小贴士列表
    bool m_enabled;                     // 是否启用
    bool m_isRunning;                   // 是否正在运行
    QDateTime m_lastPushTime;           // 最后推送时间
};

#endif // PSYCHOTIPSMANAGER_H