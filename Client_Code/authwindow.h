#pragma once

#include <QMainWindow>

namespace Ui { class AuthWindow; }

class AuthWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit AuthWindow(QWidget *parent = nullptr);
    ~AuthWindow();

private slots:
    void on_btnSwitchRegister_clicked();
    void on_btnSwitchLogin_clicked();
    void on_btnSwitchLoginMode_clicked();
    void on_btnSendCodeLogin_clicked();
    void on_btnSendCodeRegister_clicked();
    void on_btnDoLogin_clicked();
    void on_btnDoRegister_clicked();

private:
    void setLoginMode();
    void setRegisterMode();
    void updateLoginModeUI();
    QByteArray buildLoginBody() const;
    QByteArray buildRegisterBody() const;
    QByteArray buildSmsBody(const QString &phone) const;
    
    // 记住密码功能
    void loadSavedCredentials();
    void saveCredentials();
    void clearSavedCredentials();
    QString encryptPassword(const QString& password);
    QString decryptPassword(const QString& encrypted);

    Ui::AuthWindow *ui;
    bool isPasswordMode = true;  // true=密码登录, false=验证码登录
};
