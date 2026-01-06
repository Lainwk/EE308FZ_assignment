#include "chatpage.h"
#include "ui_chatpage.h"
#include "frontclient.h"

#include <QDateTime>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QPixmap>
#include <cstring>

ChatPage::ChatPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatPage)
{
    ui->setupUi(this);
    
    // 连接信号槽
    connect(ui->sendButton, &QPushButton::clicked, this, &ChatPage::onSendClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &ChatPage::onBackClicked);
    
    // 连接消息推送信号，当收到新消息时自动刷新
    connect(&FrontClient::instance(), &FrontClient::messageReceived,
            this, &ChatPage::onMessageReceived);
}

ChatPage::~ChatPage()
{
    delete ui;
}

void ChatPage::setSession(const SessionData& session)
{
    m_session = session;
}

void ChatPage::setChatInfo(const QString& friendId, const QString& friendName, int friendAvatar, int selfAvatar)
{
    m_friendId = friendId;
    m_friendName = friendName;
    m_friendAvatar = friendAvatar;
    m_selfAvatar = selfAvatar;
    
    // 设置标题
    ui->titleLabel->setText(QString("与 %1 聊天").arg(friendName));
    
    // 加载对话
    loadConversation();
}

void ChatPage::refreshConversation()
{
    loadConversation();
}

void ChatPage::loadConversation()
{
    ui->messageList->clear();
    auto msgs = LocalStore::instance().loadMessages(m_friendId);
    for (const auto& m : msgs) {
        addMessageWidget(m);
    }
    ui->messageList->scrollToBottom();
}

void ChatPage::addMessageWidget(const MessageRecord& m)
{
    bool isSelf = (m.direction == 0);
    QWidget *w = new QWidget();
    auto *h = new QHBoxLayout(w);
    h->setContentsMargins(8, 4, 8, 4);
    h->setSpacing(8);

    // 气泡内容
    auto *bubble = buildBubble(m.content, isSelf);
    bubble->setMaximumWidth(260);
    bubble->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    // 头像
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
    ui->messageList->addItem(item);
    ui->messageList->setItemWidget(item, w);
}

QLabel* ChatPage::buildAvatar(int index)
{
    auto *lbl = new QLabel();
    lbl->setFixedSize(40, 40);
    QPixmap pix(QString(":/head_portrait/head_portrait/%1.png").arg(index));
    if (!pix.isNull()) {
        lbl->setPixmap(pix.scaled(lbl->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        lbl->setStyleSheet("");
    }
    if (pix.isNull()) {
        lbl->setText(QString::number(index));
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setStyleSheet("QLabel { background:#d8d8d8; border-radius:20px; color:#333; font-weight:bold; }");
    }
    return lbl;
}

QLabel* ChatPage::buildBubble(const QString& text, bool isSelf)
{
    auto *label = new QLabel(text);
    label->setWordWrap(true);
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    label->setStyleSheet(isSelf
                         ? "QLabel { background:#07c160; color:white; border-radius:12px; padding:10px; }"
                         : "QLabel { background:#f2f3f5; color:#111; border-radius:12px; padding:10px; }");
    return label;
}

void ChatPage::onSendClicked()
{
    QString content = ui->messageInput->text().trimmed();
    if (content.isEmpty()) return;
    
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    // 本地落地
    LocalStore::instance().saveMessage(m_friendId, m_session.userId, content, now, 0);
    
    // 发送到服务器
    QByteArray body;
    MESSAGE_REQ req{};
    memcpy(req.sender_id, m_session.userId.toUtf8().constData(), 
           qMin<int>(static_cast<int>(sizeof(req.sender_id))-1, m_session.userId.size()));
    memcpy(req.receiver_id, m_friendId.toUtf8().constData(), 
           qMin<int>(static_cast<int>(sizeof(req.receiver_id))-1, m_friendId.size()));
    req.content_len = content.toUtf8().size();
    body.append(reinterpret_cast<const char*>(&req), sizeof(req));
    body.append(content.toUtf8());
    FrontClient::instance().sendSimple(MESSAGE, body);
    
    ui->messageInput->clear();
    loadConversation();
}

void ChatPage::onBackClicked()
{
    emit backRequested();
}

void ChatPage::onMessageReceived(const QString& senderId, const QString& content, qint64 timestamp)
{
    // 只有当消息来自当前聊天的好友时才刷新
    if (senderId == m_friendId) {
        qDebug() << "[ChatPage] Received message from current friend, refreshing...";
        refreshConversation();
    }
}