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
#include "reminderservice.h"

AuthWindow::AuthWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::AuthWindow),
      isPasswordMode(true)
{
    qDebug() << "[AuthWindow] Constructor called";
    ui->setupUi(this);
    qDebug() << "[AuthWindow] UI setup completed";
    
#ifdef Q_OS_ANDROID
    // Android平台：设置标题和logo的字体样式
    ui->labelTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: #000000; padding: 20px;");
    
    // 设置"记住密码"复选框的文字样式
    ui->checkRemember->setStyleSheet("QCheckBox { color: #000000; font-size: 14px; }");
    
    qDebug() << "[AuthWindow] Android: Title and checkbox styled";
#endif
    
#ifdef Q_OS_ANDROID
    // Android平台安全地移除MenuBar和StatusBar
    try {
        // 使用更安全的方式隐藏menubar和statusbar
        QMenuBar* menuBar = this->menuBar();
        if (menuBar) {
            menuBar->setVisible(false);
            menuBar->setMaximumHeight(0);
            menuBar->setFixedHeight(0);
        }
        
        QStatusBar* statusBar = this->statusBar();
        if (statusBar) {
            statusBar->setVisible(false);
            statusBar->setMaximumHeight(0);
            statusBar->setFixedHeight(0);
        }
        
        // 设置窗口属性，防止黑框和抖动
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground, false);
        setAttribute(Qt::WA_NoSystemBackground, false);
        
        // 设置白色背景
        setStyleSheet("QMainWindow { background-color: white; border: none; }");
        if (ui->centralwidget) {
            ui->centralwidget->setStyleSheet("QWidget { background-color: white; border: none; }");
        }
        
        // 修复输入框在Android上的显示问题
        // 确保输入框有足够的高度和正确的属性
        QList<QLineEdit*> lineEdits = findChildren<QLineEdit*>();
        for (QLineEdit* edit : lineEdits) {
            edit->setMinimumHeight(45);  // 增加最小高度
            edit->setAttribute(Qt::WA_InputMethodEnabled, true);  // 启用输入法
            edit->setFocusPolicy(Qt::StrongFocus);  // 强焦点策略
            // 添加边框和样式
            edit->setStyleSheet(
                "QLineEdit { "
                "font-size: 14px; "
                "color: #000000; "
                "background-color: #FFFFFF; "
                "border: 1px solid #E0E0E0; "
                "border-radius: 8px; "
                "padding: 8px; "
                "}"
                "QLineEdit:focus { "
                "border: 2px solid #07C160; "
                "}"
            );
        }
        
        qDebug() << "[AuthWindow] Android platform initialized, MenuBar/StatusBar hidden";
    } catch (...) {
        qDebug() << "[AuthWindow] WARNING: Exception during Android UI setup, continuing...";
    }
#endif
    
    setLoginMode();
    updateLoginModeUI();  // 初始化登录模式UI

    loadSavedCredentials();  // 加载保存的账号密码
    setWindowIcon(QIcon(":/image/logo.png"));
    
    qDebug() << "[AuthWindow] Constructor finished";
}

AuthWindow::~AuthWindow()
{
    delete ui;
}

