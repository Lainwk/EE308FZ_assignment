/********************************************************************************
** Form generated from reading UI file 'homepage.ui'
**
** Created by: Qt User Interface Compiler version 5.9.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HOMEPAGE_H
#define UI_HOMEPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_HomePage
{
public:
    QVBoxLayout *verticalLayout_6;
    QFrame *frameHome;
    QVBoxLayout *verticalLayout;
    QFrame *frameHomeTitle;
    QHBoxLayout *horizontalLayout;
    QLabel *lbHeart;
    QLabel *labelLogo;
    QSpacerItem *horizontalSpacer;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_7;
    QVBoxLayout *verticalLayout_8;
    QGroupBox *groupUserStatus;
    QVBoxLayout *verticalLayout_12;
    QHBoxLayout *horizontalLayout_4;
    QLabel *lbPerson;
    QVBoxLayout *verticalLayout_2;
    QLabel *labelUserName;
    QLabel *label_4;
    QLabel *lbScore;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_5;
    QSpacerItem *horizontalSpacer_3;
    QLabel *label_6;
    QProgressBar *progressMood;
    QVBoxLayout *verticalLayout_9;
    QGroupBox *groupQuickTest;
    QVBoxLayout *verticalLayout_3;
    QLabel *label;
    QLabel *labelQuickTestDesc;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *btnStartTest;
    QVBoxLayout *verticalLayout_10;
    QGroupBox *groupTrend;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_2;
    QLabel *labelTrendPlaceholder;
    QVBoxLayout *verticalLayout_11;
    QGroupBox *groupSuggestion;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_7;
    QLabel *labelSuggestion;

    void setupUi(QWidget *HomePage)
    {
        if (HomePage->objectName().isEmpty())
            HomePage->setObjectName(QStringLiteral("HomePage"));
        HomePage->resize(400, 720);
        HomePage->setStyleSheet(QString::fromUtf8("*\n"
"{\n"
"    font-family: \"Microsoft YaHei\";\n"
"}\n"
"\n"
"QLabel#lbPerson\n"
"{\n"
"	image: url(:/image/person.png);\n"
"}\n"
"QLabel#lbHeart\n"
"{\n"
"	image: url(:/image/heart.png);\n"
"}\n"
"QLabel#lbScore\n"
"{\n"
"	background-color: #EFF6FF;\n"
"	color: #2563EB;\n"
"	font:16px;\n"
"	border-radius:12px;\n"
"}\n"
"\n"
"QScrollArea\n"
"{\n"
"	border:none;\n"
"}\n"
"\n"
"QFrame#frameHome\n"
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
"\n"
"QProgressBar \n"
"{\n"
"	background-color: #f0f0f0;\n"
"	border-radius: 5px;\n"
"	text-align: center;\n"
"}\n"
"\n"
"QProgressBar::chunk \n"
"{\n"
"	border-radius: 5px;\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"         stop:0 #4a90e2, \n"
"         stop:1 #57d9a3);\n"
"}\n"
"\n"
"QGroupBox\n"
"{\n"
"	background-co"
                        "lor: rgb(255, 255, 255);\n"
"	border-radius:12px;\n"
"}\n"
"\n"
"QGroupBox#groupQuickTest\n"
"{\n"
"background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                       stop:0 #4388F4, /* \346\270\220\345\217\230\350\265\267\345\247\213\350\211\262\357\274\210\350\223\235\357\274\211 */\n"
"                                       stop:1 #2ecc71);/* \346\270\220\345\217\230\347\273\223\346\235\237\350\211\262\357\274\210\347\273\277\357\274\211 */\n"
"}\n"
"\n"
"QGroupBox#groupQuickTest QLabel\n"
"{\n"
"	color: rgb(255, 255, 255);\n"
"}\n"
"\n"
"QPushButton#btnStartTest\n"
"{\n"
"	background-color: rgb(255, 255, 255);\n"
"	color: #2563EB;\n"
"	border-radius:20px;\n"
"	font:14px;\n"
"}\n"
"\n"
"\n"
"QScrollBar:vertical \n"
"{\n"
"	border:0px solid red;\n"
"	background-color:#FAFAFA;\n"
"	width:6px;\n"
"	margin:0 0 0 0px;\n"
"}\n"
"\n"
"QScrollBar::add-line:vertical \n"
"{\n"
" border:none;\n"
" background:transparent; \n"
" height:0px;\n"
"}\n"
" /* \351\241\266\351\203\250\346\216\247\345"
                        "\210\266\345\235\227 */ \n"
