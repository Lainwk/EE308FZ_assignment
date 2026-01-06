#include "channelspage.h"
#include "ui_channelspage.h"
#include "ebookreaderwidget.h"

#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDateTime>
#include <QFont>
#include <QScrollBar>
#include <QInputDialog>
#include <QDialog>
#include <QLineEdit>
#include <QShowEvent>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStackedWidget>
#include <QScrollArea>
#include <QDir>
#include <QCoreApplication>
#include <QPixmap>
#include <QTextDocument>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>

#include "frontclient.h"
#include "localstore.h"

ChannelsPage::ChannelsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelsPage),
    m_ebookReader(nullptr),
    m_bookShelfWidget(nullptr),
    m_toolboxInitialized(false)
{
    ui->setupUi(this);
    initTexts();
    initCommunity();
    initAIChat();
    
    // 连接标签页切换信号
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &ChannelsPage::onTabChanged);
}

ChannelsPage::~ChannelsPage()
{
    delete ui;
}

void ChannelsPage::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    // 移除自动刷新，避免每次切换页面都等待25秒
    // 只在首次显示或距离上次刷新超过30秒时才刷新
    static qint64 lastRefreshTime = 0;
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (lastRefreshTime == 0 || (now - lastRefreshTime) > 30000) {
        refreshFeed();
        lastRefreshTime = now;
    }
}

void ChannelsPage::initTexts()
{
    ui->labelTitle->setText(QStringLiteral("社区"));
    //ui->btnNewChannel->setText(QStringLiteral("发布"));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tabCommunity), QStringLiteral("社区动态"));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tabAIChat), QStringLiteral("AI 对话"));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tabTools), QStringLiteral("心理工具箱"));
    ui->labelTools->setText(QStringLiteral("这里可以放置情绪日记、冥想练习等工具入口。"));
}

SessionData ChannelsPage::currentSession() const
{
    return LocalStore::instance().loadSession();
}

QString ChannelsPage::formatTime(qint64 ts) const
{
    QDateTime dt = QDateTime::fromMSecsSinceEpoch(ts);
    if (!dt.isValid()) {
        return QStringLiteral("刚刚");
    }
    return dt.toString("MM-dd hh:mm");
}

void ChannelsPage::initCommunity()
{
    // 移除初始化时的刷新，改为在showEvent中首次显示时刷新
    connect(ui->btnNewChannel, &QPushButton::clicked, this, [this]() {
        publishCurrentText();
    });
}

void ChannelsPage::onTabChanged(int index)
{
    // 当切换到"心理工具箱"标签页时，初始化电子书架
    if (index == ui->tabWidget->indexOf(ui->tabTools) && !m_toolboxInitialized) {
        initToolbox();
        m_toolboxInitialized = true;
    }
    
    // 只在"社区动态"标签页显示"发布"按钮
    if (index == ui->tabWidget->indexOf(ui->tabCommunity)) {
        ui->btnNewChannel->show();
    } else {
        ui->btnNewChannel->hide();
    }
}

