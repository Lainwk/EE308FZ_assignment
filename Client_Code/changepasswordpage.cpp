#include "changepasswordpage.h"
#include "ui_changepasswordpage.h"
#include "frontclient.h"
#include <QMessageBox>
#include <QDebug>
#include <QCryptographicHash>

ChangePasswordPage::ChangePasswordPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChangePasswordPage)
{
    ui->setupUi(this);

    // 连接信号槽
    connect(ui->btnBack, &QPushButton::clicked, this, &ChangePasswordPage::onBackClicked);
    connect(ui->btnSubmit, &QPushButton::clicked, this, &ChangePasswordPage::onSubmitClicked);
}

ChangePasswordPage::~ChangePasswordPage()
{
    delete ui;
}

void ChangePasswordPage::setUserId(const QString& userId)
{
    m_userId = userId;
    qDebug() << "[ChangePasswordPage] User ID set:" << userId;
}

void ChangePasswordPage::onBackClicked()
{
    clearInputs();
    emit backToSettings();
}

void ChangePasswordPage::onSubmitClicked()
{
    if (!validateInputs()) {
        return;
    }

    QString oldPassword = ui->editOldPassword->text();
    QString newPassword = ui->editNewPassword->text();

    // MD5加密密码
    QByteArray oldPwdHash = QCryptographicHash::hash(oldPassword.toUtf8(), QCryptographicHash::Md5);
    QByteArray newPwdHash = QCryptographicHash::hash(newPassword.toUtf8(), QCryptographicHash::Md5);
    
    QString oldPwdMd5 = oldPwdHash.toHex();
    QString newPwdMd5 = newPwdHash.toHex();

    qDebug() << "[ChangePasswordPage] Changing password for user:" << m_userId;

    // 构造请求
    CHANGE_PASSWORD_REQ req = {};
    strncpy(req.user_id, m_userId.toUtf8().constData(), sizeof(req.user_id) - 1);
    strncpy(req.old_password, oldPwdMd5.toUtf8().constData(), sizeof(req.old_password) - 1);
    strncpy(req.new_password, newPwdMd5.toUtf8().constData(), sizeof(req.new_password) - 1);

    QByteArray reqBody(reinterpret_cast<const char*>(&req), sizeof(req));

    // 发送请求
    HEAD respHead = {};
    QByteArray respBody = FrontClient::instance().sendAndReceive(CHANGE_PASSWORD, reqBody, &respHead);

    if (respBody.isEmpty() || respBody.size() < static_cast<int>(sizeof(CHANGE_PASSWORD_RESP))) {
        QMessageBox::warning(this, "错误", "网络请求失败，请检查网络连接");
        return;
    }

    // 解析响应
    CHANGE_PASSWORD_RESP* resp = reinterpret_cast<CHANGE_PASSWORD_RESP*>(respBody.data());

    qDebug() << "[ChangePasswordPage] Response status:" << resp->status_code;

    if (resp->status_code == 0) {
        QMessageBox::information(this, "成功", "密码修改成功！");
        clearInputs();
        emit passwordChanged();
        emit backToSettings();
    } else if (resp->status_code == 1) {
        QMessageBox::warning(this, "错误", "原密码错误，请重新输入");
        ui->editOldPassword->clear();
        ui->editOldPassword->setFocus();
    } else {
        QString errorMsg = QString::fromUtf8(resp->message, sizeof(resp->message)).trimmed();
        if (errorMsg.isEmpty()) {
            errorMsg = "密码修改失败，请稍后重试";
        }
        QMessageBox::warning(this, "错误", errorMsg);
    }
}

bool ChangePasswordPage::validateInputs()
{
    QString oldPassword = ui->editOldPassword->text();
    QString newPassword = ui->editNewPassword->text();
    QString confirmPassword = ui->editConfirmPassword->text();

    // 检查是否为空
    if (oldPassword.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入原密码");
        ui->editOldPassword->setFocus();
        return false;
    }

    if (newPassword.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入新密码");
        ui->editNewPassword->setFocus();
        return false;
    }

    if (confirmPassword.isEmpty()) {
        QMessageBox::warning(this, "提示", "请再次输入新密码");
        ui->editConfirmPassword->setFocus();
        return false;
    }

    // 检查新密码长度
    if (newPassword.length() < 6) {
        QMessageBox::warning(this, "提示", "新密码长度至少为6位");
        ui->editNewPassword->setFocus();
        return false;
    }

    // 检查两次新密码是否一致
    if (newPassword != confirmPassword) {
        QMessageBox::warning(this, "提示", "两次输入的新密码不一致，请重新输入");
        ui->editNewPassword->clear();
        ui->editConfirmPassword->clear();
        ui->editNewPassword->setFocus();
        return false;
    }

    // 检查新密码是否与旧密码相同
    if (oldPassword == newPassword) {
        QMessageBox::warning(this, "提示", "新密码不能与原密码相同");
        ui->editNewPassword->clear();
        ui->editConfirmPassword->clear();
        ui->editNewPassword->setFocus();
        return false;
    }

    return true;
}

void ChangePasswordPage::clearInputs()
{
    ui->editOldPassword->clear();
    ui->editNewPassword->clear();
    ui->editConfirmPassword->clear();
}