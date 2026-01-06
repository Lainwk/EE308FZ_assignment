#ifndef CONTACTSPAGE_H
#define CONTACTSPAGE_H

#include <QWidget>
#include <QVector>
#include <QMap>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDialog>
#include <QTimer>
#include "localstore.h"

struct ContactItem {
    QString id;
    QString name;
    int avatarIndex = 0;
    bool online = false;
    int unread = 0;
    ContactItem() = default;
    ContactItem(const QString& i, const QString& n, int avatar, bool on, int unreadCnt)
        : id(i), name(n), avatarIndex(avatar), online(on), unread(unreadCnt) {}
};

namespace Ui {
class ContactsPage;
}

class ContactsPage : public QWidget
{
    Q_OBJECT

public:
    explicit ContactsPage(QWidget *parent = nullptr);
    ~ContactsPage();

    void setSession(const SessionData& session);
    void refreshData();

signals:
    void showChatPage(const QString& friendId, const QString& friendName, int friendAvatar, int selfAvatar);

private slots:
    void onFriendSelected();
    void onSearchTextChanged(const QString& text);
    void onSendMessage();
    void onAddFriend();
    void onFilterAll();
    void onFilterFriends();
    void onFilterPartners();
    void onFilterAI();
    void onPendingItemActivated(QListWidgetItem* item);
    
    // 新增：处理服务器推送的消息（事件驱动）
    void onMessagePushed(const QString& senderId, const QString& content, qint64 timestamp);

private:
    void buildMockFriends();
    void applyFilter();
    void loadConversation(const QString& friendId);
    void renderMessages(const QVector<MessageRecord>& msgs);
    QString currentConvId() const;
    int getFriendAvatar(const QString& friendId) const;
    void openChatWindow(const QString& friendId, const QString& friendName, int friendAvatar);
    void fetchFriendsFromServer();
    void fetchPendingRequests();
    void fetchMessagesFromServer();
    bool acceptFriend(const QString& fromId);

    Ui::ContactsPage *ui;
    SessionData m_session;
    QVector<ContactItem> m_allFriends;
    QString m_activeFilter = "all";
    QString m_currentFriendId;
    QTimer* m_msgTimer{nullptr};
};

#endif // CONTACTSPAGE_H
