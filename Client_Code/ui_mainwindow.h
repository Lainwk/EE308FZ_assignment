/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QVBoxLayout *verticalLayout_6;
    QStackedWidget *stackedWidget;
    QWidget *page_placeholder;
    QFrame *frameBottom;
    QHBoxLayout *horizontalLayout_6;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *btnHome;
    QLabel *lbHome;
    QVBoxLayout *verticalLayout_3;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *btnContacts;
    QLabel *lbContacts;
    QVBoxLayout *verticalLayout_4;
    QSpacerItem *verticalSpacer_3;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *btnChannels;
    QLabel *lbChannels;
    QVBoxLayout *verticalLayout_5;
    QSpacerItem *verticalSpacer_4;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *btnMe;
    QLabel *lbMe;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(400, 773);
        MainWindow->setMinimumSize(QSize(400, 0));
        MainWindow->setMaximumSize(QSize(400, 16777215));
        MainWindow->setStyleSheet(QLatin1String("QFrame#frame\n"
"{\n"
"	background-color: rgb(255, 255, 255);\n"
"	border-radius:12px;\n"
"}\n"
"\n"
"\n"
"QPushButton#btnHome,#btnContacts,#btnChannels,#btnMe\n"
"{\n"
"	color:#B5BAC3;\n"
"}\n"
"\n"
"QLabel#lbHome,#lbContacts,#lbChannels,#lbMe\n"
"{\n"
"	color:#B5BAC3;\n"
"}\n"
"\n"
"QPushButton#btnHome\n"
"{\n"
"	border-image: url(:/image/home.png);\n"
"}\n"
"\n"
"QPushButton#btnHome:checked\n"
"{\n"
"	border-image: url(:/image/home_check.png);\n"
"}\n"
"\n"
"QPushButton#btnContacts\n"
"{\n"
"	border-image: url(:/image/contact.png);\n"
"}\n"
"\n"
"QPushButton#btnContacts:checked\n"
"{\n"
"	border-image: url(:/image/contact_check.png);\n"
"}\n"
"\n"
"QPushButton#btnChannels\n"
"{\n"
"	border-image: url(:/image/chat.png);\n"
"}\n"
"\n"
"\n"
"QPushButton#btnChannels:checked\n"
"{\n"
"	border-image: url(:/image/chat_check.png);\n"
"}\n"
"\n"
"QPushButton#btnMe\n"
"{\n"
"	border-image: url(:/image/my.png);\n"
"}\n"
"\n"
"QPushButton#btnMe:checked\n"
"{\n"
"	border-image: url(:/image/my_check.png);\n"
"}\n"
""));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        frame = new QFrame(centralwidget);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_6 = new QVBoxLayout(frame);
        verticalLayout_6->setSpacing(0);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        stackedWidget = new QStackedWidget(frame);
        stackedWidget->setObjectName(QStringLiteral("stackedWidget"));
        page_placeholder = new QWidget();
        page_placeholder->setObjectName(QStringLiteral("page_placeholder"));
        stackedWidget->addWidget(page_placeholder);

        verticalLayout_6->addWidget(stackedWidget);

        frameBottom = new QFrame(frame);
        frameBottom->setObjectName(QStringLiteral("frameBottom"));
        frameBottom->setMinimumSize(QSize(0, 80));
        frameBottom->setMaximumSize(QSize(16777215, 80));
        frameBottom->setFrameShape(QFrame::StyledPanel);
        frameBottom->setFrameShadow(QFrame::Raised);
        horizontalLayout_6 = new QHBoxLayout(frameBottom);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(2);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(-1, 0, -1, -1);
        btnHome = new QPushButton(frameBottom);
        btnHome->setObjectName(QStringLiteral("btnHome"));
        btnHome->setMinimumSize(QSize(30, 30));
        btnHome->setMaximumSize(QSize(30, 30));
        btnHome->setIconSize(QSize(0, 0));
        btnHome->setCheckable(true);

        horizontalLayout_2->addWidget(btnHome);


        verticalLayout_2->addLayout(horizontalLayout_2);

        lbHome = new QLabel(frameBottom);
        lbHome->setObjectName(QStringLiteral("lbHome"));
        lbHome->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(lbHome);


        horizontalLayout->addLayout(verticalLayout_2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(2);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(-1, 0, -1, -1);
        btnContacts = new QPushButton(frameBottom);
        btnContacts->setObjectName(QStringLiteral("btnContacts"));
        btnContacts->setMinimumSize(QSize(30, 30));
        btnContacts->setMaximumSize(QSize(30, 30));
        btnContacts->setCheckable(true);

        horizontalLayout_3->addWidget(btnContacts);


        verticalLayout_3->addLayout(horizontalLayout_3);

        lbContacts = new QLabel(frameBottom);
        lbContacts->setObjectName(QStringLiteral("lbContacts"));
        lbContacts->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(lbContacts);


        horizontalLayout->addLayout(verticalLayout_3);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(2);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(-1, 0, -1, -1);
        btnChannels = new QPushButton(frameBottom);
        btnChannels->setObjectName(QStringLiteral("btnChannels"));
        btnChannels->setMinimumSize(QSize(30, 30));
        btnChannels->setMaximumSize(QSize(30, 30));
        btnChannels->setCheckable(true);

        horizontalLayout_4->addWidget(btnChannels);


        verticalLayout_4->addLayout(horizontalLayout_4);

        lbChannels = new QLabel(frameBottom);
        lbChannels->setObjectName(QStringLiteral("lbChannels"));
        lbChannels->setAlignment(Qt::AlignCenter);

        verticalLayout_4->addWidget(lbChannels);


        horizontalLayout->addLayout(verticalLayout_4);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(2);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_5->addItem(verticalSpacer_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(-1, 0, -1, -1);
        btnMe = new QPushButton(frameBottom);
        btnMe->setObjectName(QStringLiteral("btnMe"));
        btnMe->setMinimumSize(QSize(30, 30));
        btnMe->setMaximumSize(QSize(30, 30));
        btnMe->setCheckable(true);

        horizontalLayout_5->addWidget(btnMe);


        verticalLayout_5->addLayout(horizontalLayout_5);

        lbMe = new QLabel(frameBottom);
        lbMe->setObjectName(QStringLiteral("lbMe"));
        lbMe->setAlignment(Qt::AlignCenter);

        verticalLayout_5->addWidget(lbMe);


        horizontalLayout->addLayout(verticalLayout_5);


        horizontalLayout_6->addLayout(horizontalLayout);


        verticalLayout_6->addWidget(frameBottom);


        verticalLayout->addWidget(frame);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "\345\277\203\345\261\277 \302\267 \345\277\203\347\220\206\345\201\245\345\272\267", Q_NULLPTR));
        btnHome->setText(QString());
        lbHome->setText(QApplication::translate("MainWindow", "\351\246\226\351\241\265", Q_NULLPTR));
        btnContacts->setText(QString());
        lbContacts->setText(QApplication::translate("MainWindow", "\350\201\224\347\263\273\344\272\272", Q_NULLPTR));
        btnChannels->setText(QString());
        lbChannels->setText(QApplication::translate("MainWindow", "\351\242\221\351\201\223", Q_NULLPTR));
        btnMe->setText(QString());
        lbMe->setText(QApplication::translate("MainWindow", "\346\210\221\347\232\204", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
