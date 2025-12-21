/********************************************************************************
** Form generated from reading UI file 'settingspage.ui'
**
** Created by: Qt User Interface Compiler version 5.9.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSPAGE_H
#define UI_SETTINGSPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsPage
{
public:
    QVBoxLayout *verticalLayout_7;
    QFrame *frameSets;
    QVBoxLayout *verticalLayout_6;
    QFrame *frameHomeTitle;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelLogo;
    QLabel *labelTitle;
    QSpacerItem *horizontalSpacer;
    QFrame *frameSet;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupProfile;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_2;
    QLabel *labelUser;
    QHBoxLayout *horizontalLayoutAvatar;
    QSpacerItem *horizontalSpacer_2;
    QLabel *labelAvatar;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *btnChooseAvatar;
    QSpacerItem *horizontalSpacer_5;
    QGroupBox *groupAccount;
    QVBoxLayout *verticalLayout_3;
    QLabel *label;
    QCheckBox *checkPhoneBind;
    QPushButton *btnChangePassword;
    QPushButton *btnLogout;
    QGroupBox *groupNotify;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_3;
    QCheckBox *checkMessageNotify;
    QCheckBox *checkSilent;
    QGroupBox *groupAbout;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_4;
    QLabel *labelVersion;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *SettingsPage)
    {
        if (SettingsPage->objectName().isEmpty())
            SettingsPage->setObjectName(QStringLiteral("SettingsPage"));
        SettingsPage->resize(400, 720);
        SettingsPage->setStyleSheet(QString::fromUtf8("*\n"
"{\n"
"    font-family: \"Microsoft YaHei\";\n"
"    font-size: 12px;\n"
"    color: rgb(30, 30, 30); /* \344\270\273\346\226\207\345\255\227\350\211\262 */\n"
"}\n"
"QPushButton\n"
"{\n"
"background-color: #07c160; \n"
"color: white; \n"
"border: none; \n"
"border-radius: 8px;\n"
"font-size: 14px;\n"
"font-weight: bold; \n"
"}\n"
"\n"
"QPushButton:hover { background-color: #06ad56; }\n"
"\n"
"QFrame#frameHomeTitle\n"
"{\n"
"	background-color: rgb(255, 255, 255);\n"
"	border-top-left-radius:12px;\n"
"	border-top-right-radius:12px;\n"
"}\n"
"QFrame#frameSets\n"
"{\n"
"	background-color: rgb(244, 245, 246);\n"
"		border-top-left-radius:12px;\n"
"	border-top-right-radius:12px;\n"
"}\n"
"\n"
"QFrame#frameSet\n"
"{\n"
"	background-color: rgb(255, 255, 255);\n"
"}\n"
"\n"
"QGroupBox \n"
"{\n"
"    border: 1px solid rgb(225, 230, 234);\n"
"    border-radius: 8px;\n"
"    background-color: rgba(255, 255, 255, 0.7);\n"
"}\n"
"\n"
"QCheckBox {\n"
"    spacing: 5px;\n"
"    padding: 3px 0;\n"
"}\n"
"\n"
"QCheckBox::"
                        "indicator {\n"
