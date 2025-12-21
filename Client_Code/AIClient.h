#ifndef AICLIENT_H
#define AICLIENT_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>

/**
 * @brief AI对话客户端类
 * 
 * 用于通过HTTP API与AI模型进行对话交互
 * 支持流式和非流式响应
 */
class AIClient : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit AIClient(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~AIClient();

    /**
     * @brief 设置API配置
     * @param apiKey API密钥
     * @param apiUrl API的URL地址
     * @param modelId 模型ID
     */
    void setConfig(const QString &apiKey, const QString &apiUrl, const QString &modelId);

    /**
     * @brief 发送消息给AI
     * @param message 用户消息内容
     * @param systemPrompt 系统提示词(可选)
     * @param temperature 温度参数(0.0-1.0,控制随机性)
     * @param maxTokens 最大token数
     */
    void sendMessage(const QString &message, 
                    const QString &systemPrompt = QString(),
                    double temperature = 0.7,
                    int maxTokens = 2000);

    /**
     * @brief 发送流式消息给AI
     * @param message 用户消息内容
     * @param systemPrompt 系统提示词(可选)
     * @param temperature 温度参数
     * @param maxTokens 最大token数
     */
    void sendStreamMessage(const QString &message,
                          const QString &systemPrompt = QString(),
                          double temperature = 0.7,
                          int maxTokens = 2000);

    /**
     * @brief 取消当前请求
     */
    void cancelRequest();

    /**
     * @brief 清除对话历史
     */
    void clearHistory();

    /**
     * @brief 设置是否保留对话历史
     * @param keep true保留,false不保留
     */
    void setKeepHistory(bool keep);

signals:
    /**
     * @brief 收到完整响应信号
     * @param response AI的响应内容
     */
    void responseReceived(const QString &response);

    /**
     * @brief 收到流式响应片段信号
     * @param chunk 响应片段
     */
    void streamChunkReceived(const QString &chunk);

    /**
     * @brief 流式响应完成信号
     */
    void streamFinished();

    /**
     * @brief 发生错误信号
     * @param error 错误信息
     */
    void errorOccurred(const QString &error);

    /**
     * @brief 请求开始信号
     */
    void requestStarted();

private slots:
    /**
     * @brief 处理网络响应完成
     */
    void onReplyFinished();

    /**
     * @brief 处理流式数据接收
     */
    void onReadyRead();

    /**
     * @brief 处理网络错误
     * @param error 错误类型
     */
    void onNetworkError(QNetworkReply::NetworkError error);

private:
    /**
     * @brief 构建请求JSON
     * @param message 用户消息
     * @param systemPrompt 系统提示词
     * @param temperature 温度参数
     * @param maxTokens 最大token数
     * @param stream 是否流式
     * @return JSON对象
     */
    QJsonObject buildRequestJson(const QString &message,
                                 const QString &systemPrompt,
                                 double temperature,
                                 int maxTokens,
                                 bool stream);

    /**
     * @brief 发送HTTP请求
     * @param jsonData 请求数据
     * @param stream 是否流式
     */
    void sendRequest(const QJsonObject &jsonData, bool stream);

    /**
     * @brief 解析非流式响应
     * @param jsonData 响应JSON数据
     * @return 解析出的文本内容
     */
    QString parseResponse(const QJsonObject &jsonData);

    /**
     * @brief 解析流式响应行
     * @param line SSE格式的数据行
     * @return 解析出的文本片段
     */
    QString parseStreamLine(const QString &line);

private:
    QNetworkAccessManager *m_networkManager;  // 网络管理器
    QNetworkReply *m_currentReply;            // 当前请求
    
    QString m_apiKey;                         // API密钥
    QString m_apiUrl;                         // API URL
    QString m_modelId;                        // 模型ID
    
    QJsonArray m_conversationHistory;         // 对话历史
    bool m_keepHistory;                       // 是否保留历史
    
    QString m_streamBuffer;                   // 流式数据缓冲区
};