// Android平台UI强制刷新方法(必须在窗口显示后调用)
void AuthWindow::forceRefreshUI()
{
#ifdef Q_OS_ANDROID
    qDebug() << "[AuthWindow] forceRefreshUI() called";
    
    try {
        // 1. 强制显示centralWidget
        if (ui->centralwidget) {
            ui->centralwidget->setVisible(true);
            ui->centralwidget->raise();
            ui->centralwidget->update();
            ui->centralwidget->repaint();
        }
        
        // 2. 强制显示stackedWidget
        if (ui->stackedWidget) {
            ui->stackedWidget->setVisible(true);
            ui->stackedWidget->raise();
            ui->stackedWidget->update();
            ui->stackedWidget->repaint();
        }
        
        // 3. 强制显示当前页面
        QWidget* currentPage = ui->stackedWidget->currentWidget();
        if (currentPage) {
            currentPage->setVisible(true);
            currentPage->raise();
            currentPage->update();
            currentPage->repaint();
        }
        
        // 4. 激活布局系统
        if (layout()) {
            layout()->activate();
            layout()->update();
        }
        if (ui->centralwidget && ui->centralwidget->layout()) {
            ui->centralwidget->layout()->activate();
            ui->centralwidget->layout()->update();
        }
        
        // 5. 强制窗口重绘
        update();
        repaint();
        
        // 6. 调试输出当前状态
        qDebug() << "[AuthWindow] Android: centralWidget visible:" << ui->centralwidget->isVisible()
                 << "size:" << ui->centralwidget->size();
        qDebug() << "[AuthWindow] Android: stackedWidget visible:" << ui->stackedWidget->isVisible()
                 << "currentIndex:" << ui->stackedWidget->currentIndex()
                 << "size:" << ui->stackedWidget->size();
        if (currentPage) {
            qDebug() << "[AuthWindow] Android: currentPage visible:" << currentPage->isVisible()
                     << "size:" << currentPage->size();
        }
        
        qDebug() << "[AuthWindow] forceRefreshUI() completed";
    } catch (...) {
        qDebug() << "[AuthWindow] WARNING: Exception in forceRefreshUI(), continuing...";
    }
#endif
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
    std::memcpy(req.user_phone, phoneBytes.constData(), std::min(static_cast<qsizetype>(sizeof(req.user_phone) - 1), phoneBytes.size()));
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
        
        std::memcpy(req.user_phone, phone.constData(), std::min(static_cast<qsizetype>(sizeof(req.user_phone) - 1), phone.size()));
        std::memcpy(req.user_pwd, pwd.constData(), std::min(static_cast<qsizetype>(sizeof(req.user_pwd) - 1), pwd.size()));
        // code字段留空
    } else {
        // 验证码登录模式：只发送验证码，密码字段留空
        QByteArray code = ui->editCodeLogin->text().toUtf8();
        qDebug() << "[AuthWindow] SMS code login mode, code:" << code;
        
        std::memcpy(req.user_phone, phone.constData(), std::min(static_cast<qsizetype>(sizeof(req.user_phone) - 1), phone.size()));
        std::memcpy(req.code, code.constData(), std::min(static_cast<qsizetype>(sizeof(req.code) - 1), code.size()));
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
    std::memcpy(req.user_phone, phone.constData(), std::min(static_cast<qsizetype>(sizeof(req.user_phone) - 1), phone.size()));
    std::memcpy(req.user_name, name.constData(), std::min(static_cast<qsizetype>(sizeof(req.user_name) - 1), name.size()));
    std::memcpy(req.user_pwd, pwd.constData(), std::min(static_cast<qsizetype>(sizeof(req.user_pwd) - 1), pwd.size()));
    std::memcpy(req.code, code.constData(), std::min(static_cast<qsizetype>(sizeof(req.code) - 1), code.size()));
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
        
        // 设置当前用户ID，切换到该用户的数据库
        LocalStore::instance().setCurrentUserId(s.userId);
        qDebug() << "[AuthWindow] Set current user ID:" << s.userId;
        
        // 启动消息提醒服务
        ReminderService::instance().start(s.userId);
        qDebug() << "[AuthWindow] Started ReminderService for user:" << s.userId;

        auto *mainWin = new MainWindow();
        // 设置用户信息
        QString userId = QString::fromUtf8(resp_body.user_id);
        QString userName = QString::fromUtf8(resp_body.user_name);
        mainWin->setUserInfo(userId, userName);
        
#ifdef Q_OS_ANDROID
        // Android上全屏显示
        qDebug() << "[AuthWindow] Showing MainWindow on Android";
        mainWin->showMaximized();
        mainWin->showFullScreen();
        mainWin->raise();
        mainWin->activateWindow();
#else
        mainWin->show();
#endif
        
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
