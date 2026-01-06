#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "localstore.h"

namespace Ui {
class ChatPage;
}

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage();

    void setSession(const SessionData& session);
    void setChatInfo(const QString& friendId, const QString& friendName, int friendAvatar, int selfAvatar);
    void refreshConversation();

signals:
    void backRequested();

private slots:
    void onSendClicked();
    void onBackClicked();
    void onMessageReceived(const QString& senderId, const QString& content, qint64 timestamp);

private:
    void loadConversation();
    void addMessageWidget(const MessageRecord& m);
    QLabel* buildAvatar(int index);
    QLabel* buildBubble(const QString& text, bool isSelf);

    Ui::ChatPage *ui;
    SessionData m_session;
    QString m_friendId;
    QString m_friendName;
    int m_friendAvatar = 0;
    int m_selfAvatar = 0;
};

#endif // CHATPAGE_H