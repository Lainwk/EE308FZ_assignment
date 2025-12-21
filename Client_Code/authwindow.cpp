#include "authwindow.h"
#include "ui_authwindow.h"

#include <QMessageBox>
#include <QRegularExpression>
#include <QStackedWidget>
#include <cstring>
#include <QDateTime>
#include <QDebug>
#include <QCryptographicHash>
#include <QSettings>

#include "frontclient.h"
#include "mainwindow.h"
#include "localstore.h"

AuthWindow::AuthWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::AuthWindow),
      isPasswordMode(true)
{
    ui->setupUi(this);
    setLoginMode();
    updateLoginModeUI();  // 初始化登录模式UI
    loadSavedCredentials();  // 加载保存的账号密码
    // 注意：连接已在main.cpp中完成，这里不需要再连接
    setWindowIcon(QIcon(":/image/logo.png"));
}

AuthWindow::~AuthWindow()
{
    delete ui;
}

void AuthWindow::setLoginMode()
{
    ui->stackedWidget->setCurrentWidget(ui->pageLogin);
    isPasswordMode = true;  // 切换到登录页面时重置为密码模式
    updateLoginModeUI();
}

void AuthWindow::setRegisterMode()
{
    ui->stackedWidget->setCurrentWidget(ui->pageRegister);
}

void AuthWindow::on_btnSwitchRegister_clicked()
{
    setRegisterMode();
}

void AuthWindow::on_btnSwitchLogin_clicked()
{
    setLoginMode();
}

void AuthWindow::on_btnSwitchLoginMode_clicked()
{
    isPasswordMode = !isPasswordMode;
    updateLoginModeUI();
}

void AuthWindow::updateLoginModeUI()
{
    if (isPasswordMode) {
        // 密码登录模式
        ui->btnSwitchLoginMode->setText("切换到验证码登录");
        ui->editPwdLogin->setVisible(true);
        ui->editPwdLogin->setEnabled(true);
        ui->editCodeLogin->setVisible(false);
        ui->editCodeLogin->setEnabled(false);
        ui->btnSendCodeLogin->setVisible(false);
        ui->btnSendCodeLogin->setEnabled(false);
        ui->editCodeLogin->clear();
    } else {
        // 验证码登录模式
        ui->btnSwitchLoginMode->setText("切换到密码登录");
        ui->editPwdLogin->setVisible(false);
        ui->editPwdLogin->setEnabled(false);
        ui->editCodeLogin->setVisible(true);
        ui->editCodeLogin->setEnabled(true);
        ui->btnSendCodeLogin->setVisible(true);
        ui->btnSendCodeLogin->setEnabled(true);
        ui->editPwdLogin->clear();
    }
}

QByteArray AuthWindow::buildSmsBody(const QString &phone) const
{
    SMS_CODE_REQ req = {};
    QByteArray body(sizeof(SMS_CODE_REQ), 0);
    QByteArray phoneBytes = phone.toUtf8();
    std::memcpy(req.user_phone, phoneBytes.constData(), std::min(phoneBytes.size(), static_cast<int>(sizeof(req.user_phone) - 1)));
    std::memcpy(body.data(), &req, sizeof(SMS_CODE_REQ));
    return body;
}

QByteArray AuthWindow::buildLoginBody() const
{
    LOGIN_REQ req = {};
    QByteArray body(sizeof(LOGIN_REQ), 0);
    QByteArray phone = ui->editPhoneLogin->text().toUtf8();
    
    if (isPasswordMode) {
        // 密码登录模式：只发送密码，验证码字段留空
        QString password = ui->editPwdLogin->text();
        QByteArray pwd = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex();
        qDebug() << "[AuthWindow] Password login mode, MD5:" << pwd;
        
        std::memcpy(req.user_phone, phone.constData(), std::min(phone.size(), static_cast<int>(sizeof(req.user_phone) - 1)));
        std::memcpy(req.user_pwd, pwd.constData(), std::min(pwd.size(), static_cast<int>(sizeof(req.user_pwd) - 1)));
        // code字段留空
    } else {
        // 验证码登录模式：只发送验证码，密码字段留空
        QByteArray code = ui->editCodeLogin->text().toUtf8();
        qDebug() << "[AuthWindow] SMS code login mode, code:" << code;
        
        std::memcpy(req.user_phone, phone.constData(), std::min(phone.size(), static_cast<int>(sizeof(req.user_phone) - 1)));
        std::memcpy(req.code, code.constData(), std::min(code.size(), static_cast<int>(sizeof(req.code) - 1)));
        // pwd字段留空
    }
    
    std::memcpy(body.data(), &req, sizeof(LOGIN_REQ));
    return body;
}

