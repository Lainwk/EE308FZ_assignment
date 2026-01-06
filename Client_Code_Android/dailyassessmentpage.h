#ifndef DAILYASSESSMENTPAGE_H
#define DAILYASSESSMENTPAGE_H

#include <QWidget>
#include <QVector>
#include <QButtonGroup>
#include "frontclient.h"

namespace Ui {
class DailyAssessmentPage;
}

// 问题数据结构
struct Question {
    int id;
    QString text;
    QStringList options;
    QStringList optionCodes;      // 选项代码
    QList<float> optionScores;    // 选项分数
    QString dimensionCode;        // 维度代码
};

class DailyAssessmentPage : public QWidget
{
    Q_OBJECT

public:
    explicit DailyAssessmentPage(QWidget *parent = nullptr);
    ~DailyAssessmentPage();

    // 设置用户ID并加载问卷
    void setUserId(const QString& userId);
    
    // 加载问卷数据
    bool loadQuestionnaire();

signals:
    // 评估完成信号
    void assessmentCompleted(float score, const QString& riskLevel, const QString& suggestion);
    
    // 返回信号
    void backRequested();

private slots:
    // 提交答案
    void onSubmitClicked();
    
    // 返回按钮
    void onBackClicked();
    
    // 选项被点击
    void onOptionClicked(int optionIndex);

private:
    // 提交评估数据到服务器
    void submitAssessment();
    
    Ui::DailyAssessmentPage *ui;
    
    QString m_userId;
    int m_questionnaireId;           // 问卷ID
    QString m_questionnaireType;     // 问卷代码
    QVector<Question> m_questions;
    QVector<int> m_answers;          // 存储用户选择的答案索引
    
    int m_currentQuestionIndex;      // 当前显示的题目索引
    QButtonGroup* m_currentButtonGroup;  // 当前题目的按钮组
    
    bool m_hasCompletedToday;        // 今日是否已完成标记
    
    // UI辅助方法
    void setupUI();
    void displayCurrentQuestion();   // 显示当前题目
    void clearCurrentQuestion();     // 清理当前题目显示
    void showNextQuestion();         // 显示下一题
    void showConfirmDialog();        // 显示确认提交对话框
    bool validateAnswers();
    void showMessage(const QString& title, const QString& message);
    
    // 本地完成标记管理
    void markTodayCompleted();
    bool checkTodayCompleted();
    QString getTodayKey();
};

#endif // DAILYASSESSMENTPAGE_H