"QScrollBar::sub-line:vertical { \n"
" border:none; \n"
" background:transparent; \n"
" height:0px; \n"
" subcontrol-position:top;\n"
" subcontrol-origin:margin;\n"
" \n"
"}\n"
"/* \346\273\221\345\235\227\350\256\276\347\275\256 */\n"
"QScrollBar::handle:vertical {\n"
"	background: #DCDFE6;\n"
"	min-height:200px;\n"
"	border-radius: 3px;\n"
"}\n"
" /* \346\273\221\345\235\227\344\270\212\350\276\271\345\214\272\345\237\237 */ \n"
"QScrollBar::add-page:vertical {\n"
" background:transparent;\n"
"} \n"
"/* \346\273\221\345\235\227\344\270\213\350\276\271\345\214\272\345\237\237 */\n"
" QScrollBar::sub-page:vertical {\n"
" background:transparent; \n"
"}"));
        verticalLayout_6 = new QVBoxLayout(HomePage);
        verticalLayout_6->setSpacing(0);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        frameHome = new QFrame(HomePage);
        frameHome->setObjectName(QStringLiteral("frameHome"));
        frameHome->setFrameShape(QFrame::StyledPanel);
        frameHome->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(frameHome);
        verticalLayout->setSpacing(15);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        frameHomeTitle = new QFrame(frameHome);
        frameHomeTitle->setObjectName(QStringLiteral("frameHomeTitle"));
        frameHomeTitle->setMaximumSize(QSize(16777215, 40));
        frameHomeTitle->setFrameShape(QFrame::StyledPanel);
        frameHomeTitle->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frameHomeTitle);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(12, -1, 12, -1);
        lbHeart = new QLabel(frameHomeTitle);
        lbHeart->setObjectName(QStringLiteral("lbHeart"));
        lbHeart->setMinimumSize(QSize(20, 20));
        lbHeart->setMaximumSize(QSize(20, 20));

        horizontalLayout->addWidget(lbHeart);

        labelLogo = new QLabel(frameHomeTitle);
        labelLogo->setObjectName(QStringLiteral("labelLogo"));
        QFont font;
        font.setFamily(QStringLiteral("Microsoft YaHei"));
        font.setPointSize(12);
        font.setBold(false);
        font.setWeight(50);
        labelLogo->setFont(font);

        horizontalLayout->addWidget(labelLogo);

        horizontalSpacer = new QSpacerItem(309, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(frameHomeTitle);

        scrollArea = new QScrollArea(frameHome);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 400, 665));
        verticalLayout_7 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_7->setSpacing(15);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        verticalLayout_7->setContentsMargins(0, 0, 0, 20);
        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        verticalLayout_8->setContentsMargins(10, 0, 10, -1);
        groupUserStatus = new QGroupBox(scrollAreaWidgetContents);
        groupUserStatus->setObjectName(QStringLiteral("groupUserStatus"));
        groupUserStatus->setMinimumSize(QSize(0, 130));
        groupUserStatus->setMaximumSize(QSize(16777215, 16777215));
        verticalLayout_12 = new QVBoxLayout(groupUserStatus);
        verticalLayout_12->setSpacing(0);
        verticalLayout_12->setObjectName(QStringLiteral("verticalLayout_12"));
        verticalLayout_12->setContentsMargins(14, 15, 14, -1);
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(10);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        lbPerson = new QLabel(groupUserStatus);
        lbPerson->setObjectName(QStringLiteral("lbPerson"));
        lbPerson->setMinimumSize(QSize(50, 50));
        lbPerson->setMaximumSize(QSize(50, 50));

        horizontalLayout_4->addWidget(lbPerson);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        labelUserName = new QLabel(groupUserStatus);
        labelUserName->setObjectName(QStringLiteral("labelUserName"));
        QFont font1;
        font1.setFamily(QStringLiteral("Microsoft YaHei"));
        font1.setPointSize(12);
        labelUserName->setFont(font1);

        verticalLayout_2->addWidget(labelUserName);

        label_4 = new QLabel(groupUserStatus);
        label_4->setObjectName(QStringLiteral("label_4"));
        QFont font2;
        font2.setFamily(QStringLiteral("Microsoft YaHei"));
        font2.setPointSize(11);
        label_4->setFont(font2);

        verticalLayout_2->addWidget(label_4);


        horizontalLayout_4->addLayout(verticalLayout_2);

        lbScore = new QLabel(groupUserStatus);
        lbScore->setObjectName(QStringLiteral("lbScore"));
        lbScore->setMinimumSize(QSize(50, 30));
        lbScore->setMaximumSize(QSize(50, 30));
        lbScore->setAlignment(Qt::AlignCenter);

        horizontalLayout_4->addWidget(lbScore);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);


        verticalLayout_12->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_5 = new QLabel(groupUserStatus);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout_5->addWidget(label_5);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_3);

        label_6 = new QLabel(groupUserStatus);
        label_6->setObjectName(QStringLiteral("label_6"));

        horizontalLayout_5->addWidget(label_6);


        verticalLayout_12->addLayout(horizontalLayout_5);

        progressMood = new QProgressBar(groupUserStatus);
        progressMood->setObjectName(QStringLiteral("progressMood"));
        progressMood->setMaximumSize(QSize(16777215, 10));
        progressMood->setValue(76);
        progressMood->setTextVisible(false);

        verticalLayout_12->addWidget(progressMood);


        verticalLayout_8->addWidget(groupUserStatus);


        verticalLayout_7->addLayout(verticalLayout_8);

        verticalLayout_9 = new QVBoxLayout();
        verticalLayout_9->setObjectName(QStringLiteral("verticalLayout_9"));
        verticalLayout_9->setContentsMargins(10, -1, 10, -1);
        groupQuickTest = new QGroupBox(scrollAreaWidgetContents);
        groupQuickTest->setObjectName(QStringLiteral("groupQuickTest"));
        groupQuickTest->setMinimumSize(QSize(0, 150));
        verticalLayout_3 = new QVBoxLayout(groupQuickTest);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        label = new QLabel(groupQuickTest);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(label);

        labelQuickTestDesc = new QLabel(groupQuickTest);
        labelQuickTestDesc->setObjectName(QStringLiteral("labelQuickTestDesc"));
        labelQuickTestDesc->setAlignment(Qt::AlignCenter);
        labelQuickTestDesc->setWordWrap(true);

        verticalLayout_3->addWidget(labelQuickTestDesc);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        btnStartTest = new QPushButton(groupQuickTest);
        btnStartTest->setObjectName(QStringLiteral("btnStartTest"));
        btnStartTest->setMinimumSize(QSize(126, 45));
        btnStartTest->setMaximumSize(QSize(126, 45));

        horizontalLayout_2->addWidget(btnStartTest);


        verticalLayout_3->addLayout(horizontalLayout_2);


        verticalLayout_9->addWidget(groupQuickTest);


        verticalLayout_7->addLayout(verticalLayout_9);

        verticalLayout_10 = new QVBoxLayout();
        verticalLayout_10->setObjectName(QStringLiteral("verticalLayout_10"));
        verticalLayout_10->setContentsMargins(10, -1, 10, -1);
        groupTrend = new QGroupBox(scrollAreaWidgetContents);
        groupTrend->setObjectName(QStringLiteral("groupTrend"));
        verticalLayout_4 = new QVBoxLayout(groupTrend);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_3 = new QLabel(groupTrend);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_3->addWidget(label_3);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        label_2 = new QLabel(groupTrend);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_3->addWidget(label_2);


        verticalLayout_4->addLayout(horizontalLayout_3);

        labelTrendPlaceholder = new QLabel(groupTrend);
        labelTrendPlaceholder->setObjectName(QStringLiteral("labelTrendPlaceholder"));
        labelTrendPlaceholder->setAlignment(Qt::AlignCenter);
        labelTrendPlaceholder->setWordWrap(true);

        verticalLayout_4->addWidget(labelTrendPlaceholder);


        verticalLayout_10->addWidget(groupTrend);


        verticalLayout_7->addLayout(verticalLayout_10);

        verticalLayout_11 = new QVBoxLayout();
        verticalLayout_11->setObjectName(QStringLiteral("verticalLayout_11"));
        verticalLayout_11->setContentsMargins(10, -1, 10, -1);
        groupSuggestion = new QGroupBox(scrollAreaWidgetContents);
        groupSuggestion->setObjectName(QStringLiteral("groupSuggestion"));
        groupSuggestion->setMinimumSize(QSize(0, 100));
        verticalLayout_5 = new QVBoxLayout(groupSuggestion);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        label_7 = new QLabel(groupSuggestion);
        label_7->setObjectName(QStringLiteral("label_7"));

        horizontalLayout_6->addWidget(label_7);


        verticalLayout_5->addLayout(horizontalLayout_6);

        labelSuggestion = new QLabel(groupSuggestion);
        labelSuggestion->setObjectName(QStringLiteral("labelSuggestion"));
        labelSuggestion->setWordWrap(true);

        verticalLayout_5->addWidget(labelSuggestion);


        verticalLayout_11->addWidget(groupSuggestion);


        verticalLayout_7->addLayout(verticalLayout_11);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollArea);


        verticalLayout_6->addWidget(frameHome);


        retranslateUi(HomePage);

        QMetaObject::connectSlotsByName(HomePage);
    } // setupUi

    void retranslateUi(QWidget *HomePage)
    {
        HomePage->setWindowTitle(QApplication::translate("HomePage", "\351\246\226\351\241\265", Q_NULLPTR));
        lbHeart->setText(QString());
        labelLogo->setText(QApplication::translate("HomePage", "\345\277\203\345\261\277", Q_NULLPTR));
        groupUserStatus->setTitle(QString());
        lbPerson->setText(QString());
        labelUserName->setText(QApplication::translate("HomePage", "\345\277\203\347\201\265\346\227\205\344\272\272", Q_NULLPTR));
        label_4->setText(QApplication::translate("HomePage", "\344\273\212\346\227\245\346\203\205\347\273\252\346\214\207\346\225\260", Q_NULLPTR));
        lbScore->setText(QApplication::translate("HomePage", "76", Q_NULLPTR));
        label_5->setText(QApplication::translate("HomePage", "\345\271\263\351\235\231", Q_NULLPTR));
        label_6->setText(QApplication::translate("HomePage", "\347\247\257\346\236\201", Q_NULLPTR));
        groupQuickTest->setTitle(QString());
        label->setText(QApplication::translate("HomePage", "\344\273\212\346\227\245\345\277\203\351\207\214\346\265\213\350\257\204", Q_NULLPTR));
        labelQuickTestDesc->setText(QApplication::translate("HomePage", "\345\217\252\351\234\200\345\207\240\345\210\206\351\222\237\357\274\214\344\272\206\350\247\243\344\275\240\347\232\204\345\275\223\344\270\213\346\203\205\347\273\252\343\200\202", Q_NULLPTR));
        btnStartTest->setText(QApplication::translate("HomePage", "\345\274\200\345\247\213\344\273\212\346\227\245\346\265\213\350\257\204", Q_NULLPTR));
        groupTrend->setTitle(QString());
        label_3->setText(QApplication::translate("HomePage", "\346\203\205\347\273\252\350\266\213\345\212\277", Q_NULLPTR));
        label_2->setText(QApplication::translate("HomePage", "\350\277\2217\345\244\251", Q_NULLPTR));
        labelTrendPlaceholder->setText(QApplication::translate("HomePage", "\350\277\231\351\207\214\351\242\204\347\225\231\347\273\230\345\210\266\346\203\205\347\273\252\350\266\213\345\212\277\345\233\276\347\232\204\344\275\215\347\275\256\343\200\202", Q_NULLPTR));
        groupSuggestion->setTitle(QString());
        label_7->setText(QApplication::translate("HomePage", "\346\257\217\346\227\245\345\277\203\347\220\206\345\260\217tips", Q_NULLPTR));
        labelSuggestion->setText(QApplication::translate("HomePage", "\346\240\271\346\215\256\344\275\240\347\232\204\346\203\205\347\273\252\346\225\260\346\215\256\357\274\214\350\277\231\351\207\214\344\274\232\347\273\231\345\207\272\344\270\252\346\200\247\345\214\226\347\232\204\345\260\217\345\273\272\350\256\256\343\200\202", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class HomePage: public Ui_HomePage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HOMEPAGE_H
