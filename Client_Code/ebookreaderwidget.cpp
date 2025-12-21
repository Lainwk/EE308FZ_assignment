#include "ebookreaderwidget.h"
#include "ui_ebookreaderwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QDebug>
#include <QPixmap>
#include <QScrollArea>
#include <QDir>
#include <QCoreApplication>

EBookReaderWidget::EBookReaderWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EBookReaderWidget),
    m_currentPage(1),
    m_totalPages(0)
{
    ui->setupUi(this);
    
    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);  // 移除边距，最大化显示区域
    mainLayout->setSpacing(0);  // 移除间距
    
    // 顶部工具栏 - 优化布局以适应400px宽度
    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    toolbarLayout->setContentsMargins(5, 5, 5, 5);
    toolbarLayout->setSpacing(5);
    
    m_btnBack = new QPushButton(this);
    m_btnBack->setFixedSize(25,25);  // 固定宽度
    m_btnBack->setStyleSheet("border-image: url(:/image/return.png);");
//    m_btnBack->setStyleSheet(
//        "QPushButton { background: #4a90e2; color: white; border: none; "
//        "border-radius: 5px; padding: 6px 10px; font-weight: bold; font-size: 9pt; }"
//        "QPushButton:hover { background: #2b7bb9; }"
//    );
    connect(m_btnBack, &QPushButton::clicked, this, &EBookReaderWidget::onBackToShelf);
    
    m_titleLabel = new QLabel();
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(10);  // 缩小字体
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setWordWrap(false);  // 不换行
    m_titleLabel->setMaximumWidth(310);  // 限制最大宽度（400 - 70按钮 - 20边距）
    m_titleLabel->setStyleSheet("QLabel { color: #333; }");
    
    toolbarLayout->setContentsMargins(10,9,30,9);
    toolbarLayout->addWidget(m_btnBack);
    toolbarLayout->addWidget(m_titleLabel);  // 拉伸因子为1
    
    mainLayout->addLayout(toolbarLayout);
    
    // PDF页面显示区域（使用滚动区域）
    m_scrollArea = new QScrollArea();
    m_scrollArea->setWidgetResizable(true);  // 改为true，让容器自动调整
    m_scrollArea->setAlignment(Qt::AlignCenter);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // 禁用水平滚动条
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);  // 垂直滚动条按需显示
    m_scrollArea->setStyleSheet("QScrollArea { background: #e0e0e0; border: none; }");
    
    // 创建容器widget来包裹图片标签，实现居中
    QWidget* containerWidget = new QWidget();
    QHBoxLayout* containerLayout = new QHBoxLayout(containerWidget);  // 使用水平布局
    containerLayout->setAlignment(Qt::AlignCenter);  // 容器内容居中
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);
    
    m_pageDisplay = new QLabel();
    m_pageDisplay->setAlignment(Qt::AlignCenter);
    m_pageDisplay->setStyleSheet("QLabel { background: white; border: 1px solid #ccc; }");
    m_pageDisplay->setScaledContents(false);
    
    containerLayout->addWidget(m_pageDisplay);
    m_scrollArea->setWidget(containerWidget);
    mainLayout->addWidget(m_scrollArea, 1);
    
    // 底部导航区域 - 优化以适应400px宽度
    QVBoxLayout* bottomLayout = new QVBoxLayout();
    bottomLayout->setSpacing(8);
    bottomLayout->setContentsMargins(10, 5, 10, 5);  // 左右各10px边距
    
    // 第一行：页码显示（居中）
    m_pageLabel = new QLabel(QStringLiteral("第 1 页 / 共 1 页"));
    m_pageLabel->setAlignment(Qt::AlignCenter);
    QFont pageFont = m_pageLabel->font();
    pageFont.setPointSize(10);
    pageFont.setBold(true);
    m_pageLabel->setFont(pageFont);
    m_pageLabel->setStyleSheet("QLabel { color: #333; padding: 5px; }");
    bottomLayout->addWidget(m_pageLabel);
    
    // 第二行：导航按钮 - 居中对齐，固定宽度
    QHBoxLayout* navLayout = new QHBoxLayout();
    navLayout->setSpacing(5);  // 减小间距
    navLayout->setAlignment(Qt::AlignCenter);  // 居中对齐
    
    m_btnFirst = new QPushButton(QStringLiteral("|◀"));
    m_btnFirst->setFixedSize(40, 32);  // 缩小尺寸
    m_btnFirst->setStyleSheet(
        "QPushButton { background: #4a90e2; color: white; border: none; "
        "border-radius: 5px; font-weight: bold; font-size: 11pt; }"
        "QPushButton:hover { background: #2b7bb9; }"
        "QPushButton:disabled { background: #cccccc; }"
    );
    m_btnFirst->setToolTip(QStringLiteral("第一页"));
    connect(m_btnFirst, &QPushButton::clicked, this, &EBookReaderWidget::onFirstPage);
    
    m_btnPrev = new QPushButton(QStringLiteral("◀ 上一页"));
    m_btnPrev->setFixedWidth(80);  // 固定宽度
    m_btnPrev->setFixedHeight(32);
    m_btnPrev->setStyleSheet(
        "QPushButton { background: #4a90e2; color: white; border: none; "
        "border-radius: 5px; padding: 6px 10px; font-weight: bold; font-size: 9pt; }"
        "QPushButton:hover { background: #2b7bb9; }"
        "QPushButton:disabled { background: #cccccc; }"
    );
    connect(m_btnPrev, &QPushButton::clicked, this, &EBookReaderWidget::onPreviousPage);
    
    m_btnNext = new QPushButton(QStringLiteral("下一页 ▶"));
    m_btnNext->setFixedWidth(80);  // 固定宽度
    m_btnNext->setFixedHeight(32);
    m_btnNext->setStyleSheet(
        "QPushButton { background: #4a90e2; color: white; border: none; "
        "border-radius: 5px; padding: 6px 10px; font-weight: bold; font-size: 9pt; }"
        "QPushButton:hover { background: #2b7bb9; }"
        "QPushButton:disabled { background: #cccccc; }"
    );
    connect(m_btnNext, &QPushButton::clicked, this, &EBookReaderWidget::onNextPage);
    
    m_btnLast = new QPushButton(QStringLiteral("▶|"));
    m_btnLast->setFixedSize(40, 32);  // 缩小尺寸
    m_btnLast->setStyleSheet(
        "QPushButton { background: #4a90e2; color: white; border: none; "
        "border-radius: 5px; font-weight: bold; font-size: 11pt; }"
        "QPushButton:hover { background: #2b7bb9; }"
        "QPushButton:disabled { background: #cccccc; }"
    );
    m_btnLast->setToolTip(QStringLiteral("最后一页"));
    connect(m_btnLast, &QPushButton::clicked, this, &EBookReaderWidget::onLastPage);
    
    navLayout->addWidget(m_btnFirst);
    navLayout->addWidget(m_btnPrev);
    navLayout->addSpacing(20);  // 中间留20px空白
    navLayout->addWidget(m_btnNext);
    navLayout->addWidget(m_btnLast);
    
    bottomLayout->addLayout(navLayout);
    mainLayout->addLayout(bottomLayout);
    
    setLayout(mainLayout);
}