void ChannelsPage::initToolbox()
{
    qDebug() << "[ChannelsPage] Initializing toolbox with e-book shelf";
    qDebug() << "[ChannelsPage] Current working directory:" << QDir::currentPath();
    
    // 隐藏原有的占位文本
    if (ui->labelTools) {
        ui->labelTools->hide();
    }
    
    // 创建堆叠窗口用于切换书架和阅读器
    QStackedWidget* stackedWidget = new QStackedWidget();
    stackedWidget->setContentsMargins(0, 0, 0, 0);  // 移除堆叠窗口边距
    
    // 创建书架页面
    m_bookShelfWidget = new QWidget();
    QVBoxLayout* shelfLayout = new QVBoxLayout(m_bookShelfWidget);
    shelfLayout->setContentsMargins(0, 10, 0, 10);  // 只保留上下边距
    shelfLayout->setSpacing(10);
    
    QLabel* titleLabel = new QLabel(QStringLiteral("📚 心理学电子书架"));
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    shelfLayout->addWidget(titleLabel);
    
    // 创建滚动区域
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("QScrollArea { background: #transparent; border: none; }");
    
    QWidget* scrollContent = new QWidget();
    QHBoxLayout* booksLayout = new QHBoxLayout(scrollContent);
    booksLayout->setSpacing(30);
    booksLayout->setContentsMargins(35, 15, 0, 15);  // 减小左右边距
    booksLayout->setAlignment(Qt::AlignCenter | Qt::AlignTop);  // 改为居中对齐
    booksLayout->setSizeConstraint(QLayout::SetFixedSize); // 固定布局大小
    
    // 尝试多个可能的路径查找books.json
    QStringList possiblePaths;
    
#ifdef Q_OS_ANDROID
    // Android平台：优先从assets读取
    possiblePaths << "assets:/PDF/books.json";
    qDebug() << "[ChannelsPage] Android platform, trying assets path first";
#endif
    
    possiblePaths << "PDF/books.json"
                  << "./PDF/books.json"
                  << "../PDF/books.json"
                  << QCoreApplication::applicationDirPath() + "/PDF/books.json";
    
    QString jsonPath;
    QString pdfDir;
    for (const QString& path : possiblePaths) {
        if (QFile::exists(path)) {
            jsonPath = path;
            // 获取PDF目录路径
#ifdef Q_OS_ANDROID
            if (path.startsWith("assets:")) {
                pdfDir = "assets:/PDF";  // Android assets路径
            } else {
                QFileInfo fileInfo(path);
                pdfDir = fileInfo.absolutePath();
            }
#else
            QFileInfo fileInfo(path);
            pdfDir = fileInfo.absolutePath();
#endif
            qDebug() << "[ChannelsPage] Found books.json at:" << path;
            qDebug() << "[ChannelsPage] PDF directory:" << pdfDir;
            break;
        } else {
            qDebug() << "[ChannelsPage] books.json not found at:" << path;
        }
    }
    
    QFile jsonFile(jsonPath);
    if (!jsonPath.isEmpty() && jsonFile.open(QIODevice::ReadOnly)) {
        QByteArray jsonData = jsonFile.readAll();
        jsonFile.close();
        
        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if (doc.isObject()) {
            QJsonObject root = doc.object();
            QJsonArray books = root["books"].toArray();
            
            for (const QJsonValue& bookVal : books) {
                QJsonObject book = bookVal.toObject();
                int bookId = book["id"].toInt();
                QString title = book["title"].toString();
                QString author = book["author"].toString();
                QString description = book["description"].toString();
                QString coverFile = book["cover"].toString();
                
                // 创建书籍卡片 - 调整为更小的尺寸以适应并排显示
                QWidget* bookCard = new QWidget();
                bookCard->setStyleSheet(
                    "QWidget { background: white; border: 2px solid #e0e0e0; border-radius: 10px; }"

                );
                bookCard->setFixedSize(140, 240); // 进一步缩小尺寸
                
                QVBoxLayout* cardLayout = new QVBoxLayout(bookCard);
                cardLayout->setSpacing(6);
                cardLayout->setContentsMargins(8, 8, 8, 8);
                
                // 封面图片区域
                QLabel* coverLabel = new QLabel();
                coverLabel->setFixedSize(124, 160); // 进一步缩小封面尺寸
                coverLabel->setAlignment(Qt::AlignCenter);
                coverLabel->setStyleSheet("background: #f0f0f0; border: 1px solid #ddd; border-radius: 5px;");
                
                // 优先使用封面文件，其次回退到第一页图片
                QString pagesDir = book["pages_dir"].toString();
                QString pagePrefix = book["page_prefix"].toString();
                
                // 构建路径时考虑Android assets
                QString firstPageFile, coverPath;
#ifdef Q_OS_ANDROID
                if (pdfDir.startsWith("assets:")) {
                    firstPageFile = QString("%1/%2/%3%4.png").arg(pdfDir, pagesDir, pagePrefix).arg(1);
                    coverPath = QString("%1/%2").arg(pdfDir, coverFile);
                } else {
                    firstPageFile = QString("%1/%2/%3%4.png").arg(pdfDir, pagesDir, pagePrefix).arg(1);
                    coverPath = QDir(pdfDir).filePath(coverFile);
                }
#else
                firstPageFile = QString("%1/%2/%3%4.png").arg(pdfDir, pagesDir, pagePrefix).arg(1);
                coverPath = QDir(pdfDir).filePath(coverFile);
#endif
                QString imagePath = (!coverFile.isEmpty() && QFile::exists(coverPath)) ? coverPath : firstPageFile;
                
                qDebug() << "[ChannelsPage] Trying to load cover image:" << imagePath;
                
                QPixmap coverPixmap(imagePath);
                if (!coverPixmap.isNull()) {
                    // 缩放图片以适应标签，保持宽高比
                    coverLabel->setPixmap(coverPixmap.scaled(124, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    qDebug() << "[ChannelsPage] Loaded cover image:" << imagePath;
                } else {
                    // 如果图片不存在，显示默认图标和书名
                    coverLabel->setText(QStringLiteral("📖\n\n%1").arg(title));
                    coverLabel->setWordWrap(true);
                    QFont iconFont = coverLabel->font();
                    iconFont.setPointSize(10);
                    coverLabel->setFont(iconFont);
                    qDebug() << "[ChannelsPage] Cover image not found:" << imagePath;
                }
                cardLayout->addWidget(coverLabel);
                
                // 书名（简短显示）
                QLabel* titleLabel = new QLabel(title);
                QFont titleFont = titleLabel->font();
                titleFont.setBold(true);
                titleFont.setPointSize(9);
                titleLabel->setFont(titleFont);
                titleLabel->setAlignment(Qt::AlignCenter);
                titleLabel->setWordWrap(true);
                titleLabel->setMaximumHeight(40);
                titleLabel->setStyleSheet("border:none");
                cardLayout->addWidget(titleLabel);
                
                // 阅读按钮
                QPushButton* readBtn = new QPushButton(QStringLiteral("开始阅读"));
                readBtn->setStyleSheet(
                    "QPushButton { background: #4a90e2; color: white; border: none; "
                    "border-radius: 5px; padding: 8px; font-weight: bold; font-size: 10pt; }"
                    "QPushButton:hover { background: #2b7bb9; }"
                    "QPushButton:pressed { background: #1a5a8a; }"
                );
                cardLayout->addWidget(readBtn);
                
                // 连接点击事件
                connect(readBtn, &QPushButton::clicked, this, [this, bookId]() {
                    onBookClicked(bookId);
                });
                
                // 添加到水平布局
                booksLayout->addWidget(bookCard);
            }
            
            // 添加弹性空间，使书籍靠左对齐
            booksLayout->addStretch();
            
            qDebug() << "[ChannelsPage] Loaded" << books.size() << "books";
        }
    } else {
        QString errorMsg = QStringLiteral("❌ 无法加载书籍列表\n\n当前工作目录：\n%1\n\n请确保 PDF/books.json 文件存在")
                          .arg(QDir::currentPath());
        QLabel* errorLabel = new QLabel(errorMsg);
        errorLabel->setAlignment(Qt::AlignCenter);
        errorLabel->setStyleSheet("color: red; font-size: 10pt;");
        errorLabel->setWordWrap(true);
        booksLayout->addWidget(errorLabel);
        qWarning() << "[ChannelsPage] Failed to open books.json. Tried paths:" << possiblePaths;
        qWarning() << "[ChannelsPage] Current directory:" << QDir::currentPath();
        qWarning() << "[ChannelsPage] Application directory:" << QCoreApplication::applicationDirPath();
    }
    
    scrollArea->setWidget(scrollContent);
    shelfLayout->addWidget(scrollArea);
    
    // 创建阅读器页面
    m_ebookReader = new EBookReaderWidget();
    connect(m_ebookReader, &EBookReaderWidget::backRequested, this, &ChannelsPage::onBackToShelf);
    
    // 添加到堆叠窗口
    stackedWidget->addWidget(m_bookShelfWidget);
    stackedWidget->addWidget(m_ebookReader);
    stackedWidget->setCurrentWidget(m_bookShelfWidget);
    
    // 将堆叠窗口添加到工具箱标签页
    ui->verticalLayout_4->addWidget(stackedWidget);
    
    qDebug() << "[ChannelsPage] Toolbox initialized successfully";
}

void ChannelsPage::loadBookShelf()
{
    qDebug() << "[ChannelsPage] Loading book shelf from PDF/books.json";
}

void ChannelsPage::onBookClicked(int bookId)
{
    qDebug() << "[ChannelsPage] Book clicked, ID:" << bookId;
    
    // 尝试多个可能的路径查找books.json
    QStringList possiblePaths;
    
#ifdef Q_OS_ANDROID
    // Android平台：优先从assets读取
    possiblePaths << "assets:/PDF/books.json";
#endif
    
    possiblePaths << "PDF/books.json"
                  << "./PDF/books.json"
                  << "../PDF/books.json"
                  << QCoreApplication::applicationDirPath() + "/PDF/books.json";
    
    QString jsonPath;
    QString pdfDir;
    for (const QString& path : possiblePaths) {
        if (QFile::exists(path)) {
            jsonPath = path;
#ifdef Q_OS_ANDROID
            if (path.startsWith("assets:")) {
                pdfDir = "assets:/PDF";
            } else {
                QFileInfo fileInfo(path);
                pdfDir = fileInfo.absolutePath();
            }
#else
            QFileInfo fileInfo(path);
            pdfDir = fileInfo.absolutePath();
#endif
            break;
        }
    }
    
    QFile jsonFile(jsonPath);
    if (jsonPath.isEmpty()) {
        qWarning() << "[ChannelsPage] jsonPath is empty!";
        return;
    }
    
    if (!jsonFile.open(QIODevice::ReadOnly)) {
        qWarning() << "[ChannelsPage] Failed to open books.json at:" << jsonPath;
        qWarning() << "[ChannelsPage] Error:" << jsonFile.errorString();
        return;
    }
    
    QByteArray jsonData = jsonFile.readAll();
    jsonFile.close();
    
    qDebug() << "[ChannelsPage] JSON data size:" << jsonData.size() << "bytes";
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isObject()) {
        qWarning() << "[ChannelsPage] JSON is not an object!";
        return;
    }
    
    QJsonObject root = doc.object();
    QJsonArray books = root["books"].toArray();
    
    qDebug() << "[ChannelsPage] Found" << books.size() << "books in JSON";
    
    for (const QJsonValue& bookVal : books) {
        QJsonObject book = bookVal.toObject();
        qDebug() << "[ChannelsPage] Checking book ID:" << book["id"].toInt() << "vs" << bookId;
        
        if (book["id"].toInt() == bookId) {
            QString title = book["title"].toString();
            QString author = book["author"].toString();
            QString pagesDir = book["pages_dir"].toString();
            QString pagePrefix = book["page_prefix"].toString();
            int pageCount = book["page_count"].toInt();
            
            qDebug() << "[ChannelsPage] Read from JSON:"
                     << "pagesDir=" << pagesDir
                     << "pagePrefix=" << pagePrefix
                     << "pageCount=" << pageCount;
            qDebug() << "[ChannelsPage] JSON keys:" << book.keys();
            
            // 构建完整的页面目录路径
            QString fullPagesDir = pdfDir + "/" + pagesDir;
            
            qDebug() << "[ChannelsPage] Opening book:" << title
                     << "Pages:" << pageCount
                     << "Full pages dir:" << fullPagesDir
                     << "Prefix:" << pagePrefix;
            
            // 切换到阅读器并加载PDF页面
            m_ebookReader->loadPDFPages(title, author, fullPagesDir, pagePrefix, pageCount);
            
            // 切换堆叠窗口显示阅读器
            QStackedWidget* stackedWidget = qobject_cast<QStackedWidget*>(m_ebookReader->parentWidget());
            if (stackedWidget) {
                stackedWidget->setCurrentWidget(m_ebookReader);
                qDebug() << "[ChannelsPage] Switched to reader view";
            } else {
                qWarning() << "[ChannelsPage] Failed to get stacked widget";
            }
            
            break;
        }
    }
}

void ChannelsPage::onBackToShelf()
{
    qDebug() << "[ChannelsPage] Back to shelf requested";
    
    // 切换回书架
    QStackedWidget* stackedWidget = qobject_cast<QStackedWidget*>(m_ebookReader->parentWidget());
    if (stackedWidget) {
        stackedWidget->setCurrentWidget(m_bookShelfWidget);
    }
}

void ChannelsPage::publishCurrentText()
{
    QString content = ui->txtPost->toPlainText().trimmed();
    if (content.isEmpty()) {
        QMessageBox::warning(this, tr("发布失败"), tr("内容不能为空"));
        return;
    }
    SessionData s = currentSession();
    if (s.userId.isEmpty()) {
        QMessageBox::warning(this, tr("发布失败"), tr("请先登录后再发布"));
        return;
    }

    BLOG_REQ req{};
    QByteArray body;
    QByteArray uid = (s.userId.isEmpty() ? QByteArray("0") : s.userId.toUtf8());
    memcpy(req.user_id, uid.constData(), qMin<int>(static_cast<int>(sizeof(req.user_id)) - 1, uid.size()));
    req.is_anonymous = 0;
    req.content_len = content.toUtf8().size();
    body.append(reinterpret_cast<const char*>(&req), sizeof(req));
    body.append(content.toUtf8());

    HEAD respHead{};
    QByteArray resp = FrontClient::instance().sendAndReceive(BLOG, body, &respHead);
    if (resp.size() < static_cast<int>(sizeof(BLOG_RESP))) {
        QMessageBox::warning(this, tr("发布失败"), tr("无法连接服务器或返回数据无效"));
        return;
    }
    BLOG_RESP respBody{};
    memcpy(&respBody, resp.constData(), sizeof(respBody));
    if (respBody.status_code == 0) {
        ui->txtPost->clear();
        refreshFeed();
        QMessageBox::information(this, tr("发布成功"), tr("帖子已发布"));
    } else {
        QMessageBox::warning(this, tr("发布失败"), tr("服务器拒绝发布或参数错误"));
    }
}

bool ChannelsPage::fetchFeedFromServer()
{
    SessionData s = currentSession();
    BLOG_FEED_REQ req{};
    QByteArray body;
    QByteArray uid = (s.userId.isEmpty() ? QByteArray("0") : s.userId.toUtf8());
    memcpy(req.user_id, uid.constData(), qMin<int>(static_cast<int>(sizeof(req.user_id)) - 1, uid.size()));
    req.limit = 50;
    body.append(reinterpret_cast<const char*>(&req), sizeof(req));

    HEAD respHead{};
    QByteArray resp = FrontClient::instance().sendAndReceive(BLOG_FEED, body, &respHead);
    if (respHead.data_len <= 0 || resp.size() < static_cast<int>(sizeof(BLOG_FEED_RESP))) {
        return false;
    }

    int offset = 0;
    BLOG_FEED_RESP feed{};
    memcpy(&feed, resp.constData(), sizeof(feed));
    offset += sizeof(feed);

    QVector<PostRecord> posts;
    for (int i = 0; i < feed.blog_count; ++i) {
        if (offset + static_cast<int>(sizeof(BLOG_ITEM)) > resp.size()) break;
        BLOG_ITEM item{};
        memcpy(&item, resp.constData() + offset, sizeof(BLOG_ITEM));
        offset += sizeof(BLOG_ITEM);
        if (offset + item.content_len > resp.size()) break;
        QByteArray contentBytes = resp.mid(offset, item.content_len);
        offset += item.content_len;

        PostRecord p;
        p.id = QByteArray(item.blog_id).toInt();
        p.authorId = QString::fromLatin1(item.author_id).trimmed();
        p.authorName = QString::fromLatin1(item.author_name).trimmed();
        if (p.authorName.isEmpty()) p.authorName = QStringLiteral("匿名");
        p.content = QString::fromUtf8(contentBytes);
        p.likes = item.likes;
        p.comments = item.comments;
        QDateTime dt = QDateTime::fromString(QString::fromLatin1(item.created_at), "yyyy-MM-dd hh:mm:ss");
        p.timestampMs = dt.isValid() ? dt.toMSecsSinceEpoch() : QDateTime::currentMSecsSinceEpoch();
        posts.append(p);
    }
    m_posts = posts;
    return true;
}

QVector<CommentRecord> ChannelsPage::fetchCommentsFromServer(const QString& blogId, int limit)
{
    QVector<CommentRecord> res;
    BLOG_COMMENT_LIST_REQ req{};
    QByteArray body;
    QByteArray bid = blogId.toUtf8();
    memcpy(req.blog_id, bid.constData(), qMin<int>(static_cast<int>(sizeof(req.blog_id)) - 1, bid.size()));
    req.limit = limit;
    body.append(reinterpret_cast<const char*>(&req), sizeof(req));

    HEAD respHead{};
    QByteArray resp = FrontClient::instance().sendAndReceive(BLOG_COMMENT_LIST, body, &respHead);
    if (resp.size() < static_cast<int>(sizeof(BLOG_COMMENT_LIST_RESP))) return res;

    int offset = 0;
    BLOG_COMMENT_LIST_RESP listResp{};
    memcpy(&listResp, resp.constData(), sizeof(listResp));
    offset += sizeof(listResp);

    for (int i = 0; i < listResp.comment_count; ++i) {
        if (offset + static_cast<int>(sizeof(BLOG_COMMENT_ITEM)) > resp.size()) break;
        BLOG_COMMENT_ITEM item{};
        memcpy(&item, resp.constData() + offset, sizeof(BLOG_COMMENT_ITEM));
        offset += sizeof(BLOG_COMMENT_ITEM);
        if (offset + item.content_len > resp.size()) break;
        QByteArray contentBytes = resp.mid(offset, item.content_len);
        offset += item.content_len;

        CommentRecord c;
        c.authorName = QString::fromLatin1(item.author_name).trimmed();
        c.content = QString::fromUtf8(contentBytes);
        QDateTime dt = QDateTime::fromString(QString::fromLatin1(item.created_at), "yyyy-MM-dd hh:mm:ss");
        c.timestampMs = dt.isValid() ? dt.toMSecsSinceEpoch() : QDateTime::currentMSecsSinceEpoch();
        res.append(c);
    }
    return res;
}

QWidget* ChannelsPage::buildPostWidget(const PostRecord& post, int index)
{
    QWidget *card = new QWidget;
    card->setObjectName("postCard");
    card->setStyleSheet("#postCard { background: #f7f7f7; border: 1px solid #e5e5e5; border-radius: 8px; }");
    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(12, 10, 12, 10);
    layout->setSpacing(6);

    QLabel *icon = new QLabel();
    icon->setStyleSheet("border-image: url(:/image/person.png)");
    icon->setFixedSize(25,25);

    auto *header = new QHBoxLayout();
    header->setSpacing(6);
    auto *authorLbl = new QLabel(post.authorName.isEmpty() ? QStringLiteral("匿名") : post.authorName);
    QFont authorFont = authorLbl->font();
    authorFont.setBold(true);
    authorLbl->setFont(authorFont);
    auto *timeLbl = new QLabel(formatTime(post.timestampMs));
    timeLbl->setStyleSheet("color:#666;");
    header->addWidget(icon);
    header->addWidget(authorLbl);
    header->addStretch();
    header->addWidget(timeLbl);
    layout->addLayout(header);

    auto *contentLbl = new QLabel(post.content);
    contentLbl->setWordWrap(true);
    contentLbl->setTextInteractionFlags(Qt::TextSelectableByMouse);
    contentLbl->setStyleSheet("color:#111; line-height:1.4;");
    contentLbl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    bool expanded = (index < m_expanded.size()) ? m_expanded[index] : false;
    bool needsToggle = post.content.size() > 100;
    if (!expanded && needsToggle) {
        contentLbl->setMaximumHeight(100);
    } else {
        contentLbl->setMaximumHeight(QWIDGETSIZE_MAX);
    }

    layout->addWidget(contentLbl);

    auto *footer = new QHBoxLayout();
    footer->setSpacing(12);

    auto *btnLike = new QPushButton(QStringLiteral("点赞 %1").arg(post.likes));
    btnLike->setFlat(true);
    btnLike->setStyleSheet("color:#6B7280;");
    footer->addWidget(btnLike);

    auto *btnComment = new QPushButton(QStringLiteral("评论 %1").arg(post.comments));
    btnComment->setFlat(true);
    btnComment->setStyleSheet("color:#6B7280;");
    footer->addWidget(btnComment);

    QPushButton *btnViewComments = new QPushButton(QStringLiteral("查看评论"));
    btnViewComments->setFlat(true);
    btnViewComments->setStyleSheet("color:#6B7280;");
    footer->addWidget(btnViewComments);

    footer->addStretch();

    if (needsToggle) {
        QPushButton *toggleBtn = new QPushButton(expanded ? QStringLiteral("收起") : QStringLiteral("展开全文"));
        toggleBtn->setFlat(true);
        toggleBtn->setStyleSheet("color:#2b7bb9;");
        footer->addWidget(toggleBtn);
        connect(toggleBtn, &QPushButton::clicked, this, [this, index, contentLbl, toggleBtn]() {
            if (index >= m_expanded.size()) return;
            m_expanded[index] = !m_expanded[index];
            
            // 直接更新UI，不重新获取数据
            if (m_expanded[index]) {
                contentLbl->setMaximumHeight(QWIDGETSIZE_MAX);
                toggleBtn->setText(QStringLiteral("收起"));
            } else {
                contentLbl->setMaximumHeight(80);
                toggleBtn->setText(QStringLiteral("展开全文"));
            }
        });
    }

    connect(btnLike, &QPushButton::clicked, this, [this, post, btnLike]() {
        // 乐观更新UI
        btnLike->setText(QStringLiteral("点赞 %1").arg(post.likes + 1));
        btnLike->setEnabled(false);
        
        BLOG_LIKE_REQ req{};
        QByteArray body;
        QByteArray bid = QByteArray::number(post.id);
        memcpy(req.blog_id, bid.constData(), qMin<int>(static_cast<int>(sizeof(req.blog_id)) - 1, bid.size()));
        SessionData s = currentSession();
        QByteArray uid = (s.userId.isEmpty() ? QByteArray("0") : s.userId.toUtf8());
        memcpy(req.user_id, uid.constData(), qMin<int>(static_cast<int>(sizeof(req.user_id)) - 1, uid.size()));
        body.append(reinterpret_cast<const char*>(&req), sizeof(req));
        FrontClient::instance().sendAndReceive(BLOG_LIKE, body, nullptr);
        // 移除刷新，使用乐观更新
    });

    connect(btnComment, &QPushButton::clicked, this, [this, post, btnComment]() {
        bool ok = false;
        QString txt = QInputDialog::getText(this, QStringLiteral("评论"), QStringLiteral("写下你的想法:"),
                                            QLineEdit::Normal, QString(), &ok).trimmed();
        if (!ok || txt.isEmpty()) return;
        
        // 乐观更新UI
        btnComment->setText(QStringLiteral("评论 %1").arg(post.comments + 1));
        
        BLOG_COMMENT_REQ req{};
        QByteArray body;
        QByteArray bid = QByteArray::number(post.id);
        memcpy(req.blog_id, bid.constData(), qMin<int>(static_cast<int>(sizeof(req.blog_id)) - 1, bid.size()));
        SessionData s = currentSession();
        QByteArray uid = (s.userId.isEmpty() ? QByteArray("0") : s.userId.toUtf8());
        QByteArray uname = (s.userName.isEmpty() ? QByteArray("我") : s.userName.toUtf8());
        memcpy(req.user_id, uid.constData(), qMin<int>(static_cast<int>(sizeof(req.user_id)) - 1, uid.size()));
        memcpy(req.user_name, uname.constData(), qMin<int>(static_cast<int>(sizeof(req.user_name)) - 1, uname.size()));
        QByteArray content = txt.toUtf8();
        req.content_len = content.size();
        body.append(reinterpret_cast<const char*>(&req), sizeof(req));
        body.append(content);
        FrontClient::instance().sendAndReceive(BLOG_COMMENT, body, nullptr);
        // 移除刷新，使用乐观更新
    });

    connect(btnViewComments, &QPushButton::clicked, this, [this, post]() {
        // 创建非模态对话框
        QDialog* dlg = new QDialog(this);
        dlg->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动删除
        dlg->setWindowTitle(QStringLiteral("评论列表"));
        dlg->resize(400, 450);
        
        QVBoxLayout* mainLayout = new QVBoxLayout(dlg);
        mainLayout->setContentsMargins(12, 12, 12, 12);
        mainLayout->setSpacing(8);
        
        // 标题栏
        QLabel* titleLabel = new QLabel(QStringLiteral("💬 评论列表"));
        QFont titleFont = titleLabel->font();
        titleFont.setPointSize(12);
        titleFont.setBold(true);
        titleLabel->setFont(titleFont);
        titleLabel->setStyleSheet("color: #333; padding: 4px;");
        mainLayout->addWidget(titleLabel);
        
        // 加载提示
        QLabel* loadingLabel = new QLabel(QStringLiteral("正在加载评论..."));
        loadingLabel->setAlignment(Qt::AlignCenter);
        loadingLabel->setStyleSheet("color: #888; padding: 20px; font-size: 11pt;");
        mainLayout->addWidget(loadingLabel);
        
        // 评论列表（初始隐藏）
        QListWidget* commentList = new QListWidget();
        commentList->setStyleSheet(
            "QListWidget { "
            "  border: 1px solid #e0e0e0; "
            "  border-radius: 6px; "
            "  background: white; "
            "  padding: 4px; "
            "} "
            "QListWidget::item { "
            "  border-bottom: 1px solid #f0f0f0; "
            "  padding: 8px; "
            "  margin: 2px 0; "
            "} "
            "QListWidget::item:hover { "
            "  background: #f5f5f5; "
            "} "
            "QListWidget::item:selected { "
            "  background: #e3f2fd; "
            "  color: #1976d2; "
            "}"
        );
        commentList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        commentList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        commentList->hide(); // 初始隐藏
        mainLayout->addWidget(commentList);
        
        // 关闭按钮
        QPushButton* closeBtn = new QPushButton(QStringLiteral("关闭"));
        closeBtn->setStyleSheet(
            "QPushButton { "
            "  background: #f0f0f0; "
            "  border: none; "
            "  border-radius: 4px; "
            "  padding: 8px 16px; "
            "  font-size: 10pt; "
            "} "
            "QPushButton:hover { "
            "  background: #e0e0e0; "
            "} "
            "QPushButton:pressed { "
            "  background: #d0d0d0; "
            "}"
        );
        connect(closeBtn, &QPushButton::clicked, dlg, &QDialog::close);
        mainLayout->addWidget(closeBtn, 0, Qt::AlignRight);
        
        // 显示非模态对话框
        dlg->show();
        
        // 异步加载评论
        QString blogId = QString::number(post.id);
        QFuture<QVector<CommentRecord>> future = QtConcurrent::run([this, blogId]() {
            return fetchCommentsFromServer(blogId, 50);
        });
        
        // 创建监视器
        QFutureWatcher<QVector<CommentRecord>>* watcher = new QFutureWatcher<QVector<CommentRecord>>(dlg);
        connect(watcher, &QFutureWatcher<QVector<CommentRecord>>::finished, dlg, [dlg, loadingLabel, commentList, watcher]() {
            QVector<CommentRecord> comments = watcher->result();
            
            // 线程安全地更新UI
            QMetaObject::invokeMethod(dlg, [dlg, loadingLabel, commentList, comments]() {
                loadingLabel->hide();
                commentList->show();
                
                if (comments.isEmpty()) {
                    QListWidgetItem* emptyItem = new QListWidgetItem(QStringLiteral("暂无评论"));
                    emptyItem->setTextAlignment(Qt::AlignCenter);
                    emptyItem->setForeground(QColor("#999"));
                    commentList->addItem(emptyItem);
                } else {
                    for (const CommentRecord& c : comments) {
                        // 创建自定义评论项
                        QWidget* commentWidget = new QWidget();
                        QVBoxLayout* itemLayout = new QVBoxLayout(commentWidget);
                        itemLayout->setContentsMargins(8, 6, 8, 6);
                        itemLayout->setSpacing(4);
                        
                        // 作者和时间
                        QHBoxLayout* headerLayout = new QHBoxLayout();
                        headerLayout->setSpacing(8);
                        
                        QLabel* authorLabel = new QLabel(c.authorName.isEmpty() ? QStringLiteral("匿名") : c.authorName);
                        QFont authorFont = authorLabel->font();
                        authorFont.setBold(true);
                        authorFont.setPointSize(9);
                        authorLabel->setFont(authorFont);
                        authorLabel->setStyleSheet("color: #333;");
                        headerLayout->addWidget(authorLabel);
                        
                        QDateTime dt = QDateTime::fromMSecsSinceEpoch(c.timestampMs);
                        QLabel* timeLabel = new QLabel(dt.toString("MM-dd hh:mm"));
                        timeLabel->setStyleSheet("color: #999; font-size: 8pt;");
                        headerLayout->addWidget(timeLabel);
                        headerLayout->addStretch();
                        
                        itemLayout->addLayout(headerLayout);
                        
                        // 评论内容
                        QLabel* contentLabel = new QLabel(c.content);
                        contentLabel->setWordWrap(true);
                        contentLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
                        contentLabel->setStyleSheet("color: #555; font-size: 9pt; line-height: 1.4;");
                        itemLayout->addWidget(contentLabel);
                        
                        QListWidgetItem* item = new QListWidgetItem();
                        item->setSizeHint(commentWidget->sizeHint());
                        commentList->addItem(item);
                        commentList->setItemWidget(item, commentWidget);
                    }
                }
            }, Qt::QueuedConnection);
            
            watcher->deleteLater();
        });
        
        watcher->setFuture(future);
    });

    layout->addLayout(footer);
    return card;
}

void ChannelsPage::refreshFeed()
{
    if (!fetchFeedFromServer()) {
        return;
    }
    if (m_expanded.size() != m_posts.size()) {
        m_expanded.fill(false, m_posts.size());
    }

    ui->listFeed->clear();

    for (int i = 0; i < m_posts.size(); ++i) {
        auto *w = buildPostWidget(m_posts.at(i), i);
        auto *item = new QListWidgetItem();
        item->setSizeHint(w->sizeHint());
        ui->listFeed->addItem(item);
        ui->listFeed->setItemWidget(item, w);
    }

    ui->listFeed->scrollToTop();
}

// ============ AI对话功能实现 ============

void ChannelsPage::initAIChat()
{
    // 连接发送按钮
    connect(ui->btnAISend1, SIGNAL(clicked()), this, SLOT(onAISendClicked()));
    
    // 连接回车键发送
    connect(ui->txtAIInput, SIGNAL(returnPressed()), this, SLOT(onAISendClicked()));

    // 聊天列表设置：禁止横向滚动条，按像素平滑滚动
    ui->listAIChat->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listAIChat->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->listAIChat->setSpacing(4);
    
    // 添加欢迎消息
    addSystemMessage(QStringLiteral("你好！我是AI心理助手，有什么可以帮助你的吗？"));
}

void ChannelsPage::onAISendClicked()
{
    QString message = ui->txtAIInput->text().trimmed();
    if (message.isEmpty()) {
        return;
    }
    
    // 清空输入框
    ui->txtAIInput->clear();
    
    // 显示用户消息
    addUserMessage(message);
    
    // 发送到服务器
    sendAIMessage(message);
}

void ChannelsPage::sendAIMessage(const QString& message)
{
    int thinkingRow = -1;
    
    // 显示"正在思考..."提示
    addSystemMessage(QStringLiteral("AI正在思考..."));
    thinkingRow = ui->listAIChat->count() - 1;
    
    // 强制刷新UI，确保"正在思考"消息立即显示
    QCoreApplication::processEvents();
    
    auto removeThinking = [&]() {
        if (thinkingRow >= 0 && thinkingRow < ui->listAIChat->count()) {
            delete ui->listAIChat->takeItem(thinkingRow);
            thinkingRow = -1;
        }
    };
    
    // 构建请求
    AI_CHAT_REQ req;
    memset(&req, 0, sizeof(req));
    
    SessionData session = currentSession();
    req.user_id = session.userId.toInt();
    
    QByteArray msgBytes = message.toUtf8();
    strncpy(req.message, msgBytes.constData(), sizeof(req.message) - 1);
    req.message[sizeof(req.message) - 1] = '\0';
    
    // 发送请求
    QByteArray body(reinterpret_cast<const char*>(&req), sizeof(req));
    HEAD respHead;
    QByteArray respBody = FrontClient::instance().sendAndReceive(AI_CHAT, body, &respHead);
    
    // 移除"正在思考..."提示
    removeThinking();
    
    // 解析响应
    if (respBody.isEmpty()) {
        addSystemMessage(QStringLiteral("错误：无法连接到AI服务"));
        return;
    }
    
    const AI_CHAT_RESP* resp = reinterpret_cast<const AI_CHAT_RESP*>(respBody.constData());
    
    if (resp->status_code != 0) {
        QString errorMsg = QString::fromUtf8(resp->error_msg);
        if (errorMsg.isEmpty()) {
            errorMsg = QStringLiteral("AI服务返回错误");
        }
        addSystemMessage(QStringLiteral("错误：") + errorMsg);
        return;
    }
    
    // 显示AI回复
    QString aiResponse = QString::fromUtf8(resp->ai_response);
    if (aiResponse.isEmpty()) {
        addSystemMessage(QStringLiteral("错误：AI未返回有效响应"));
        return;
    }
    
    addAIMessage(aiResponse);
}

void ChannelsPage::addUserMessage(const QString& message)
{
    SessionData session = currentSession();
    int avatarIndex = session.avatarIndex;
    
    QWidget* msgWidget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(msgWidget);
    layout->setContentsMargins(6, 4, 6, 4);
    layout->setSpacing(6);
    
    // 用户消息：右对齐布局
    // 添加弹性空间（左侧）
    layout->addStretch();
    
    // 消息气泡 - 参考好友聊天设计（使用字体度量避免裁剪/行距异常）
    QLabel* bubble = new QLabel();
    bubble->setWordWrap(true);
    bubble->setTextInteractionFlags(Qt::TextSelectableByMouse);
    bubble->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    int viewWidth = ui->listAIChat->viewport()->width();
    // 根据文本动态计算尺寸，限制最大宽度
    int bubbleMaxWidth = (viewWidth > 0) ? viewWidth - 60 : 260;
    int bubbleMinWidth = 60;
    const int padding = 12;
    QFontMetrics fm(bubble->font());
    QRect br = fm.boundingRect(QRect(0, 0, bubbleMaxWidth - padding * 2, INT_MAX),
                               Qt::TextWordWrap, message);
    int bubbleWidth = qBound(bubbleMinWidth, br.width() + padding * 2, bubbleMaxWidth);
    int bubbleHeight = br.height() + padding * 2 + 4; // 额外留白防止底部裁切

    bubble->setText(message);
    bubble->setWordWrap(true);
    bubble->setFixedWidth(bubbleWidth);
    bubble->setMinimumHeight(bubbleHeight);
    bubble->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    bubble->setStyleSheet(
        "QLabel { "
        "background: #07c160; "
        "color: white; "
        "border-radius: 12px; "
        "padding: 10px; "
        "margin:0; "
        "}"
    );
    layout->addWidget(bubble);
    
    // 用户头像
    QLabel* avatar = new QLabel();
    avatar->setFixedSize(40, 40);
    QPixmap pix(QString(":/head_portrait/head_portrait/%1.png").arg(avatarIndex));
    if (!pix.isNull()) {
        avatar->setPixmap(pix.scaled(avatar->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        avatar->setText(QString::number(avatarIndex));
        avatar->setAlignment(Qt::AlignCenter);
        avatar->setStyleSheet("QLabel { background:#d8d8d8; border-radius:20px; color:#333; font-weight:bold; }");
    }
    layout->addWidget(avatar);
    
    // 计算并设置合适的高度，避免文字被裁剪
    layout->activate();
    msgWidget->adjustSize();

    QListWidgetItem* item = new QListWidgetItem();
    int avatarHeight = avatar->sizeHint().height();
    int itemHeight = qMax(bubbleHeight, avatarHeight) + layout->contentsMargins().top() + layout->contentsMargins().bottom();
    int itemWidth = (viewWidth > 0) ? viewWidth - 12 : bubbleWidth + avatar->width() + 32;
    item->setSizeHint(QSize(itemWidth, itemHeight));
    ui->listAIChat->addItem(item);
    ui->listAIChat->setItemWidget(item, msgWidget);
    ui->listAIChat->scrollToBottom();
}

void ChannelsPage::addAIMessage(const QString& message)
{
    QWidget* msgWidget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(msgWidget);
    layout->setContentsMargins(6, 4, 6, 4);
    layout->setSpacing(6);
    
    // AI消息：左对齐布局（与好友消息一致）
    // AI头像（使用0号头像）
    QLabel* avatar = new QLabel();
    avatar->setFixedSize(40, 40);
    QPixmap pix(QString(":/head_portrait/head_portrait/0.png"));
    if (!pix.isNull()) {
        avatar->setPixmap(pix.scaled(avatar->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        avatar->setText("AI");
        avatar->setAlignment(Qt::AlignCenter);
        avatar->setStyleSheet("QLabel { background:#4A90E2; border-radius:20px; color:white; font-weight:bold; }");
    }
    layout->addWidget(avatar);
    
    // 消息气泡 - 参考好友聊天设计（使用字体度量避免裁剪/行距异常）
    QLabel* bubble = new QLabel();
    bubble->setWordWrap(true);
    bubble->setTextInteractionFlags(Qt::TextSelectableByMouse);
    bubble->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    int viewWidth = ui->listAIChat->viewport()->width();
    // 根据文本动态计算尺寸，限制最大宽度
    int bubbleMaxWidth = (viewWidth > 0) ? viewWidth - 60 : 260;
    int bubbleMinWidth = 60;
    const int padding = 12;
    QFontMetrics fm(bubble->font());
    QRect br = fm.boundingRect(QRect(0, 0, bubbleMaxWidth - padding * 2, INT_MAX),
                               Qt::TextWordWrap, message);
    int bubbleWidth = qBound(bubbleMinWidth, br.width() + padding * 2, bubbleMaxWidth);
    int bubbleHeight = br.height() + padding * 2 + 4;

    bubble->setText(message);
    bubble->setWordWrap(true);
    bubble->setFixedWidth(bubbleWidth);
    bubble->setMinimumHeight(bubbleHeight);
    bubble->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    bubble->setStyleSheet(
        "QLabel { "
        "background: #f2f3f5; "
        "color: #111; "
        "border-radius: 12px; "
        "padding: 10px; "
        "margin:0; "
        "}"
    );
    layout->addWidget(bubble);
    
    // 添加弹性空间（右侧）
    layout->addStretch();
    
    // 计算并设置合适的高度，避免文字被裁剪
    layout->activate();
    msgWidget->adjustSize();

    QListWidgetItem* item = new QListWidgetItem();
    int avatarHeight = avatar->sizeHint().height();
    int itemHeight = qMax(bubbleHeight, avatarHeight) + layout->contentsMargins().top() + layout->contentsMargins().bottom();
    int itemWidth = (viewWidth > 0) ? viewWidth - 12 : bubbleWidth + avatar->width() + 32;
    item->setSizeHint(QSize(itemWidth, itemHeight));
    ui->listAIChat->addItem(item);
    ui->listAIChat->setItemWidget(item, msgWidget);
    ui->listAIChat->scrollToBottom();
}

void ChannelsPage::addSystemMessage(const QString& message)
{
    QWidget* msgWidget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(msgWidget);
    layout->setContentsMargins(8, 4, 8, 4);
    layout->setSpacing(0);
    
    // 居中的系统消息 - 使用弹性空间实现真正的居中
    layout->addStretch();
    
    QLabel* label = new QLabel(message);
    label->setWordWrap(false);  // 系统消息通常较短，不需要换行
    label->setAlignment(Qt::AlignCenter);
    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);  // 让标签根据内容自适应
    label->setStyleSheet(
        "QLabel { "
        "background-color: #e5e5e5; "
        "border-radius: 4px; "
        "padding: 6px 12px; "
        "color: #888; "
        "font-size: 12px; "
        "}"
    );
    layout->addWidget(label, 0, Qt::AlignCenter);
    
    layout->addStretch();
    
    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(msgWidget->sizeHint());
    ui->listAIChat->addItem(item);
    ui->listAIChat->setItemWidget(item, msgWidget);
    ui->listAIChat->scrollToBottom();
}