QByteArray AuthWindow::buildRegisterBody() const
{
    REGISTER_REQ req = {};
    QByteArray body(sizeof(REGISTER_REQ), 0);
    QByteArray phone = ui->editPhoneRegister->text().toUtf8();
    QByteArray name = ui->editNameRegister->text().toUtf8();
    
    // MD5加密密码
    QString password = ui->editPwdRegister->text();
    QByteArray pwd = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex();
    qDebug() << "[AuthWindow] Register password MD5:" << pwd;
    
    QByteArray code = ui->editCodeRegister->text().toUtf8();
    std::memcpy(req.user_phone, phone.constData(), std::min(phone.size(), static_cast<int>(sizeof(req.user_phone) - 1)));
    std::memcpy(req.user_name, name.constData(), std::min(name.size(), static_cast<int>(sizeof(req.user_name) - 1)));
    std::memcpy(req.user_pwd, pwd.constData(), std::min(pwd.size(), static_cast<int>(sizeof(req.user_pwd) - 1)));
    std::memcpy(req.code, code.constData(), std::min(code.size(), static_cast<int>(sizeof(req.code) - 1)));
    std::memcpy(body.data(), &req, sizeof(REGISTER_REQ));
    return body;
}

void AuthWindow::on_btnSendCodeLogin_clicked()
{
    QByteArray body = buildSmsBody(ui->editPhoneLogin->text());
    bool ok = FrontClient::instance().sendSimple(SMS_CODE, body);
    QMessageBox::information(this, tr("验证码"), ok ? tr("验证码已发送") : tr("发送失败，请重试"));
}

void AuthWindow::on_btnSendCodeRegister_clicked()
{
    QByteArray body = buildSmsBody(ui->editPhoneRegister->text());
    bool ok = FrontClient::instance().sendSimple(SMS_CODE, body);
    QMessageBox::information(this, tr("验证码"), ok ? tr("验证码已发送") : tr("发送失败，请重试"));
}

void AuthWindow::on_btnDoLogin_clicked()
{
    QByteArray body = buildLoginBody();
    HEAD head = {};
    QByteArray resp = FrontClient::instance().sendAndReceive(LOGIN, body, &head);
    if (resp.size() < static_cast<int>(sizeof(LOGIN_RESP))) {
        QMessageBox::warning(this, tr("登录失败"), tr("无法连接服务器或返回包无效。"));
        return;
    }
    LOGIN_RESP resp_body = {};
    std::memcpy(&resp_body, resp.constData(), sizeof(LOGIN_RESP));

    if (resp_body.status_code == 0) {
        // 登录成功,保存凭据(如果勾选了记住密码)
        if (isPasswordMode) {
            saveCredentials();
        }
        
        SessionData s;
        s.phone = ui->editPhoneLogin->text();
        s.pwd = ui->editPwdLogin->text();
        s.code = ui->editCodeLogin->text();
        s.userId = QString::fromLatin1(resp_body.user_id);
        s.userName = QString::fromLatin1(resp_body.user_name);
        s.avatarIndex = resp_body.profile_picture_index;
        LocalStore::instance().saveSession(s);

        auto *mainWin = new MainWindow();
        // 设置用户信息
        QString userId = QString::fromUtf8(resp_body.user_id);
        QString userName = QString::fromUtf8(resp_body.user_name);
        mainWin->setUserInfo(userId, userName);
        mainWin->show();
        this->close();
    } else if (resp_body.status_code == 1) {
        QMessageBox::warning(this, tr("登录失败"), tr("验证码错误"));
    } else if (resp_body.status_code == 4) {
        QMessageBox::warning(this, tr("登录失败"), tr("验证码过期"));
    } else {
        QMessageBox::warning(this, tr("登录失败"), tr("用户名或密码错误/用户已在线"));
    }
}

