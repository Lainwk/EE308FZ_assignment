#include "settingspage.h"
#include "ui_settingspage.h"
#include "reminderservice.h"
#include "psychotipsmanager.h"
#include <QDir>
#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QCoreApplication>
#include <QFileInfo>
#include <QtGlobal>
#include <QCheckBox>

SettingsPage::SettingsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsPage)
{
    ui->setupUi(this);

    m_labelAvatar = this->findChild<QLabel*>("labelAvatar");
    m_btnChooseAvatar = this->findChild<QPushButton*>("btnChooseAvatar");
    // 若 UI 编译器未生成头像控件，运行时补建
    if (!m_labelAvatar || !m_btnChooseAvatar) {
        auto vbox = this->findChild<QVBoxLayout*>("verticalLayout_2");
        if (vbox) {
            auto hbox = new QHBoxLayout();
            hbox->setObjectName("horizontalLayoutAvatar_runtime");

            m_labelAvatar = new QLabel(this);
            m_labelAvatar->setObjectName("labelAvatar");
            m_labelAvatar->setMinimumSize(QSize(80, 80));
            m_labelAvatar->setMaximumSize(QSize(96, 96));
            m_labelAvatar->setFrameShape(QFrame::Box);
            m_labelAvatar->setAlignment(Qt::AlignCenter);
            m_labelAvatar->setText(tr("Avatar"));
            m_labelAvatar->setScaledContents(true);
            hbox->addWidget(m_labelAvatar);

            m_btnChooseAvatar = new QPushButton(tr("Choose Avatar"), this);
            m_btnChooseAvatar->setObjectName("btnChooseAvatar");
            hbox->addWidget(m_btnChooseAvatar);

            vbox->addLayout(hbox);
        }
    } else {
        m_labelAvatar->setScaledContents(true);
    }

    loadAvatarPaths();
    refreshSession();

    if (m_btnChooseAvatar) {
        connect(m_btnChooseAvatar, &QPushButton::clicked,
                this, &SettingsPage::onChooseAvatar);
    }

    // 连接修改密码按钮
    QPushButton* btnChangePassword = this->findChild<QPushButton*>("btnChangePassword");
    if (btnChangePassword) {
        connect(btnChangePassword, &QPushButton::clicked,
                this, &SettingsPage::onChangePasswordClicked);
    }

    // 连接退出登录按钮
    QPushButton* btnLogout = this->findChild<QPushButton*>("btnLogout");
    if (btnLogout) {
        connect(btnLogout, &QPushButton::clicked,
                this, &SettingsPage::onLogoutClicked);
    }
    
    // 连接每日测评提醒复选框
    QCheckBox* checkAssessmentReminder = this->findChild<QCheckBox*>("checkAssessmentReminder");
    if (checkAssessmentReminder) {
        // 从ReminderService读取当前状态
        checkAssessmentReminder->setChecked(ReminderService::instance().isEnabled());
        
        // 连接状态变化信号
        connect(checkAssessmentReminder, &QCheckBox::toggled, this, [](bool checked) {
            ReminderService::instance().setEnabled(checked);
            qDebug() << "[SettingsPage] Assessment reminder" << (checked ? "enabled" : "disabled");
        });
    }
    
    // 连接心理小贴士推送复选框
    QCheckBox* checkPsychoTips = this->findChild<QCheckBox*>("checkPsychoTips");
    if (checkPsychoTips) {
        // 从PsychoTipsManager读取当前状态
        checkPsychoTips->setChecked(PsychoTipsManager::instance().isEnabled());
        
        // 连接状态变化信号
        connect(checkPsychoTips, &QCheckBox::toggled, this, [](bool checked) {
            PsychoTipsManager::instance().setEnabled(checked);
            qDebug() << "[SettingsPage] Psycho tips push" << (checked ? "enabled" : "disabled");
        });
    }
}

SettingsPage::~SettingsPage()
{
    delete ui;
}

void SettingsPage::setSession(const SessionData& session)
{
    m_session = session;
    m_avatarIndex = qMax(0, session.avatarIndex);
    if (m_avatarIndex >= m_avatarPaths.size()) {
        m_avatarIndex = 0;
    }

    QString displayName = m_session.userName.isEmpty() ? tr("用户") : m_session.userName;
    QString displayId = m_session.userId.isEmpty() ? tr("未登录") : m_session.userId;
    ui->labelUser->setText(QString("%1 · ID: %2").arg(displayName, displayId));
    updateAvatarDisplay();
}

void SettingsPage::refreshSession()
{
    if (m_session.userId.isEmpty() && m_session.userName.isEmpty()) {
        setSession(LocalStore::instance().loadSession());
    } else {
        setSession(m_session);
    }
}

