#include "contactspage.h"
#include "ui_contactspage.h"

#include <QDateTime>
#include <QDialog>
#include <QHBoxLayout>
#include <QIcon>
#include <QInputDialog>
#include <QLabel>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPixmap>
#include <QVBoxLayout>
#include <cstring>

#include "frontclient.h"

// 鐙珛鑱婂ぉ绐楀彛锛堝叏灞忥級锛屾敮鎸佸ご鍍忓拰宸﹀彸姘旀场
class ChatDialog : public QDialog
{
public:
    ChatDialog(const QString& friendId,
               const QString& friendName,
               int friendAvatar,
               int selfAvatar,
               const SessionData& session,
               QWidget* parent = nullptr)
        : QDialog(parent),
          m_friendId(friendId),
          m_friendAvatar(friendAvatar),
          m_selfAvatar(selfAvatar),
          m_session(session)
    {
        setWindowTitle(tr("Chat with %1").arg(friendName));
        QSize targetSize = parent ? parent->size() : QSize(420, 720);
        resize(targetSize);
        setModal(true);
        // 宓屽叆寮忚鐩栵細鏃犺竟妗嗐€佸悓鐖跺ぇ灏忥紝鍙殢鏃跺叧闂繑鍥?        setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        if (parent) {
            QPoint gp = parent->mapToGlobal(QPoint(0, 0));
            move(gp);
        }

        auto *layout = new QVBoxLayout(this);

        // 椤堕儴鏍忥細杩斿洖 + 鏍囬
        auto *topBar = new QHBoxLayout();
        QPushButton *btnBack = new QPushButton(tr("Back"), this);
        btnBack->setFixedHeight(32);
        btnBack->setStyleSheet("QPushButton { border: none; font-size:14px; }");
        QLabel *lblTitle = new QLabel(windowTitle(), this);
        lblTitle->setStyleSheet("QLabel { font-weight: bold; font-size: 16px; }");
        topBar->addWidget(btnBack);
        topBar->addStretch();
        topBar->addWidget(lblTitle);
        topBar->addStretch();
        layout->addLayout(topBar);

        m_list = new QListWidget(this);
        m_list->setSpacing(8);
        layout->addWidget(m_list, 1);

        auto *inputLay = new QHBoxLayout();
        m_edit = new QLineEdit(this);
        m_edit->setPlaceholderText(tr("Type a message..."));
        m_btnSend = new QPushButton(tr("Send"), this);
        inputLay->addWidget(m_edit, 1);
        inputLay->addWidget(m_btnSend);
        layout->addLayout(inputLay);

        connect(m_btnSend, &QPushButton::clicked, this, &ChatDialog::sendMsg);
        connect(btnBack, &QPushButton::clicked, this, &ChatDialog::close);
        loadConversation();
    }

    void refreshConversation() { loadConversation(); }

