#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "homepage.h"
#include "contactspage.h"
#include "channelspage.h"
#include "settingspage.h"
#include "changepasswordpage.h"
#include "chatpage.h"
#include "authwindow.h"
#include "localstore.h"
#include <QTimer>
#include <QButtonGroup>
#include <QMouseEvent>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_homePage(nullptr),
    m_contactsPage(nullptr),
    m_channelsPage(nullptr),
    m_settingsPage(nullptr),
    m_changePasswordPage(nullptr),
    m_chatPage(nullptr)
{
    ui->setupUi(this);
    // Drag support for frameless window even when clicking child widgets
    qApp->installEventFilter(this);
    
    // 强制设置固定宽度，防止子页面内容撑大窗口
    setFixedWidth(400);
    setMinimumHeight(600);
    
    setupPages();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupPages()
{
    QWidget *placeholder = ui->stackedWidget->widget(0);
    if (placeholder) {
        ui->stackedWidget->removeWidget(placeholder);
        placeholder->deleteLater();
    }

    m_homePage = new HomePage(this);
    m_contactsPage = new ContactsPage(this);
    m_channelsPage = new ChannelsPage(this);
    m_settingsPage = new SettingsPage(this);
    m_changePasswordPage = new ChangePasswordPage(this);
    m_chatPage = new ChatPage(this);

    m_settingsPage->setSession(LocalStore::instance().loadSession());
    m_contactsPage->setSession(LocalStore::instance().loadSession());

    ui->stackedWidget->addWidget(m_homePage);
    ui->stackedWidget->addWidget(m_contactsPage);
    ui->stackedWidget->addWidget(m_channelsPage);
    ui->stackedWidget->addWidget(m_settingsPage);
    ui->stackedWidget->addWidget(m_changePasswordPage);
    ui->stackedWidget->addWidget(m_chatPage);

    // 连接设置页面的信号
    connect(m_settingsPage, &SettingsPage::showChangePasswordPage,
            this, &MainWindow::onShowChangePasswordPage);
    connect(m_settingsPage, &SettingsPage::logoutRequested,
            this, &MainWindow::onLogout);
    
    // 连接联系人页面的信号
    connect(m_contactsPage, &ContactsPage::showChatPage,
            this, &MainWindow::onShowChatPage);
    
    // 连接修改密码页面的信号
    connect(m_changePasswordPage, &ChangePasswordPage::backToSettings,
            this, &MainWindow::onBackToSettings);
    
    // 连接聊天页面的信号
    connect(m_chatPage, &ChatPage::backRequested,
            this, &MainWindow::onBackToContacts);

    ui->stackedWidget->setCurrentWidget(m_homePage);

    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(ui->btnHome);
    group->addButton(ui->btnChannels);
    group->addButton(ui->btnContacts);
    group->addButton(ui->btnMe);
    group->setExclusive(true);

    ui->btnHome->setChecked(true);

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    ui->centralwidget->setAttribute(Qt::WA_TranslucentBackground);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // Intercept mouse events from any widget that belongs to this window
    if (event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::MouseMove ||
        event->type() == QEvent::MouseButtonRelease) {
        QWidget *target = qobject_cast<QWidget*>(obj);
        if (target && target->window() == this) {
            if (event->type() == QEvent::MouseButtonPress) {
                auto *e = static_cast<QMouseEvent*>(event);
                if (e->button() == Qt::LeftButton) {
                    m_dragging = true;
                    m_dragStartPos = e->globalPos() - frameGeometry().topLeft();
                }
            } else if (event->type() == QEvent::MouseMove) {
                auto *e = static_cast<QMouseEvent*>(event);
                if (m_dragging && (e->buttons() & Qt::LeftButton)) {
                    move(e->globalPos() - m_dragStartPos);
                }
            } else if (event->type() == QEvent::MouseButtonRelease) {
                auto *e = static_cast<QMouseEvent*>(event);
                if (e->button() == Qt::LeftButton) {
                    m_dragging = false;
                }
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragStartPos = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPos() - m_dragStartPos);
        event->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        event->accept();
    }
}

void MainWindow::on_btnHome_clicked()
{
    if (m_homePage)
        ui->stackedWidget->setCurrentWidget(m_homePage);

    ui->lbHome->setStyleSheet("color:#2FC868;");
    ui->lbContacts->setStyleSheet("color:#B5BAC3;");
    ui->lbChannels->setStyleSheet("color:#B5BAC3;");
    ui->lbMe->setStyleSheet("color:#B5BAC3;");
}

void MainWindow::on_btnContacts_clicked()
{
    if (m_contactsPage) {
        m_contactsPage->refreshData(); // 每次进入联系人页刷新好友列表/请求
        ui->stackedWidget->setCurrentWidget(m_contactsPage);
    }

    ui->lbContacts->setStyleSheet("color:#2FC868;");
    ui->lbHome->setStyleSheet("color:#B5BAC3;");
    ui->lbChannels->setStyleSheet("color:#B5BAC3;");
    ui->lbMe->setStyleSheet("color:#B5BAC3;");
}

void MainWindow::on_btnChannels_clicked()
{
    if (m_channelsPage)
        ui->stackedWidget->setCurrentWidget(m_channelsPage);

    ui->lbChannels->setStyleSheet("color:#2FC868;");
    ui->lbHome->setStyleSheet("color:#B5BAC3;");
    ui->lbMe->setStyleSheet("color:#B5BAC3;");
    ui->lbContacts->setStyleSheet("color:#B5BAC3;");
}

void MainWindow::on_btnMe_clicked()
{
    if (m_settingsPage) {
        m_settingsPage->refreshSession();
        ui->stackedWidget->setCurrentWidget(m_settingsPage);
    }

    ui->lbMe->setStyleSheet("color:#2FC868;");
    ui->lbHome->setStyleSheet("color:#B5BAC3;");
    ui->lbChannels->setStyleSheet("color:#B5BAC3;");
    ui->lbContacts->setStyleSheet("color:#B5BAC3;");
}

void MainWindow::setUserInfo(const QString& userId, const QString& userName)
{
    // 先保存session数据
    SessionData s = LocalStore::instance().loadSession();
    s.userId = userId;
    s.userName = userName;
    LocalStore::instance().saveSession(s);
    if (m_settingsPage) m_settingsPage->setSession(s);
    if (m_contactsPage) m_contactsPage->setSession(s);
    if (m_changePasswordPage) m_changePasswordPage->setUserId(userId);
    if (m_chatPage) m_chatPage->setSession(s);
    
    // 延迟加载HomePage数据，避免阻塞UI
    // 使用QTimer::singleShot在事件循环中异步执行
    if (m_homePage) {
        QTimer::singleShot(100, this, [this, userId, userName]() {
            m_homePage->setUserInfo(userId, userName);
        });
    }
}

void MainWindow::onShowChangePasswordPage()
{
    if (m_changePasswordPage) {
        // 更新用户ID
        SessionData s = LocalStore::instance().loadSession();
        m_changePasswordPage->setUserId(s.userId);
        
        // 切换到修改密码页面
        ui->stackedWidget->setCurrentWidget(m_changePasswordPage);
    }
}

void MainWindow::onShowChatPage(const QString& friendId, const QString& friendName, int friendAvatar, int selfAvatar)
{
    if (m_chatPage) {
        m_chatPage->setSession(LocalStore::instance().loadSession());
        m_chatPage->setChatInfo(friendId, friendName, friendAvatar, selfAvatar);
        ui->stackedWidget->setCurrentWidget(m_chatPage);
    }
}

void MainWindow::onBackToSettings()
{
    if (m_settingsPage) {
        m_settingsPage->refreshSession();
        ui->stackedWidget->setCurrentWidget(m_settingsPage);
    }
}

void MainWindow::onBackToContacts()
{
    if (m_contactsPage) {
        m_contactsPage->refreshData();
        ui->stackedWidget->setCurrentWidget(m_contactsPage);
    }
}

void MainWindow::onLogout()
{
    // 清除本地会话数据
    LocalStore::instance().clearSession();
    
    // 创建并显示登录窗口
    AuthWindow* authWin = new AuthWindow();
    authWin->show();
    
    // 关闭主窗口
    this->close();
    this->deleteLater();
}
