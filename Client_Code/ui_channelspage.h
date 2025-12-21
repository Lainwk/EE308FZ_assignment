/********************************************************************************
** Form generated from reading UI file 'channelspage.ui'
**
** Created by: Qt User Interface Compiler version 5.9.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHANNELSPAGE_H
#define UI_CHANNELSPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChannelsPage
{
public:
    QVBoxLayout *verticalLayout_5;
    QFrame *frameChannel;
    QVBoxLayout *verticalLayout;
    QFrame *frameHomeTitle;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelTitle;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *btnNewChannel;
    QFrame *frameTab;
    QVBoxLayout *verticalLayout_9;
    QTabWidget *tabWidget;
    QWidget *tabCommunity;
    QVBoxLayout *verticalLayout_2;
    QTextEdit *txtPost;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label;
    QListWidget *listFeed;
    QWidget *tabAIChat;
    QVBoxLayout *verticalLayout_3;
    QLabel *labelAITitle;
    QListWidget *listAIChat;
    QHBoxLayout *horizontalLayout_ai;
    QLineEdit *txtAIInput;
    QPushButton *btnAISend1;
    QWidget *tabTools;
    QVBoxLayout *verticalLayout_4;
    QLabel *labelTools;

    void setupUi(QWidget *ChannelsPage)
    {
        if (ChannelsPage->objectName().isEmpty())
            ChannelsPage->setObjectName(QStringLiteral("ChannelsPage"));
        ChannelsPage->resize(400, 720);
        ChannelsPage->setMinimumSize(QSize(400, 0));
        ChannelsPage->setMaximumSize(QSize(400, 16777215));
        ChannelsPage->setStyleSheet(QString::fromUtf8("*\n"
"{\n"
"    font-family: \"Microsoft YaHei\";\n"
"}\n"
"\n"
"QScrollArea\n"
"{\n"
"	border:none;\n"
"}\n"
"QFrame#frameTab\n"
"{\n"
"	background-color: rgb(255, 255, 255);\n"
"}\n"
"QFrame#frameChannel\n"
"{\n"
"	background-color: rgb(244, 245, 246);\n"
"	border-top-left-radius:12px;\n"
"	border-top-right-radius:12px;\n"
"}\n"
"\n"
"QFrame#frameHomeTitle\n"
"{\n"
"	background-color: rgb(255, 255, 255);\n"
"	border-top-left-radius:12px;\n"
"	border-top-right-radius:12px;\n"
"}\n"
"QPushButton#btnNewChannel\n"
"{\n"
"	border-image: url(:/image/release.png);\n"
"}\n"
"\n"
"QPushButton#btnAISend1\n"
"{\n"
"	background: #07C160;\n"
"	color: rgb(255, 255, 255);\n"
"	border-radius: 8px\n"
"}\n"
"\n"
"QTabWidget\n"
"{\n"
"	border:none;\n"
"}\n"
"\n"
"QTabWidget::pane \n"
"{ \n"
"	border:none;\n"
"	border-top:1px solid #D9D9D9;\n"
"}\n"
"\n"
"QTabBar::tab\n"
"{\n"
"	border:none;\n"
"	padding-bottom:4px;\n"
"	margin-right:10px;\n"
"	color:#999999;\n"
"	border-top-left-radius: 5px;\n"
"    border-top-right-radius: 5p"
                        "x;\n"
"    padding: 6px 12px;\n"
"    margin-right: 2px;\n"
"	font-size: 12px;\n"
"}\n"
"\n"
"QTabBar::tab:selected, QTabBar::tab:hover\n"
"{\n"
"	color:#2ecc71;\n"
"	border-bottom:2px solid #2ecc71;\n"
"}\n"
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
"QTextEdit\n"
"{\n"
"	border-radius:8px;\n"
"}\n"
"QListWidget\n"
"{\n"
"	border:none;\n"
"}"));
        verticalLayout_5 = new QVBoxLayout(ChannelsPage);
        verticalLayout_5->setSpacing(0);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        frameChannel = new QFrame(ChannelsPage);
        frameChannel->setObjectName(QStringLiteral("frameChannel"));
        frameChannel->setFrameShape(QFrame::StyledPanel);
        frameChannel->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(frameChannel);
        verticalLayout->setSpacing(1);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 20);
        frameHomeTitle = new QFrame(frameChannel);
        frameHomeTitle->setObjectName(QStringLiteral("frameHomeTitle"));
        frameHomeTitle->setMaximumSize(QSize(16777215, 40));
        frameHomeTitle->setFrameShape(QFrame::StyledPanel);
        frameHomeTitle->setFrameShadow(QFrame::Raised);
        horizontalLayout_2 = new QHBoxLayout(frameHomeTitle);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(12, -1, 12, -1);
        labelTitle = new QLabel(frameHomeTitle);
        labelTitle->setObjectName(QStringLiteral("labelTitle"));
        QFont font;
        font.setFamily(QStringLiteral("Microsoft YaHei"));
        font.setPointSize(12);
        font.setBold(false);
        font.setWeight(50);
        labelTitle->setFont(font);

        horizontalLayout_2->addWidget(labelTitle);

        horizontalSpacer_2 = new QSpacerItem(335, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        btnNewChannel = new QPushButton(frameHomeTitle);
        btnNewChannel->setObjectName(QStringLiteral("btnNewChannel"));
        btnNewChannel->setMinimumSize(QSize(25, 25));
        btnNewChannel->setMaximumSize(QSize(25, 25));
        QFont font1;
        font1.setFamily(QStringLiteral("Microsoft YaHei"));
        font1.setPointSize(8);
        btnNewChannel->setFont(font1);

        horizontalLayout_2->addWidget(btnNewChannel);


        verticalLayout->addWidget(frameHomeTitle);

        frameTab = new QFrame(frameChannel);
        frameTab->setObjectName(QStringLiteral("frameTab"));
        frameTab->setFrameShape(QFrame::StyledPanel);
        frameTab->setFrameShadow(QFrame::Raised);
        verticalLayout_9 = new QVBoxLayout(frameTab);
        verticalLayout_9->setObjectName(QStringLiteral("verticalLayout_9"));
        tabWidget = new QTabWidget(frameTab);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabCommunity = new QWidget();
        tabCommunity->setObjectName(QStringLiteral("tabCommunity"));
        verticalLayout_2 = new QVBoxLayout(tabCommunity);
        verticalLayout_2->setSpacing(15);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 10, 0, 0);
        txtPost = new QTextEdit(tabCommunity);
        txtPost->setObjectName(QStringLiteral("txtPost"));
        txtPost->setMaximumSize(QSize(16777215, 100));

        verticalLayout_2->addWidget(txtPost);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label = new QLabel(tabCommunity);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_3->addWidget(label);


        verticalLayout_2->addLayout(horizontalLayout_3);

        listFeed = new QListWidget(tabCommunity);
        listFeed->setObjectName(QStringLiteral("listFeed"));
        listFeed->setSpacing(6);
        listFeed->setUniformItemSizes(false);

        verticalLayout_2->addWidget(listFeed);

        tabWidget->addTab(tabCommunity, QString());
        tabAIChat = new QWidget();
        tabAIChat->setObjectName(QStringLiteral("tabAIChat"));
        verticalLayout_3 = new QVBoxLayout(tabAIChat);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(8, 8, 8, 8);
        labelAITitle = new QLabel(tabAIChat);
        labelAITitle->setObjectName(QStringLiteral("labelAITitle"));
        QFont font2;
        font2.setFamily(QStringLiteral("Microsoft YaHei"));
        font2.setPointSize(12);
        font2.setBold(true);
        font2.setWeight(75);
        labelAITitle->setFont(font2);
        labelAITitle->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(labelAITitle);

        listAIChat = new QListWidget(tabAIChat);
        listAIChat->setObjectName(QStringLiteral("listAIChat"));
        listAIChat->setSpacing(8);
        listAIChat->setWordWrap(true);

        verticalLayout_3->addWidget(listAIChat);

        horizontalLayout_ai = new QHBoxLayout();
        horizontalLayout_ai->setObjectName(QStringLiteral("horizontalLayout_ai"));
        txtAIInput = new QLineEdit(tabAIChat);
        txtAIInput->setObjectName(QStringLiteral("txtAIInput"));

        horizontalLayout_ai->addWidget(txtAIInput);

        btnAISend1 = new QPushButton(tabAIChat);
        btnAISend1->setObjectName(QStringLiteral("btnAISend1"));
        btnAISend1->setMinimumSize(QSize(70, 25));

        horizontalLayout_ai->addWidget(btnAISend1);


        verticalLayout_3->addLayout(horizontalLayout_ai);

        tabWidget->addTab(tabAIChat, QString());
        tabTools = new QWidget();
        tabTools->setObjectName(QStringLiteral("tabTools"));
        verticalLayout_4 = new QVBoxLayout(tabTools);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        labelTools = new QLabel(tabTools);
        labelTools->setObjectName(QStringLiteral("labelTools"));
        labelTools->setWordWrap(true);

        verticalLayout_4->addWidget(labelTools);

        tabWidget->addTab(tabTools, QString());

        verticalLayout_9->addWidget(tabWidget);


        verticalLayout->addWidget(frameTab);


        verticalLayout_5->addWidget(frameChannel);


        retranslateUi(ChannelsPage);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ChannelsPage);
    } // setupUi

    void retranslateUi(QWidget *ChannelsPage)
    {
        ChannelsPage->setWindowTitle(QApplication::translate("ChannelsPage", "\347\244\276\345\214\272", Q_NULLPTR));
        labelTitle->setText(QApplication::translate("ChannelsPage", "\347\244\276\345\214\272", Q_NULLPTR));
        btnNewChannel->setText(QString());
        txtPost->setPlaceholderText(QApplication::translate("ChannelsPage", "\345\206\231\347\202\271\344\273\200\344\271\210\357\274\214\345\210\206\344\272\253\347\273\231\347\244\276\345\214\272\342\200\246", Q_NULLPTR));
        label->setText(QApplication::translate("ChannelsPage", "\346\234\200\346\226\260\345\212\250\346\200\201", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tabCommunity), QApplication::translate("ChannelsPage", "\347\244\276\345\214\272\345\212\250\346\200\201", Q_NULLPTR));
        labelAITitle->setText(QApplication::translate("ChannelsPage", "\360\237\244\226 AI\345\277\203\347\220\206\345\212\251\346\211\213", Q_NULLPTR));
        txtAIInput->setPlaceholderText(QApplication::translate("ChannelsPage", "\350\276\223\345\205\245\346\266\210\346\201\257...", Q_NULLPTR));
        btnAISend1->setText(QApplication::translate("ChannelsPage", "\345\217\221\351\200\201", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tabAIChat), QApplication::translate("ChannelsPage", "AI \345\257\271\350\257\235", Q_NULLPTR));
        labelTools->setText(QApplication::translate("ChannelsPage", "\350\277\231\351\207\214\345\217\257\344\273\245\346\224\276\347\275\256\346\203\205\347\273\252\346\227\245\350\256\260\343\200\201\345\206\245\346\203\263\347\273\203\344\271\240\347\255\211\345\267\245\345\205\267\345\205\245\345\217\243\343\200\202", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tabTools), QApplication::translate("ChannelsPage", "\345\277\203\347\220\206\345\267\245\345\205\267\347\256\261", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ChannelsPage: public Ui_ChannelsPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHANNELSPAGE_H