private:
    QLabel* buildAvatar(int index)
    {
        auto *lbl = new QLabel();
        lbl->setFixedSize(40, 40);
        // qrc 璺緞涓?/head_portrait/head_portrait/X.png
        QPixmap pix(QString(":/head_portrait/head_portrait/%1.png").arg(index));
        if (!pix.isNull()) {
            lbl->setPixmap(pix.scaled(lbl->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            lbl->setStyleSheet("");
        }
        // 鑻ヨ祫婧愪笉瀛樺湪锛岀敤鍦嗗舰搴曡壊+鏁板瓧鍗犱綅
        if (pix.isNull()) {
            lbl->setText(QString::number(index));
            lbl->setAlignment(Qt::AlignCenter);
            lbl->setStyleSheet("QLabel { background:#d8d8d8; border-radius:20px; color:#333; font-weight:bold; }");
        }
        return lbl;
    }

    QLabel* buildBubble(const QString& text, bool isSelf)
    {
        auto *label = new QLabel(text);
        label->setWordWrap(true);
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        label->setStyleSheet(isSelf
                             ? "QLabel { background:#07c160; color:white; border-radius:12px; padding:10px; }"
                             : "QLabel { background:#f2f3f5; color:#111; border-radius:12px; padding:10px; }");
        return label;
    }

    void addMessageWidget(const MessageRecord& m)
    {
        bool isSelf = (m.direction == 0);
        QWidget *w = new QWidget();
        auto *h = new QHBoxLayout(w);
        h->setContentsMargins(8, 4, 8, 4);
        h->setSpacing(8);

        // 姘旀场鍐呭
        auto *bubble = buildBubble(m.content, isSelf);
        bubble->setMaximumWidth(260);
        bubble->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

        // 澶村儚
        QLabel *avatar = buildAvatar(isSelf ? m_selfAvatar : m_friendAvatar);

        if (isSelf) {
            h->addStretch();
            h->addWidget(bubble);
            h->addWidget(avatar);
        } else {
            h->addWidget(avatar);
            h->addWidget(bubble);
            h->addStretch();
        }

        auto *item = new QListWidgetItem();
        QSize hint = w->sizeHint();
        hint.setHeight(qMax(hint.height(), 60));
        item->setSizeHint(hint);
        m_list->addItem(item);
        m_list->setItemWidget(item, w);
    }

    void loadConversation()
    {
        m_list->clear();
        auto msgs = LocalStore::instance().loadMessages(m_friendId);
        for (const auto& m : msgs) {
            addMessageWidget(m);
        }
        m_list->scrollToBottom();
    }

private slots:
    void sendMsg()
    {
        QString content = m_edit->text().trimmed();
        if (content.isEmpty()) return;
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        // 鏈湴钀藉湴
        LocalStore::instance().saveMessage(m_friendId, m_session.userId, content, now, 0);
        QByteArray body;
        MESSAGE_REQ req{};
        memcpy(req.sender_id, m_session.userId.toUtf8().constData(), qMin<int>(static_cast<int>(sizeof(req.sender_id))-1, m_session.userId.size()));
        memcpy(req.receiver_id, m_friendId.toUtf8().constData(), qMin<int>(static_cast<int>(sizeof(req.receiver_id))-1, m_friendId.size()));
        req.content_len = content.toUtf8().size();
        body.append(reinterpret_cast<const char*>(&req), sizeof(req));
        body.append(content.toUtf8());
        FrontClient::instance().sendSimple(MESSAGE, body);
        m_edit->clear();
        loadConversation();
    }

private:
    QString m_friendId;
    int m_friendAvatar = 0;
    int m_selfAvatar = 0;
    SessionData m_session;
    QListWidget *m_list{nullptr};
    QLineEdit *m_edit{nullptr};
    QPushButton *m_btnSend{nullptr};
};

ContactsPage::ContactsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContactsPage)
{
    ui->setupUi(this);

    connect(ui->listFriends, &QListWidget::itemSelectionChanged,
            this, &ContactsPage::onFriendSelected);
    connect(ui->editSearch, &QLineEdit::textChanged,
            this, &ContactsPage::onSearchTextChanged);
    connect(ui->btnAddFriend, &QPushButton::clicked,
            this, &ContactsPage::onAddFriend);
    connect(ui->listPending, &QListWidget::itemDoubleClicked,
            this, &ContactsPage::onPendingItemActivated);

    ui->listFriends->setIconSize(QSize(44, 44));
    ui->listFriends->setUniformItemSizes(false);
    ui->listFriends->setSpacing(6);

    // ✅ 删除轮询Timer，改用事件驱动
    // m_msgTimer = new QTimer(this);
    // m_msgTimer->setInterval(2000);
    // connect(m_msgTimer, &QTimer::timeout, this, &ContactsPage::fetchMessagesFromServer);
    // m_msgTimer->start();
    
    // ✅ 连接FrontClient的消息推送信号
    connect(&FrontClient::instance(), &FrontClient::messageReceived,
            this, &ContactsPage::onMessagePushed);
}

ContactsPage::~ContactsPage()
{
    delete ui;
}

void ContactsPage::setSession(const SessionData& session)
{
    m_session = session;
    if (!m_session.userId.isEmpty()) {
        fetchFriendsFromServer();
        fetchPendingRequests();
        
        // ✅ 启用消息推送（替代轮询）
        FrontClient::instance().setUserId(m_session.userId);
        FrontClient::instance().enableMessagePush(true);
    }
}

