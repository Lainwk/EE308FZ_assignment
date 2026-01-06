#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QWidget>
#include <QStringList>
#include "localstore.h"
class QLabel;
class QPushButton;

namespace Ui {
class SettingsPage;
}

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(QWidget *parent = nullptr);
    ~SettingsPage();

    void setSession(const SessionData& session);
    void refreshSession();

signals:
    void showChangePasswordPage();
    void logoutRequested();

private slots:
    void onChooseAvatar();
    void onChangePasswordClicked();
    void onLogoutClicked();

private:
    void loadAvatarPaths();
    QString resolveAvatarDir() const;
    void updateAvatarDisplay();

    Ui::SettingsPage *ui;
    QLabel *m_labelAvatar{nullptr};
    QPushButton *m_btnChooseAvatar{nullptr};
    SessionData m_session;
    QStringList m_avatarPaths;
    int m_avatarIndex = 0;
};

#endif // SETTINGSPAGE_H