void AuthWindow::on_btnDoRegister_clicked()
{
    qDebug() << "[AuthWindow] Register button clicked";
    
    QByteArray body = buildRegisterBody();
    qDebug() << "[AuthWindow] Built register body, size=" << body.size();
    
    HEAD head = {};
    QByteArray resp = FrontClient::instance().sendAndReceive(REGISTER, body, &head);
    
    qDebug() << "[AuthWindow] Received response, size=" << resp.size()
             << "expected=" << sizeof(REGISTER_RESP);
    
    if (resp.size() < static_cast<int>(sizeof(REGISTER_RESP))) {
        QString errorMsg = QString("无法连接服务器或返回包无效。\n收到: %1 字节\n期望: %2 字节")
            .arg(resp.size())
            .arg(sizeof(REGISTER_RESP));
        qDebug() << "[AuthWindow] ERROR:" << errorMsg;
        QMessageBox::warning(this, tr("注册失败"), errorMsg);
        return;
    }
    
    REGISTER_RESP resp_body = {};
    std::memcpy(&resp_body, resp.constData(), sizeof(REGISTER_RESP));
    
    qDebug() << "[AuthWindow] Parsed response: status_code=" << resp_body.status_code;
    
    if (resp_body.status_code == 0) {
        qDebug() << "[AuthWindow] Registration SUCCESS";
        QMessageBox::information(this, tr("注册成功"), tr("请使用账号登录"));
        setLoginMode();
    } else if (resp_body.status_code == 1) {
        qDebug() << "[AuthWindow] Registration FAILED: wrong code";
        QMessageBox::warning(this, tr("注册失败"), tr("验证码错误"));
    } else if (resp_body.status_code == 3) {
        qDebug() << "[AuthWindow] Registration FAILED: code expired";
        QMessageBox::warning(this, tr("注册失败"), tr("验证码过期"));
    } else {
        qDebug() << "[AuthWindow] Registration FAILED: status_code=" << resp_body.status_code;
        QMessageBox::warning(this, tr("注册失败"),
            QString("手机号已注册或服务器错误 (错误码: %1)").arg(resp_body.status_code));
    }
}

// 记住密码功能实现
void AuthWindow::loadSavedCredentials()
{
    QSettings settings("MindCarrer", "AuthInfo");
    bool rememberMe = settings.value("rememberMe", false).toBool();
    
    if (rememberMe) {
        QString phone = settings.value("phone", "").toString();
        QString encryptedPwd = settings.value("password", "").toString();
        
        if (!phone.isEmpty() && !encryptedPwd.isEmpty()) {
            ui->editPhoneLogin->setText(phone);
            ui->editPwdLogin->setText(decryptPassword(encryptedPwd));
            ui->checkRemember->setChecked(true);
        }
    }
}

void AuthWindow::saveCredentials()
{
    QSettings settings("MindCarrer", "AuthInfo");
    
    if (ui->checkRemember->isChecked()) {
        QString phone = ui->editPhoneLogin->text();
        QString password = ui->editPwdLogin->text();
        QString encryptedPwd = encryptPassword(password);
        
        settings.setValue("rememberMe", true);
        settings.setValue("phone", phone);
        settings.setValue("password", encryptedPwd);
    } else {
        clearSavedCredentials();
    }
}

void AuthWindow::clearSavedCredentials()
{
    QSettings settings("MindCarrer", "AuthInfo");
    settings.setValue("rememberMe", false);
    settings.remove("phone");
    settings.remove("password");
}

QString AuthWindow::encryptPassword(const QString& password)
{
    // 简单的XOR加密
    QByteArray data = password.toUtf8();
    for (int i = 0; i < data.size(); ++i) {
        data[i] = data[i] ^ 0x5A;  // 使用0x5A作为密钥
    }
    return QString::fromLatin1(data.toBase64());
}

QString AuthWindow::decryptPassword(const QString& encrypted)
{
    // 简单的XOR解密
    QByteArray data = QByteArray::fromBase64(encrypted.toLatin1());
    for (int i = 0; i < data.size(); ++i) {
        data[i] = data[i] ^ 0x5A;  // 使用相同的密钥
    }
    return QString::fromUtf8(data);
}
