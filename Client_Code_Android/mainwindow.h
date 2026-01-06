#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <QEvent>
#include <QMouseEvent>

class HomePage;
class ContactsPage;
class ChannelsPage;
class SettingsPage;
class ChangePasswordPage;
class ChatPage;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    // 设置用户信息
    void setUserInfo(const QString& userId, const QString& userName);

private slots:
    void on_btnHome_clicked();
    void on_btnContacts_clicked();
    void on_btnChannels_clicked();
    void on_btnMe_clicked();
    void onShowChangePasswordPage();
    void onShowChatPage(const QString& friendId, const QString& friendName, int friendAvatar, int selfAvatar);
    void onBackToSettings();
    void onBackToContacts();
    void onLogout();

private:
    void setupPages();

    Ui::MainWindow *ui;
    HomePage *m_homePage;
    ContactsPage *m_contactsPage;
    ChannelsPage *m_channelsPage;
    SettingsPage *m_settingsPage;
    ChangePasswordPage *m_changePasswordPage;
    ChatPage *m_chatPage;

    // Drag support for frameless window
    bool m_dragging = false;
    QPoint m_dragStartPos;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // MAINWINDOW_H
