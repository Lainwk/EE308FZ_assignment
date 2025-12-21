/********************************************************************************
** Form generated from reading UI file 'authwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUTHWINDOW_H
#define UI_AUTHWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AuthWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QLabel *labelTitle;
    QStackedWidget *stackedWidget;
    QWidget *pageLogin;
    QVBoxLayout *verticalLayout_2;
    QLineEdit *editPhoneLogin;
    QLineEdit *editPwdLogin;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *editCodeLogin;
    QPushButton *btnSendCodeLogin;
    QCheckBox *checkRemember;
    QPushButton *btnSwitchLoginMode;
    QPushButton *btnDoLogin;
    QPushButton *btnSwitchRegister;
    QWidget *pageRegister;
    QVBoxLayout *verticalLayout_3;
    QLineEdit *editPhoneRegister;
    QLineEdit *editNameRegister;
    QLineEdit *editPwdRegister;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *editCodeRegister;
    QPushButton *btnSendCodeRegister;
    QPushButton *btnDoRegister;
    QPushButton *btnSwitchLogin;
    QSpacerItem *verticalSpacer;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *AuthWindow)
    {
        if (AuthWindow->objectName().isEmpty())
            AuthWindow->setObjectName(QStringLiteral("AuthWindow"));
        AuthWindow->resize(400, 800);
        AuthWindow->setStyleSheet(QString::fromUtf8("*\n"
"{\n"
"    font-family: \"Microsoft YaHei\";\n"
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
"QPushButton#btnBack\n"
"{\n"
"	background-color: transparent; \n"
"	border-image: url(:/image/return.png);\n"
"}\n"
"\n"
"QLineEdit \n"
"{\n"
"    background-color: #FFFFFF;\n"
"    color: rgb(30, 30, 30);\n"
"    border: 1px solid rgb(225, 230, 234);\n"
"    border-radius: 5px;\n"
"    padding: 5px 8px;  /* \346\257\224QComboBox\347\250\215\345\256\275\347\232\204\345\206\205\350\276\271\350\267\235 */\n"
"    min-height: 15px;\n"
"    selection-background-color: rgb(70, 130, 200);  /* \351\200\211\344\270\255\346\226\207\346\234\254\350\203\214\346\231\257\350\211\262 */\n"
"    selection-color: white;  /* \351\200\211\344\270\255\346\226\207\346\234\254\351\242\234\350\211\262 */\n"
"}\n"
"\n"
"\n"
"QCheckBox {\n"
"    spacing: 5px;\n"
"    padding: 3px 0;\n"
"}\n"
"\n"
"QCheckBo"
                        "x::indicator {\n"
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
"\n"
"QPushButton#btnSwitchRegister,#btnSwitchLogin\n"
"{\n"
"	background-color: transparent; \n"
"	 color: rgb(30, 30, 30);\n"
"}\n"
"\n"
""));
        centralwidget = new QWidget(AuthWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setSpacing(12);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(16, 16, 16, 16);
        labelTitle = new QLabel(centralwidget);
        labelTitle->setObjectName(QStringLiteral("labelTitle"));
        labelTitle->setStyleSheet(QStringLiteral("font-size:20px; font-weight:600;"));
        labelTitle->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(labelTitle);

        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName(QStringLiteral("stackedWidget"));
        pageLogin = new QWidget();
        pageLogin->setObjectName(QStringLiteral("pageLogin"));
        verticalLayout_2 = new QVBoxLayout(pageLogin);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        editPhoneLogin = new QLineEdit(pageLogin);
        editPhoneLogin->setObjectName(QStringLiteral("editPhoneLogin"));

        verticalLayout_2->addWidget(editPhoneLogin);

        editPwdLogin = new QLineEdit(pageLogin);
        editPwdLogin->setObjectName(QStringLiteral("editPwdLogin"));
        editPwdLogin->setEchoMode(QLineEdit::Password);

        verticalLayout_2->addWidget(editPwdLogin);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        editCodeLogin = new QLineEdit(pageLogin);
        editCodeLogin->setObjectName(QStringLiteral("editCodeLogin"));

        horizontalLayout_2->addWidget(editCodeLogin);

        btnSendCodeLogin = new QPushButton(pageLogin);
        btnSendCodeLogin->setObjectName(QStringLiteral("btnSendCodeLogin"));
        btnSendCodeLogin->setMinimumSize(QSize(100, 25));
        btnSendCodeLogin->setMaximumSize(QSize(100, 25));

        horizontalLayout_2->addWidget(btnSendCodeLogin);


        verticalLayout_2->addLayout(horizontalLayout_2);

        checkRemember = new QCheckBox(pageLogin);
        checkRemember->setObjectName(QStringLiteral("checkRemember"));

        verticalLayout_2->addWidget(checkRemember);

        btnSwitchLoginMode = new QPushButton(pageLogin);
        btnSwitchLoginMode->setObjectName(QStringLiteral("btnSwitchLoginMode"));
        btnSwitchLoginMode->setStyleSheet(QStringLiteral("background:#f0f0f0; color:#576b95; padding:8px; border-radius:6px;"));
        btnSwitchLoginMode->setFlat(false);

        verticalLayout_2->addWidget(btnSwitchLoginMode);

        btnDoLogin = new QPushButton(pageLogin);
        btnDoLogin->setObjectName(QStringLiteral("btnDoLogin"));
        btnDoLogin->setStyleSheet(QStringLiteral("background:#07C160; color:white; padding:10px; border-radius:8px; font-weight:600;"));

        verticalLayout_2->addWidget(btnDoLogin);

        btnSwitchRegister = new QPushButton(pageLogin);
        btnSwitchRegister->setObjectName(QStringLiteral("btnSwitchRegister"));
        QFont font;
        font.setFamily(QStringLiteral("Microsoft YaHei"));
        font.setBold(true);
        font.setWeight(75);
        btnSwitchRegister->setFont(font);
        btnSwitchRegister->setFlat(true);

        verticalLayout_2->addWidget(btnSwitchRegister);

        stackedWidget->addWidget(pageLogin);
        pageRegister = new QWidget();
        pageRegister->setObjectName(QStringLiteral("pageRegister"));
        verticalLayout_3 = new QVBoxLayout(pageRegister);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        editPhoneRegister = new QLineEdit(pageRegister);
        editPhoneRegister->setObjectName(QStringLiteral("editPhoneRegister"));

        verticalLayout_3->addWidget(editPhoneRegister);

        editNameRegister = new QLineEdit(pageRegister);
        editNameRegister->setObjectName(QStringLiteral("editNameRegister"));

        verticalLayout_3->addWidget(editNameRegister);

        editPwdRegister = new QLineEdit(pageRegister);
        editPwdRegister->setObjectName(QStringLiteral("editPwdRegister"));
        editPwdRegister->setEchoMode(QLineEdit::Password);

        verticalLayout_3->addWidget(editPwdRegister);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        editCodeRegister = new QLineEdit(pageRegister);
        editCodeRegister->setObjectName(QStringLiteral("editCodeRegister"));

        horizontalLayout_3->addWidget(editCodeRegister);

        btnSendCodeRegister = new QPushButton(pageRegister);
        btnSendCodeRegister->setObjectName(QStringLiteral("btnSendCodeRegister"));
        btnSendCodeRegister->setMinimumSize(QSize(100, 25));
        btnSendCodeRegister->setMaximumSize(QSize(100, 25));

        horizontalLayout_3->addWidget(btnSendCodeRegister);


        verticalLayout_3->addLayout(horizontalLayout_3);

        btnDoRegister = new QPushButton(pageRegister);
        btnDoRegister->setObjectName(QStringLiteral("btnDoRegister"));
        btnDoRegister->setStyleSheet(QStringLiteral("background:#07C160; color:white; padding:10px; border-radius:8px; font-weight:600;"));

        verticalLayout_3->addWidget(btnDoRegister);

        btnSwitchLogin = new QPushButton(pageRegister);
        btnSwitchLogin->setObjectName(QStringLiteral("btnSwitchLogin"));
        btnSwitchLogin->setFlat(true);

        verticalLayout_3->addWidget(btnSwitchLogin);

        stackedWidget->addWidget(pageRegister);

        verticalLayout->addWidget(stackedWidget);

        verticalSpacer = new QSpacerItem(20, 200, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        AuthWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(AuthWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 400, 22));
        AuthWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(AuthWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        AuthWindow->setStatusBar(statusbar);

        retranslateUi(AuthWindow);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(AuthWindow);
    } // setupUi

    void retranslateUi(QMainWindow *AuthWindow)
    {
        AuthWindow->setWindowTitle(QApplication::translate("AuthWindow", "\345\277\203\345\261\277 \302\267 \347\231\273\345\275\225/\346\263\250\345\206\214", Q_NULLPTR));
        labelTitle->setText(QApplication::translate("AuthWindow", "\346\254\242\350\277\216\346\235\245\345\210\260\345\277\203\345\261\277", Q_NULLPTR));
        editPhoneLogin->setPlaceholderText(QApplication::translate("AuthWindow", "\346\211\213\346\234\272\345\217\267", Q_NULLPTR));
        editPwdLogin->setPlaceholderText(QApplication::translate("AuthWindow", "\345\257\206\347\240\201", Q_NULLPTR));
        editCodeLogin->setPlaceholderText(QApplication::translate("AuthWindow", "\351\252\214\350\257\201\347\240\201", Q_NULLPTR));
        btnSendCodeLogin->setText(QApplication::translate("AuthWindow", "\350\216\267\345\217\226\351\252\214\350\257\201\347\240\201", Q_NULLPTR));
        checkRemember->setText(QApplication::translate("AuthWindow", "\350\256\260\344\275\217\345\257\206\347\240\201", Q_NULLPTR));
        btnSwitchLoginMode->setText(QApplication::translate("AuthWindow", "\345\210\207\346\215\242\345\210\260\351\252\214\350\257\201\347\240\201\347\231\273\345\275\225", Q_NULLPTR));
        btnDoLogin->setText(QApplication::translate("AuthWindow", "\347\231\273\345\275\225", Q_NULLPTR));
        btnSwitchRegister->setText(QApplication::translate("AuthWindow", "\346\262\241\346\234\211\350\264\246\345\217\267\357\274\237\345\216\273\346\263\250\345\206\214", Q_NULLPTR));
        editPhoneRegister->setPlaceholderText(QApplication::translate("AuthWindow", "\346\211\213\346\234\272\345\217\267", Q_NULLPTR));
        editNameRegister->setPlaceholderText(QApplication::translate("AuthWindow", "\346\230\265\347\247\260", Q_NULLPTR));
        editPwdRegister->setPlaceholderText(QApplication::translate("AuthWindow", "\345\257\206\347\240\201", Q_NULLPTR));
        editCodeRegister->setPlaceholderText(QApplication::translate("AuthWindow", "\351\252\214\350\257\201\347\240\201", Q_NULLPTR));
        btnSendCodeRegister->setText(QApplication::translate("AuthWindow", "\350\216\267\345\217\226\351\252\214\350\257\201\347\240\201", Q_NULLPTR));
        btnDoRegister->setText(QApplication::translate("AuthWindow", "\346\263\250\345\206\214", Q_NULLPTR));
        btnSwitchLogin->setText(QApplication::translate("AuthWindow", "\345\267\262\346\234\211\350\264\246\345\217\267\357\274\237\345\216\273\347\231\273\345\275\225", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class AuthWindow: public Ui_AuthWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUTHWINDOW_H