void SettingsPage::loadAvatarPaths()
{
    m_avatarPaths.clear();
    // Prefer Qt resources
    QDir resDir(":/head_portrait");
    if (resDir.exists()) {
        const QStringList files = resDir.entryList(QStringList() << "*.png" << "*.jpg" << "*.jpeg", QDir::Files, QDir::Name);
        for (const QString& f : files) {
            m_avatarPaths.append(":/head_portrait/" + f);
        }
    }

    // Fallback to filesystem if resources missing (dev mode)
    if (m_avatarPaths.isEmpty()) {
        QString appDir = QCoreApplication::applicationDirPath();
        QStringList candidates;
        candidates << (appDir + "/head_portrait");
        candidates << QDir(appDir).absoluteFilePath("../Client_Code/head_portrait");
        candidates << QStringLiteral("E:/ProgramSoftWare/Project/MindCarrer_All-vs/Client_Code/head_portrait");

        for (const QString& dirPath : candidates) {
            QDir dir(dirPath);
            if (!dir.exists())
                continue;
            QStringList files = dir.entryList(QStringList() << "*.png" << "*.jpg" << "*.jpeg", QDir::Files, QDir::Name);
            for (const QString& f : files) {
                const QString abs = dir.absoluteFilePath(f);
                if (!m_avatarPaths.contains(abs)) {
                    m_avatarPaths.append(abs);
                }
            }
            if (!m_avatarPaths.isEmpty()) break;
        }
    }
}

QString SettingsPage::resolveAvatarDir() const
{
    if (m_avatarPaths.isEmpty()) return QString();
    QFileInfo fi(m_avatarPaths.first());
    return fi.absolutePath();
}

void SettingsPage::updateAvatarDisplay()
{
    if (m_avatarPaths.isEmpty()) {
        if (m_labelAvatar) {
            m_labelAvatar->setText(tr("No avatar"));
            m_labelAvatar->setPixmap(QPixmap());
        }
        return;
    }
    if (m_avatarIndex < 0 || m_avatarIndex >= m_avatarPaths.size()) {
        m_avatarIndex = 0;
    }
    QPixmap pix(m_avatarPaths.at(m_avatarIndex));
    if (!pix.isNull()) {
        if (m_labelAvatar) {
            QSize target = m_labelAvatar->size();
            m_labelAvatar->setPixmap(pix.scaled(target, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    } else if (m_labelAvatar) {
        m_labelAvatar->setText(tr("Broken avatar"));
    }
}

void SettingsPage::onChooseAvatar()
{
    if (m_avatarPaths.isEmpty()) {
        loadAvatarPaths();
    }
    if (m_avatarPaths.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("未找到头像文件夹，请确认 head_portrait 目录存在。"));
        return;
    }

    int chosen = -1;
    QDialog dlg(this);
    dlg.setWindowTitle(tr("选择头像"));
    QGridLayout* layout = new QGridLayout(&dlg);
    layout->setSpacing(8);
    layout->setContentsMargins(12, 12, 12, 12);

    const int columnCount = 4;
    for (int i = 0; i < m_avatarPaths.size(); ++i) {
        QPixmap pix(m_avatarPaths.at(i));
        auto *btn = new QPushButton(&dlg);
        btn->setCheckable(true);
        btn->setAutoExclusive(true);
        btn->setFixedSize(90, 90);
        btn->setIconSize(QSize(72, 72));
        btn->setIcon(pix.scaled(btn->iconSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        btn->setStyleSheet("QPushButton { border: 2px solid #e0e0e0; border-radius: 8px; background: #fff; }"
                           "QPushButton:checked { border-color: #07C160; box-shadow: 0 0 6px #07C160; }");
        if (i == m_avatarIndex) btn->setChecked(true);
        connect(btn, &QPushButton::clicked, this, [&, i]() {
            chosen = i;
            dlg.accept();
        });
        int row = i / columnCount;
        int col = i % columnCount;
        layout->addWidget(btn, row, col);
    }

    if (dlg.exec() == QDialog::Accepted && chosen >= 0) {
        m_avatarIndex = chosen;
        m_session.avatarIndex = chosen;
        LocalStore::instance().saveSession(m_session);
        updateAvatarDisplay();
    }
}

void SettingsPage::onChangePasswordClicked()
{
    if (m_session.userId.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请先登录"));
        return;
    }

    // 发射信号通知 MainWindow 切换到修改密码页面
    emit showChangePasswordPage();
}

void SettingsPage::onLogoutClicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("退出登录"),
                                  tr("确定要退出登录吗？"),
                                  QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // 发射信号通知 MainWindow 处理退出登录
        emit logoutRequested();
    }
}