"    width: 16px;\n"
"    height: 16px;\n"
"    border: 1px solid rgb(225, 230, 234);\n"
"    border-radius: 3px;\n"
"    background-color: white;\n"
"}\n"
"\n"
"QCheckBox::indicator:hover {\n"
"    border-color: rgb(210, 215, 220);\n"
"}\n"
"\n"
"QCheckBox::indicator:checked {\n"
"    image: url(:/image/check.png);\n"
"}\n"
"QLabel#labelAvatar\n"
"{\n"
"	border: none; \n"
"	border-image:url(:/head_portrait/head_portrait/0.png);\n"
"}\n"
"QLabel#labelLogo\n"
"{\n"
"	border-image:url(:/image/logo.png);\n"
"}\n"
""));
        verticalLayout_7 = new QVBoxLayout(SettingsPage);
        verticalLayout_7->setSpacing(0);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        verticalLayout_7->setContentsMargins(0, 0, 0, 0);
        frameSets = new QFrame(SettingsPage);
        frameSets->setObjectName(QStringLiteral("frameSets"));
        frameSets->setFrameShape(QFrame::StyledPanel);
        frameSets->setFrameShadow(QFrame::Raised);
        verticalLayout_6 = new QVBoxLayout(frameSets);
        verticalLayout_6->setSpacing(0);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        frameHomeTitle = new QFrame(frameSets);
        frameHomeTitle->setObjectName(QStringLiteral("frameHomeTitle"));
        frameHomeTitle->setMaximumSize(QSize(16777215, 40));
        frameHomeTitle->setFrameShape(QFrame::StyledPanel);
        frameHomeTitle->setFrameShadow(QFrame::Raised);
        horizontalLayout_2 = new QHBoxLayout(frameHomeTitle);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(12, -1, 12, -1);
        labelLogo = new QLabel(frameHomeTitle);
        labelLogo->setObjectName(QStringLiteral("labelLogo"));
        labelLogo->setMinimumSize(QSize(25, 25));
        labelLogo->setMaximumSize(QSize(25, 25));

        horizontalLayout_2->addWidget(labelLogo);

        labelTitle = new QLabel(frameHomeTitle);
        labelTitle->setObjectName(QStringLiteral("labelTitle"));
        QFont font;
        font.setFamily(QStringLiteral("Microsoft YaHei"));
        font.setBold(false);
        font.setWeight(50);
        labelTitle->setFont(font);

        horizontalLayout_2->addWidget(labelTitle);

        horizontalSpacer = new QSpacerItem(309, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout_6->addWidget(frameHomeTitle);

        frameSet = new QFrame(frameSets);
        frameSet->setObjectName(QStringLiteral("frameSet"));
        frameSet->setFrameShape(QFrame::StyledPanel);
        frameSet->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(frameSet);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        groupProfile = new QGroupBox(frameSet);
        groupProfile->setObjectName(QStringLiteral("groupProfile"));
        verticalLayout_2 = new QVBoxLayout(groupProfile);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(20, -1, 20, -1);
        label_2 = new QLabel(groupProfile);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setMaximumSize(QSize(16777215, 20));

        verticalLayout_2->addWidget(label_2);

        labelUser = new QLabel(groupProfile);
        labelUser->setObjectName(QStringLiteral("labelUser"));
        labelUser->setWordWrap(true);

        verticalLayout_2->addWidget(labelUser);

        horizontalLayoutAvatar = new QHBoxLayout();
        horizontalLayoutAvatar->setObjectName(QStringLiteral("horizontalLayoutAvatar"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayoutAvatar->addItem(horizontalSpacer_2);

        labelAvatar = new QLabel(groupProfile);
        labelAvatar->setObjectName(QStringLiteral("labelAvatar"));
        labelAvatar->setMinimumSize(QSize(80, 80));
        labelAvatar->setMaximumSize(QSize(96, 96));
        labelAvatar->setFrameShape(QFrame::Box);
        labelAvatar->setAlignment(Qt::AlignCenter);

        horizontalLayoutAvatar->addWidget(labelAvatar);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayoutAvatar->addItem(horizontalSpacer_3);


        verticalLayout_2->addLayout(horizontalLayoutAvatar);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);

        btnChooseAvatar = new QPushButton(groupProfile);
        btnChooseAvatar->setObjectName(QStringLiteral("btnChooseAvatar"));
        btnChooseAvatar->setMinimumSize(QSize(100, 30));
        btnChooseAvatar->setMaximumSize(QSize(16777215, 30));

        horizontalLayout->addWidget(btnChooseAvatar);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_5);


        verticalLayout_2->addLayout(horizontalLayout);


        verticalLayout->addWidget(groupProfile);

        groupAccount = new QGroupBox(frameSet);
        groupAccount->setObjectName(QStringLiteral("groupAccount"));
        verticalLayout_3 = new QVBoxLayout(groupAccount);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(20, -1, 20, -1);
        label = new QLabel(groupAccount);
        label->setObjectName(QStringLiteral("label"));
        label->setMaximumSize(QSize(16777215, 20));

        verticalLayout_3->addWidget(label);

        checkPhoneBind = new QCheckBox(groupAccount);
        checkPhoneBind->setObjectName(QStringLiteral("checkPhoneBind"));
        checkPhoneBind->setChecked(true);

        verticalLayout_3->addWidget(checkPhoneBind);

        btnChangePassword = new QPushButton(groupAccount);
        btnChangePassword->setObjectName(QStringLiteral("btnChangePassword"));
        btnChangePassword->setMinimumSize(QSize(0, 30));
        btnChangePassword->setMaximumSize(QSize(16777215, 30));

        verticalLayout_3->addWidget(btnChangePassword);

        btnLogout = new QPushButton(groupAccount);
        btnLogout->setObjectName(QStringLiteral("btnLogout"));
        btnLogout->setMinimumSize(QSize(0, 30));
        btnLogout->setMaximumSize(QSize(16777215, 30));
        btnLogout->setStyleSheet(QLatin1String("QPushButton {\n"
"    background-color: #f44336;\n"
"    color: white;\n"
"    border: none;\n"
"    padding: 8px;\n"
"    border-radius: 6px;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #d32f2f;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: #b71c1c;\n"
"}"));

        verticalLayout_3->addWidget(btnLogout);


        verticalLayout->addWidget(groupAccount);

        groupNotify = new QGroupBox(frameSet);
        groupNotify->setObjectName(QStringLiteral("groupNotify"));
        verticalLayout_4 = new QVBoxLayout(groupNotify);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(20, 9, 20, 9);
        label_3 = new QLabel(groupNotify);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setMaximumSize(QSize(16777215, 20));

        verticalLayout_4->addWidget(label_3);

        checkMessageNotify = new QCheckBox(groupNotify);
        checkMessageNotify->setObjectName(QStringLiteral("checkMessageNotify"));
        checkMessageNotify->setChecked(true);

        verticalLayout_4->addWidget(checkMessageNotify);

        checkSilent = new QCheckBox(groupNotify);
        checkSilent->setObjectName(QStringLiteral("checkSilent"));

        verticalLayout_4->addWidget(checkSilent);


        verticalLayout->addWidget(groupNotify);

        groupAbout = new QGroupBox(frameSet);
        groupAbout->setObjectName(QStringLiteral("groupAbout"));
        verticalLayout_5 = new QVBoxLayout(groupAbout);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(20, -1, 20, -1);
        label_4 = new QLabel(groupAbout);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setMaximumSize(QSize(16777215, 20));

        verticalLayout_5->addWidget(label_4);

        labelVersion = new QLabel(groupAbout);
        labelVersion->setObjectName(QStringLiteral("labelVersion"));
        labelVersion->setWordWrap(true);

        verticalLayout_5->addWidget(labelVersion);


        verticalLayout->addWidget(groupAbout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        verticalLayout_6->addWidget(frameSet);


        verticalLayout_7->addWidget(frameSets);


        retranslateUi(SettingsPage);

        QMetaObject::connectSlotsByName(SettingsPage);
    } // setupUi

    void retranslateUi(QWidget *SettingsPage)
    {
        SettingsPage->setWindowTitle(QApplication::translate("SettingsPage", "\350\256\276\347\275\256", Q_NULLPTR));
        labelLogo->setText(QString());
        labelTitle->setText(QApplication::translate("SettingsPage", "\350\256\276\347\275\256", Q_NULLPTR));
        groupProfile->setTitle(QString());
        label_2->setText(QApplication::translate("SettingsPage", "Personal Info", Q_NULLPTR));
        labelUser->setText(QApplication::translate("SettingsPage", "User \302\267 ID:", Q_NULLPTR));
        labelAvatar->setText(QString());
        btnChooseAvatar->setText(QApplication::translate("SettingsPage", "\344\277\256\346\224\271\345\244\264\345\203\217", Q_NULLPTR));
        groupAccount->setTitle(QString());
        label->setText(QApplication::translate("SettingsPage", "\350\264\246\345\217\267\350\256\276\347\275\256", Q_NULLPTR));
        checkPhoneBind->setText(QApplication::translate("SettingsPage", "\345\267\262\347\273\221\345\256\232\346\211\213\346\234\272\345\217\267", Q_NULLPTR));
        btnChangePassword->setText(QApplication::translate("SettingsPage", "\344\277\256\346\224\271\345\257\206\347\240\201", Q_NULLPTR));
        btnLogout->setText(QApplication::translate("SettingsPage", "\351\200\200\345\207\272\347\231\273\345\275\225", Q_NULLPTR));
        groupNotify->setTitle(QString());
        label_3->setText(QApplication::translate("SettingsPage", "\351\200\232\347\237\245\344\270\216\346\217\220\351\206\222", Q_NULLPTR));
        checkMessageNotify->setText(QApplication::translate("SettingsPage", "\346\266\210\346\201\257\346\217\220\351\206\222", Q_NULLPTR));
        checkSilent->setText(QApplication::translate("SettingsPage", "\346\231\232\344\270\212\345\205\215\346\211\223\346\211\260\346\250\241\345\274\217", Q_NULLPTR));
        groupAbout->setTitle(QString());
        label_4->setText(QApplication::translate("SettingsPage", "\345\205\263\344\272\216", Q_NULLPTR));
        labelVersion->setText(QApplication::translate("SettingsPage", "\345\277\203\345\261\277 \302\267 \345\277\203\347\220\206\345\201\245\345\272\267\346\224\257\346\214\201\345\271\263\345\217\260\357\274\210\346\241\214\351\235\242\345\216\237\345\236\213\357\274\211", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class SettingsPage: public Ui_SettingsPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSPAGE_H
