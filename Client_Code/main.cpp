#include <QApplication>
#include "authwindow.h"
#include "localstore.h"
#include "frontclient.h"
#include "mainwindow.h"
#include <QCoreApplication>
#include <cstring>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LocalStore::instance().init();
    // 自动连接前置服务器（默认 127.0.0.1:10001，可按需调整）
    FrontClient::instance().connectToServer("106.53.21.199", 10001);

    if (LocalStore::instance().hasSession()) {
        SessionData s = LocalStore::instance().loadSession();
        // 尝试自动登录（使用存储的手机号/密码，验证码留空）
        LOGIN_REQ req = {};
        QByteArray body(sizeof(LOGIN_REQ), 0);
        QByteArray phone = s.phone.toUtf8();
        QByteArray pwd = s.pwd.toUtf8();
        std::memcpy(req.user_phone, phone.constData(), std::min(phone.size(), static_cast<int>(sizeof(req.user_phone) - 1)));
        std::memcpy(req.user_pwd, pwd.constData(), std::min(pwd.size(), static_cast<int>(sizeof(req.user_pwd) - 1)));
        std::memcpy(body.data(), &req, sizeof(LOGIN_REQ));

        HEAD head = {};
        QByteArray resp = FrontClient::instance().sendAndReceive(LOGIN, body, &head);
        if (resp.size() >= static_cast<int>(sizeof(LOGIN_RESP))) {
            LOGIN_RESP resp_body = {};
            std::memcpy(&resp_body, resp.constData(), sizeof(LOGIN_RESP));
            if (resp_body.status_code == 0) {
                SessionData updated = s;
                updated.userId = QString::fromLatin1(resp_body.user_id);
                updated.userName = QString::fromLatin1(resp_body.user_name);
                updated.avatarIndex = resp_body.profile_picture_index;
                LocalStore::instance().saveSession(updated);

                MainWindow* w = new MainWindow();
                w->setUserInfo(updated.userId, updated.userName);
                w->show();
                return a.exec();
            }
        }
    }

    AuthWindow* w = new AuthWindow();
    w->show();
    return a.exec();
}
