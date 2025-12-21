/********************************************************************************
** Form generated from reading UI file 'chatpage.ui'
**
** Created by: Qt User Interface Compiler version 5.9.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHATPAGE_H
#define UI_CHATPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChatPage
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *topBar;
    QHBoxLayout *horizontalLayout;
    QPushButton *backButton;
    QSpacerItem *horizontalSpacer;
    QLabel *titleLabel;
    QSpacerItem *horizontalSpacer_2;
    QWidget *widget;
    QListWidget *messageList;
    QWidget *inputBar;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *messageInput;
    QPushButton *sendButton;

    void setupUi(QWidget *ChatPage)
    {
        if (ChatPage->objectName().isEmpty())
            ChatPage->setObjectName(QStringLiteral("ChatPage"));
        ChatPage->resize(400, 600);
        ChatPage->setStyleSheet(QLatin1String("QPushButton#backButton\n"
"{\n"
"	border-image: url(:/image/return.png);\n"
"}"));
        verticalLayout = new QVBoxLayout(ChatPage);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        topBar = new QWidget(ChatPage);
        topBar->setObjectName(QStringLiteral("topBar"));
        topBar->setMinimumSize(QSize(0, 50));
        topBar->setMaximumSize(QSize(16777215, 50));
        topBar->setStyleSheet(QStringLiteral("QWidget#topBar { background-color: #f7f7f7; border-bottom: 1px solid #e0e0e0; }"));
        horizontalLayout = new QHBoxLayout(topBar);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        backButton = new QPushButton(topBar);
        backButton->setObjectName(QStringLiteral("backButton"));
        backButton->setMinimumSize(QSize(25, 25));
        backButton->setMaximumSize(QSize(25, 25));
        backButton->setStyleSheet(QStringLiteral("QPushButton { border: none; font-size: 14px; color: #07c160; }"));

        horizontalLayout->addWidget(backButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        titleLabel = new QLabel(topBar);
        titleLabel->setObjectName(QStringLiteral("titleLabel"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        titleLabel->setFont(font);
        titleLabel->setStyleSheet(QStringLiteral("QLabel { font-weight: bold; font-size: 16px; }"));
        titleLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(titleLabel);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        widget = new QWidget(topBar);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setMinimumSize(QSize(60, 0));
        widget->setMaximumSize(QSize(60, 16777215));

        horizontalLayout->addWidget(widget);


        verticalLayout->addWidget(topBar);

        messageList = new QListWidget(ChatPage);
        messageList->setObjectName(QStringLiteral("messageList"));
        messageList->setStyleSheet(QStringLiteral("QListWidget { border: none; background-color: #ededed; }"));
        messageList->setSpacing(8);

        verticalLayout->addWidget(messageList);

        inputBar = new QWidget(ChatPage);
        inputBar->setObjectName(QStringLiteral("inputBar"));
        inputBar->setMinimumSize(QSize(0, 60));
        inputBar->setMaximumSize(QSize(16777215, 60));
        inputBar->setStyleSheet(QStringLiteral("QWidget#inputBar { background-color: white; border-top: 1px solid #e0e0e0; }"));
        horizontalLayout_2 = new QHBoxLayout(inputBar);
        horizontalLayout_2->setSpacing(8);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(12, 8, 12, 8);
        messageInput = new QLineEdit(inputBar);
        messageInput->setObjectName(QStringLiteral("messageInput"));
        messageInput->setMinimumSize(QSize(0, 36));
        messageInput->setStyleSheet(QStringLiteral("QLineEdit { border: 1px solid #d0d0d0; border-radius: 18px; padding: 8px 12px; font-size: 14px; }"));

        horizontalLayout_2->addWidget(messageInput);

        sendButton = new QPushButton(inputBar);
        sendButton->setObjectName(QStringLiteral("sendButton"));
        sendButton->setMinimumSize(QSize(70, 36));
        sendButton->setMaximumSize(QSize(70, 36));
        sendButton->setStyleSheet(QLatin1String("QPushButton { background-color: #07c160; color: white; border: none; border-radius: 18px; font-size: 14px; font-weight: bold; }\n"
"QPushButton:hover { background-color: #06ad56; }"));

        horizontalLayout_2->addWidget(sendButton);


        verticalLayout->addWidget(inputBar);


        retranslateUi(ChatPage);

        QMetaObject::connectSlotsByName(ChatPage);
    } // setupUi

    void retranslateUi(QWidget *ChatPage)
    {
        ChatPage->setWindowTitle(QApplication::translate("ChatPage", "Chat", Q_NULLPTR));
        backButton->setText(QString());
        titleLabel->setText(QApplication::translate("ChatPage", "\350\201\212\345\244\251", Q_NULLPTR));
        messageInput->setPlaceholderText(QApplication::translate("ChatPage", "\350\276\223\345\205\245\346\266\210\346\201\257...", Q_NULLPTR));
        sendButton->setText(QApplication::translate("ChatPage", "\345\217\221\351\200\201", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ChatPage: public Ui_ChatPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATPAGE_H