void ContactsPage::refreshData()
{
    fetchFriendsFromServer();
    fetchPendingRequests();
    fetchMessagesFromServer();
}

void ContactsPage::buildMockFriends()
{
    m_allFriends.clear();
    ContactItem a { "f001", "Demo Friend A", 1, true, 2 };
    ContactItem b { "f002", "Demo Friend B", 2, false, 0 };
    ContactItem c { "f003", "AI Helper", 8, true, 5 };
    m_allFriends << a << b << c;
}

void ContactsPage::fetchFriendsFromServer()
{
    m_allFriends.clear();
    if (m_session.userId.isEmpty()) {
        applyFilter();
        return;
    }
    HEAD respHead{};
    QByteArray body;
    if (!m_session.userId.isEmpty()) {
        char uid[32] = {0};
        QByteArray u = m_session.userId.toUtf8();
        memcpy(uid, u.constData(), qMin<int>(static_cast<int>(sizeof(uid)) - 1, u.size()));
        body.append(uid, sizeof(uid));
    }
    QByteArray resp = FrontClient::instance().sendAndReceive(FRIEND_LIST, body, &respHead);
    if (resp.size() >= static_cast<int>(sizeof(FRIEND_LIST_RESP))) {
        auto *respBody = reinterpret_cast<const FRIEND_LIST_RESP*>(resp.constData());
        if (respBody->status_code == 0 && resp.size() >= static_cast<int>(sizeof(FRIEND_LIST_RESP) + respBody->friend_count * sizeof(FRIEND_ITEM))) {
            const FRIEND_ITEM* items = reinterpret_cast<const FRIEND_ITEM*>(resp.constData() + sizeof(FRIEND_LIST_RESP));
            for (int i = 0; i < respBody->friend_count; ++i) {
                ContactItem f;
                f.id = QString::fromUtf8(items[i].friend_id);
                f.name = QString::fromUtf8(items[i].friend_name);
                f.avatarIndex = static_cast<int>(items[i].profile_picture_index);
                f.online = true;
                f.unread = 0;
                m_allFriends.push_back(f);
            }
        }
    }
    applyFilter();
}

void ContactsPage::fetchPendingRequests()
{
    ui->listPending->clear();
    if (m_session.userId.isEmpty()) return;
    QByteArray body;
    char uid[32] = {0};
    QByteArray u = m_session.userId.toUtf8();
    memcpy(uid, u.constData(), qMin<int>(static_cast<int>(sizeof(uid)) - 1, u.size()));
    body.append(uid, sizeof(uid));

    HEAD respHead{};
    QByteArray resp = FrontClient::instance().sendAndReceive(FRIEND_PENDING, body, &respHead);
    if (resp.size() < static_cast<int>(sizeof(FRIEND_PENDING_RESP))) return;
    auto *respBody = reinterpret_cast<const FRIEND_PENDING_RESP*>(resp.constData());
    if (respBody->status_code != 0) return;
    int expect = respBody->request_count;
    int need = static_cast<int>(sizeof(FRIEND_PENDING_RESP) + expect * sizeof(FRIEND_REQUEST_ITEM));
    if (resp.size() < need) return;
    const auto* items = reinterpret_cast<const FRIEND_REQUEST_ITEM*>(resp.constData() + sizeof(FRIEND_PENDING_RESP));
    for (int i = 0; i < expect; ++i) {
        QString from = QString::fromUtf8(items[i].from_user);
        QString name = QString::fromUtf8(items[i].from_name);
        QString hello = QString::fromUtf8(items[i].hello);
        QString display = name.isEmpty() ? from : name;
        QString text = QString("%1 (%2)").arg(display, from);
        auto *it = new QListWidgetItem(text);
        it->setData(Qt::UserRole, from);
        it->setData(Qt::UserRole + 1, hello);
        it->setToolTip(hello);
        ui->listPending->addItem(it);
    }
}