EBookReaderWidget::~EBookReaderWidget()
{
    delete ui;
}

void EBookReaderWidget::loadPDFPages(const QString& title, const QString& author, 
                                     const QString& pagesDir, const QString& pagePrefix, 
                                     int pageCount)
{
    m_bookTitle = title;
    m_bookAuthor = author;
    m_pagesDir = pagesDir;
    m_pagePrefix = pagePrefix;
    m_totalPages = pageCount;
    m_currentPage = 1;
    
    qDebug() << "[EBookReaderWidget] Loading PDF pages:";
    qDebug() << "  Title:" << title;
    qDebug() << "  Author:" << author;
    qDebug() << "  Pages dir:" << pagesDir;
    qDebug() << "  Page prefix:" << pagePrefix;
    qDebug() << "  Total pages:" << pageCount;
    
    // 更新标题
    QString titleText = title;
    if (!author.isEmpty()) {
        titleText += QStringLiteral(" - ") + author;
    }
    m_titleLabel->setText(titleText);
    
    // 加载第一页
    loadPage(1);
}

void EBookReaderWidget::loadPage(int pageNum)
{
    if (pageNum < 1 || pageNum > m_totalPages) {
        qWarning() << "[EBookReaderWidget] Invalid page number:" << pageNum;
        return;
    }
    
    m_currentPage = pageNum;
    
    // 尝试多个可能的路径
    QStringList possiblePaths;
    QString filename = QString("%1%2.png").arg(m_pagePrefix).arg(pageNum);
    
    possiblePaths << m_pagesDir + "/" + filename
                  << "./" + m_pagesDir + "/" + filename
                  << "../" + m_pagesDir + "/" + filename
                  << QCoreApplication::applicationDirPath() + "/" + m_pagesDir + "/" + filename;
    
    QString imagePath;
    for (const QString& path : possiblePaths) {
        if (QFile::exists(path)) {
            imagePath = path;
            qDebug() << "[EBookReaderWidget] Found page image at:" << path;
            break;
        }
    }
    
    if (imagePath.isEmpty()) {
        qWarning() << "[EBookReaderWidget] Page image not found. Tried paths:";
        for (const QString& path : possiblePaths) {
            qWarning() << "  " << path;
        }
        
        // 显示错误信息
        m_pageDisplay->setText(QStringLiteral("❌ 无法加载第 %1 页\n\n"
                                              "图片文件不存在：\n%2\n\n"
                                              "请运行 pdf_to_images.py 生成页面图片")
                              .arg(pageNum)
                              .arg(filename));
        m_pageDisplay->setStyleSheet("QLabel { background: white; color: red; "
                                    "padding: 20px; font-size: 12pt; }");
        updatePageDisplay();
        return;
    }
    
    // 加载图片
    QPixmap pixmap(imagePath);
    if (pixmap.isNull()) {
        qWarning() << "[EBookReaderWidget] Failed to load image:" << imagePath;
        m_pageDisplay->setText(QStringLiteral("❌ 图片加载失败"));
        m_pageDisplay->setStyleSheet("QLabel { background: white; color: red; }");
    } else {
        // 限制最大宽度为380px（400px窗口 - 20px边距）
        int maxWidth = 380;
        int maxHeight = m_scrollArea->viewport()->height() - 20;
        
        // 缩放图片以适应窗口，保持宽高比
        QPixmap scaledPixmap = pixmap.scaled(
            maxWidth,
            maxHeight,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        );
        
        m_pageDisplay->setPixmap(scaledPixmap);
        // 不设置固定大小，让QLabel自动调整，配合容器布局实现居中
        m_pageDisplay->adjustSize();
        
        qDebug() << "[EBookReaderWidget] Image scaled to:" << scaledPixmap.size()
                 << "Max width:" << maxWidth;
        m_pageDisplay->setStyleSheet("QLabel { background: white; border: 1px solid #ccc; }");
        
        qDebug() << "[EBookReaderWidget] Loaded page" << pageNum
                 << "Original size:" << pixmap.size()
                 << "Scaled size:" << scaledPixmap.size();
    }
    
    updatePageDisplay();
}

