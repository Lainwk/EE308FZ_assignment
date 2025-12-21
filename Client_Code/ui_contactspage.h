/********************************************************************************
** Form generated from reading UI file 'contactspage.ui'
**
** Created by: Qt User Interface Compiler version 5.9.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTACTSPAGE_H
#define UI_CONTACTSPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
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

class Ui_ContactsPage
{
public:
    QVBoxLayout *verticalLayout_4;
    QFrame *frameContacts;
    QVBoxLayout *verticalLayout;
    QFrame *frameHomeTitle;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelTitle;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *btnAddFriend;
    QHBoxLayout *horizontalLayout;
    QFrame *frameFind;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *btnFind;
    QLineEdit *editSearch;
    QHBoxLayout *horizontalLayout_5;
    QGroupBox *groupFriends;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label;
    QSpacerItem *horizontalSpacer;
    QListWidget *listFriends;
    QHBoxLayout *horizontalLayout_7;
    QGroupBox *groupRequests;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_2;
    QListWidget *listPending;

    void setupUi(QWidget *ContactsPage)
    {
        if (ContactsPage->objectName().isEmpty())
            ContactsPage->setObjectName(QStringLiteral("ContactsPage"));
        ContactsPage->resize(420, 720);
        ContactsPage->setStyleSheet(QLatin1String("*\n"
"{\n"
"    font-family: \"Microsoft YaHei\";\n"
"}\n"
"\n"
"QScrollArea\n"
"{\n"
"	border:none;\n"
"}\n"
"\n"
"QFrame#frameContacts\n"
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
"QPushButton#btnAddFriend\n"
"{\n"
"	border-image:url(:/image/add.png);\n"
"}\n"
"\n"
"QPushButton#btnFind\n"
"{\n"
"	border-image:url(:/image/find.png);\n"
"}\n"
"\n"
"QFrame#frameFind\n"
"{\n"
"	border: 1px solid #ddd;\n"
"	border-radius: 4px;\n"
"	padding: 8px;\n"
"	font-size: 14px;\n"
"	background-color: rgb(255, 255, 255);\n"
"}\n"
"\n"
"QLineEdit\n"
"{\n"
"	border:none;\n"
"	background-color: rgb(255, 255, 255);\n"
"}\n"
"\n"
"QGroupBox\n"
"{\n"
"	background-color: rgb(255, 255, 255);\n"
"	border-radius:12px;\n"
"}\n"
"\n"
"QListWidget\n"
"{\n"
"	border:none;\n"
"}\n"
""));
        verticalLayout_4 = new QVBoxLayout(ContactsPage);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        frameContacts = new QFrame(ContactsPage);
        frameContacts->setObjectName(QStringLiteral("frameContacts"));
        frameContacts->setFrameShape(QFrame::StyledPanel);
        frameContacts->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(frameContacts);
        verticalLayout->setSpacing(15);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 20);
        frameHomeTitle = new QFrame(frameContacts);
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

        btnAddFriend = new QPushButton(frameHomeTitle);
        btnAddFriend->setObjectName(QStringLiteral("btnAddFriend"));
        btnAddFriend->setMinimumSize(QSize(20, 20));
        btnAddFriend->setMaximumSize(QSize(20, 20));

        horizontalLayout_2->addWidget(btnAddFriend);


        verticalLayout->addWidget(frameHomeTitle);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(15, 0, 15, -1);
        frameFind = new QFrame(frameContacts);
        frameFind->setObjectName(QStringLiteral("frameFind"));
        frameFind->setMinimumSize(QSize(0, 40));
        frameFind->setMaximumSize(QSize(16777215, 16777215));
        frameFind->setFrameShape(QFrame::StyledPanel);
        frameFind->setFrameShadow(QFrame::Raised);
        horizontalLayout_3 = new QHBoxLayout(frameFind);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(-1, 0, -1, 0);
        btnFind = new QPushButton(frameFind);
        btnFind->setObjectName(QStringLiteral("btnFind"));
        btnFind->setMinimumSize(QSize(20, 20));
        btnFind->setMaximumSize(QSize(20, 20));

        horizontalLayout_3->addWidget(btnFind);

        editSearch = new QLineEdit(frameFind);
        editSearch->setObjectName(QStringLiteral("editSearch"));

        horizontalLayout_3->addWidget(editSearch);


        horizontalLayout->addWidget(frameFind);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(12, 0, 12, -1);
        groupFriends = new QGroupBox(frameContacts);
        groupFriends->setObjectName(QStringLiteral("groupFriends"));
        groupFriends->setMinimumSize(QSize(0, 400));
        verticalLayout_2 = new QVBoxLayout(groupFriends);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label = new QLabel(groupFriends);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_4->addWidget(label);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);


        verticalLayout_2->addLayout(horizontalLayout_4);

        listFriends = new QListWidget(groupFriends);
        listFriends->setObjectName(QStringLiteral("listFriends"));

        verticalLayout_2->addWidget(listFriends);


        horizontalLayout_5->addWidget(groupFriends);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(12, 0, 12, -1);
        groupRequests = new QGroupBox(frameContacts);
        groupRequests->setObjectName(QStringLiteral("groupRequests"));
        groupRequests->setMinimumSize(QSize(0, 0));
        groupRequests->setMaximumSize(QSize(16777215, 16777215));
        verticalLayout_3 = new QVBoxLayout(groupRequests);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        label_2 = new QLabel(groupRequests);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_6->addWidget(label_2);


        verticalLayout_3->addLayout(horizontalLayout_6);

        listPending = new QListWidget(groupRequests);
        listPending->setObjectName(QStringLiteral("listPending"));
        listPending->setSelectionMode(QAbstractItemView::SingleSelection);

        verticalLayout_3->addWidget(listPending);


        horizontalLayout_7->addWidget(groupRequests);


        verticalLayout->addLayout(horizontalLayout_7);


        verticalLayout_4->addWidget(frameContacts);


        retranslateUi(ContactsPage);

        QMetaObject::connectSlotsByName(ContactsPage);
    } // setupUi

    void retranslateUi(QWidget *ContactsPage)
    {
        ContactsPage->setWindowTitle(QApplication::translate("ContactsPage", "\350\201\224\347\263\273\344\272\272", Q_NULLPTR));
        labelTitle->setText(QApplication::translate("ContactsPage", "\350\201\224\347\263\273\344\272\272", Q_NULLPTR));
        btnAddFriend->setText(QString());
        btnFind->setText(QString());
        editSearch->setPlaceholderText(QApplication::translate("ContactsPage", "\346\220\234\347\264\242\350\201\224\347\263\273\344\272\272", Q_NULLPTR));
        groupFriends->setTitle(QString());
        label->setText(QApplication::translate("ContactsPage", "\345\245\275\345\217\213\345\210\227\350\241\250", Q_NULLPTR));
        groupRequests->setTitle(QString());
        label_2->setText(QApplication::translate("ContactsPage", "\345\245\275\345\217\213\347\224\263\350\257\267", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ContactsPage: public Ui_ContactsPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTACTSPAGE_H