void ContactsPage::fetchMessagesFromServer()
{
    if (m_session.userId.isEmpty()) return;
    QByteArray body;
    char uid[32] = {0};
    QByteArray u = m_session.userId.toUtf8();
    memcpy(uid, u.constData(), qMin<int>(static_cast<int>(sizeof(uid)) - 1, u.size()));
    body.append(uid, sizeof(uid));

    HEAD respHead{};
    QByteArray resp = FrontClient::instance().sendAndReceive(MESSAGE_PULL, body, &respHead);
    if (resp.size() < static_cast<int>(sizeof(MESSAGE_LIST_RESP))) return;
    const auto* listResp = reinterpret_cast<const MESSAGE_LIST_RESP*>(resp.constData());
    if (listResp->status_code != 0) return;

    int offset = static_cast<int>(sizeof(MESSAGE_LIST_RESP));
    for (int i = 0; i < listResp->message_count; ++i) {
        if (offset + static_cast<int>(sizeof(MESSAGE_ITEM)) > resp.size()) break;
        const auto* item = reinterpret_cast<const MESSAGE_ITEM*>(resp.constData() + offset);
        offset += static_cast<int>(sizeof(MESSAGE_ITEM));
        int clen = item->content_len;
        if (clen < 0 || offset + clen > resp.size()) break;
        QByteArray content = resp.mid(offset, clen);
        offset += clen;

        QString sender = QString::fromUtf8(item->sender_id);
        QString text = QString::fromUtf8(content);
        qint64 ts = QDateTime::currentMSecsSinceEpoch();
        LocalStore::instance().saveMessage(sender, sender, text, ts, 1);
        
        // 消息已保存到本地数据库，ChatPage会通过信号自动刷新
    }
}

bool ContactsPage::acceptFriend(const QString& fromId)
{
    QByteArray body;
    FRIEND_APPLY_REQ req{};
    QByteArray from = fromId.toUtf8();
    QByteArray to = m_session.userId.toUtf8();
    memcpy(req.from_user, from.constData(), qMin<int>(static_cast<int>(sizeof(req.from_user)) - 1, from.size()));
    memcpy(req.to_user, to.constData(), qMin<int>(static_cast<int>(sizeof(req.to_user)) - 1, to.size()));
    QByteArray resp = FrontClient::instance().sendAndReceive(FRIEND_ACCEPT, QByteArray(reinterpret_cast<const char*>(&req), sizeof(req)));
    if (resp.size() >= static_cast<int>(sizeof(FRIEND_APPLY_RESP))) {
        auto *r = reinterpret_cast<const FRIEND_APPLY_RESP*>(resp.constData());
        return r->status_code == 0;
    }
    return false;
}

void ContactsPage::onPendingItemActivated(QListWidgetItem* item)
{
    if (!item) return;
    QString from = item->data(Qt::UserRole).toString();
    if (from.isEmpty()) return;
    if (acceptFriend(from)) {
        delete item;
        fetchFriendsFromServer();
        fetchPendingRequests();
    }
}

void ContactsPage::applyFilter()
{
    ui->listFriends->clear();
    const QString keyword = ui->editSearch->text().trimmed();
    for (const auto& f : m_allFriends) {
        if (!keyword.isEmpty() && !f.name.contains(keyword, Qt::CaseInsensitive)) continue;
        // 只显示好友名字,不显示online状态
        QListWidgetItem* item = new QListWidgetItem(f.name);
        item->setData(Qt::UserRole, f.id);
        item->setData(Qt::UserRole + 1, f.avatarIndex);
        item->setIcon(QIcon(QString(":/head_portrait/head_portrait/%1.png").arg(f.avatarIndex)));
        item->setSizeHint(QSize(80, 56));
        if (f.unread > 0) {
            item->setText(item->text() + QString(" (%1)").arg(f.unread));
        }
        ui->listFriends->addItem(item);
    }
}

void ContactsPage::onFriendSelected()
{
    auto items = ui->listFriends->selectedItems();
    if (items.isEmpty()) return;
    QListWidgetItem* it = items.first();
    m_currentFriendId = it->data(Qt::UserRole).toString();
    int avatarIdx = it->data(Qt::UserRole + 1).toInt();
    QString friendName = it->text().split(" [").first().trimmed();
    openChatWindow(m_currentFriendId, friendName, avatarIdx);
}

void ContactsPage::loadConversation(const QString& friendId)
{
    QVector<MessageRecord> msgs = LocalStore::instance().loadMessages(friendId);
    Q_UNUSED(msgs);
}