void EBookReaderWidget::updatePageDisplay()
{
    // 更新页码显示
    m_pageLabel->setText(QStringLiteral("第 %1 页 / 共 %2 页")
                        .arg(m_currentPage)
                        .arg(m_totalPages));
    
    // 更新按钮状态
    updateNavigationButtons();
}

void EBookReaderWidget::updateNavigationButtons()
{
    m_btnFirst->setEnabled(m_currentPage > 1);
    m_btnPrev->setEnabled(m_currentPage > 1);
    m_btnNext->setEnabled(m_currentPage < m_totalPages);
    m_btnLast->setEnabled(m_currentPage < m_totalPages);
}

void EBookReaderWidget::onBackToShelf()
{
    qDebug() << "[EBookReaderWidget] Back to shelf button clicked";
    emit backRequested();
}

void EBookReaderWidget::onPreviousPage()
{
    if (m_currentPage > 1) {
        loadPage(m_currentPage - 1);
        qDebug() << "[EBookReaderWidget] Previous page:" << m_currentPage;
    }
}

void EBookReaderWidget::onNextPage()
{
    if (m_currentPage < m_totalPages) {
        loadPage(m_currentPage + 1);
        qDebug() << "[EBookReaderWidget] Next page:" << m_currentPage;
    }
}

void EBookReaderWidget::onFirstPage()
{
    if (m_currentPage != 1) {
        loadPage(1);
        qDebug() << "[EBookReaderWidget] Jump to first page";
    }
}

void EBookReaderWidget::onLastPage()
{
    if (m_currentPage != m_totalPages) {
        loadPage(m_totalPages);
        qDebug() << "[EBookReaderWidget] Jump to last page";
    }
}