void ContactsPage::renderMessages(const QVector<MessageRecord>& msgs)
{
    Q_UNUSED(msgs); // 采用独立聊天窗口，这里已不渲染列表
}

void ContactsPage::onSearchTextChanged(const QString& text)
{
    Q_UNUSED(text);
    applyFilter();
}

void ContactsPage::onSendMessage()
{
    // 宸叉敼鐢ㄧ嫭绔嬭亰澶╃獥鍙ｏ紝杩欓噷鐣欑┖瀹炵幇
}

void ContactsPage::onAddFriend()
{
    if (m_session.userId.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请先登录后再添加好友"));
        return;
    }
    // 只需要输入一个ID/手机号
    bool ok = false;
    QString friendId = QInputDialog::getText(this, tr("添加好友"), tr("输入好友ID/手机号"), QLineEdit::Normal, "", &ok);
    if (!ok || friendId.trimmed().isEmpty()) return;
    QString friendName = friendId.trimmed();
    if (friendName == m_session.userId) {
        QMessageBox::information(this, tr("提示"), tr("不能添加自己为好友哦~"));
        return;
    }
    // 发送好友申请
    QByteArray body;
    FRIEND_APPLY_REQ req{};
    QByteArray fid = friendId.trimmed().toUtf8();
    QByteArray hello = QString("Hi, 我是 %1").arg(m_session.userName).toUtf8();
    memcpy(req.from_user, m_session.userId.toUtf8().constData(), qMin<int>(static_cast<int>(sizeof(req.from_user))-1, m_session.userId.size()));
    memcpy(req.to_user, fid.constData(), qMin<int>(static_cast<int>(sizeof(req.to_user))-1, fid.size()));
    memcpy(req.hello, hello.constData(), qMin<int>(static_cast<int>(sizeof(req.hello))-1, hello.size()));
    body.append(reinterpret_cast<const char*>(&req), sizeof(req));
    QByteArray resp = FrontClient::instance().sendAndReceive(FRIEND_APPLY, body);
    if (resp.size() >= static_cast<int>(sizeof(FRIEND_APPLY_RESP))) {
        auto *r = reinterpret_cast<const FRIEND_APPLY_RESP*>(resp.constData());
        if (r->status_code == 0) {
            QMessageBox::information(this, tr("提示"), tr("好友申请已发送"));
            fetchFriendsFromServer();
        } else {
            QMessageBox::warning(this, tr("提示"), tr("好友申请发送失败"));
        }
    } else {
        QMessageBox::warning(this, tr("提示"), tr("网络异常，稍后再试"));
    }
}
void ContactsPage::onFilterAll()      { m_activeFilter = "all"; applyFilter(); }
void ContactsPage::onFilterFriends()  { m_activeFilter = "friends"; applyFilter(); }
void ContactsPage::onFilterPartners() { m_activeFilter = "partners"; applyFilter(); }
void ContactsPage::onFilterAI()       { m_activeFilter = "ai"; applyFilter(); }

int ContactsPage::getFriendAvatar(const QString& friendId) const
{
    for (const auto& f : m_allFriends) {
        if (f.id == friendId) return f.avatarIndex;
    }
    return 0;
}

void ContactsPage::openChatWindow(const QString& friendId, const QString& friendName, int friendAvatar)
{
    fetchMessagesFromServer();
    
    // 发送信号给MainWindow，让它切换到ChatPage
    int selfAvatar = m_session.avatarIndex;
    emit showChatPage(friendId, friendName, friendAvatar, selfAvatar);
}

// 新增：处理服务器推送的消息（事件驱动）
void ContactsPage::onMessagePushed(const QString& senderId, const QString& content, qint64 timestamp)
{
    qDebug() << "[ContactsPage] Message pushed from:" << senderId << "content:" << content;
    
    // 保存到本地数据库
    LocalStore::instance().saveMessage(senderId, senderId, content, timestamp, 1);
    
    // 注意：聊天页面的刷新由ChatPage自己处理（通过连接FrontClient的messageReceived信号）
    // 这里只需要保存消息到本地数据库即可
    
    // 更新好友列表的未读计数（可选实现）
    // updateUnreadCount(senderId);
}
