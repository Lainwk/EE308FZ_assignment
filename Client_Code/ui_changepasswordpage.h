/********************************************************************************
** Form generated from reading UI file 'changepasswordpage.ui'
**
** Created by: Qt User Interface Compiler version 5.9.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHANGEPASSWORDPAGE_H
#define UI_CHANGEPASSWORDPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChangePasswordPage
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnBack;
    QLabel *labelTitle;
    QSpacerItem *horizontalSpacer;
    QGroupBox *groupPassword;
    QFormLayout *formLayout;
    QLabel *labelOldPassword;
    QLineEdit *editOldPassword;
    QLabel *labelNewPassword;
    QLineEdit *editNewPassword;
    QLabel *labelConfirmPassword;
    QLineEdit *editConfirmPassword;
    QLabel *labelHint;
    QPushButton *btnSubmit;
    QSpacerItem *verticalSpacer_bottom;

    void setupUi(QWidget *ChangePasswordPage)
    {
        if (ChangePasswordPage->objectName().isEmpty())
            ChangePasswordPage->setObjectName(QStringLiteral("ChangePasswordPage"));
        ChangePasswordPage->resize(400, 720);
        ChangePasswordPage->setStyleSheet(QString::fromUtf8("*\n"
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
""));
        verticalLayout = new QVBoxLayout(ChangePasswordPage);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(16, 16, 16, 16);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        btnBack = new QPushButton(ChangePasswordPage);
        btnBack->setObjectName(QStringLiteral("btnBack"));
        btnBack->setMinimumSize(QSize(25, 25));
        btnBack->setMaximumSize(QSize(25, 25));

        horizontalLayout->addWidget(btnBack);

        labelTitle = new QLabel(ChangePasswordPage);
        labelTitle->setObjectName(QStringLiteral("labelTitle"));
        QFont font;
        font.setFamily(QStringLiteral("Microsoft YaHei"));
        font.setPointSize(12);
        font.setBold(false);
        font.setWeight(50);
        labelTitle->setFont(font);
        labelTitle->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(labelTitle);

        horizontalSpacer = new QSpacerItem(60, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        groupPassword = new QGroupBox(ChangePasswordPage);
        groupPassword->setObjectName(QStringLiteral("groupPassword"));
        formLayout = new QFormLayout(groupPassword);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setHorizontalSpacing(10);
        formLayout->setVerticalSpacing(15);
        labelOldPassword = new QLabel(groupPassword);
        labelOldPassword->setObjectName(QStringLiteral("labelOldPassword"));

        formLayout->setWidget(0, QFormLayout::LabelRole, labelOldPassword);

        editOldPassword = new QLineEdit(groupPassword);
        editOldPassword->setObjectName(QStringLiteral("editOldPassword"));
        editOldPassword->setEchoMode(QLineEdit::Password);

        formLayout->setWidget(0, QFormLayout::FieldRole, editOldPassword);

        labelNewPassword = new QLabel(groupPassword);
        labelNewPassword->setObjectName(QStringLiteral("labelNewPassword"));

        formLayout->setWidget(1, QFormLayout::LabelRole, labelNewPassword);

        editNewPassword = new QLineEdit(groupPassword);
        editNewPassword->setObjectName(QStringLiteral("editNewPassword"));
        editNewPassword->setEchoMode(QLineEdit::Password);

        formLayout->setWidget(1, QFormLayout::FieldRole, editNewPassword);

        labelConfirmPassword = new QLabel(groupPassword);
        labelConfirmPassword->setObjectName(QStringLiteral("labelConfirmPassword"));

        formLayout->setWidget(2, QFormLayout::LabelRole, labelConfirmPassword);

        editConfirmPassword = new QLineEdit(groupPassword);
        editConfirmPassword->setObjectName(QStringLiteral("editConfirmPassword"));
        editConfirmPassword->setEchoMode(QLineEdit::Password);

        formLayout->setWidget(2, QFormLayout::FieldRole, editConfirmPassword);


        verticalLayout->addWidget(groupPassword);

        labelHint = new QLabel(ChangePasswordPage);
        labelHint->setObjectName(QStringLiteral("labelHint"));
        labelHint->setStyleSheet(QStringLiteral("color: gray; font-size: 10pt;"));
        labelHint->setAlignment(Qt::AlignCenter);
        labelHint->setWordWrap(true);

        verticalLayout->addWidget(labelHint);

        btnSubmit = new QPushButton(ChangePasswordPage);
        btnSubmit->setObjectName(QStringLiteral("btnSubmit"));
        btnSubmit->setMinimumSize(QSize(0, 40));
        QFont font1;
        font1.setFamily(QStringLiteral("Microsoft YaHei"));
        font1.setBold(true);
        font1.setWeight(75);
        btnSubmit->setFont(font1);

        verticalLayout->addWidget(btnSubmit);

        verticalSpacer_bottom = new QSpacerItem(20, 150, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_bottom);


        retranslateUi(ChangePasswordPage);

        QMetaObject::connectSlotsByName(ChangePasswordPage);
    } // setupUi

    void retranslateUi(QWidget *ChangePasswordPage)
    {
        ChangePasswordPage->setWindowTitle(QApplication::translate("ChangePasswordPage", "\344\277\256\346\224\271\345\257\206\347\240\201", Q_NULLPTR));
        btnBack->setText(QString());
        labelTitle->setText(QApplication::translate("ChangePasswordPage", "\344\277\256\346\224\271\345\257\206\347\240\201", Q_NULLPTR));
        groupPassword->setTitle(QApplication::translate("ChangePasswordPage", "\345\257\206\347\240\201\344\277\241\346\201\257", Q_NULLPTR));
        labelOldPassword->setText(QApplication::translate("ChangePasswordPage", "\345\216\237\345\257\206\347\240\201:", Q_NULLPTR));
        editOldPassword->setPlaceholderText(QApplication::translate("ChangePasswordPage", "\350\257\267\350\276\223\345\205\245\345\216\237\345\257\206\347\240\201", Q_NULLPTR));
        labelNewPassword->setText(QApplication::translate("ChangePasswordPage", "\346\226\260\345\257\206\347\240\201:", Q_NULLPTR));
        editNewPassword->setPlaceholderText(QApplication::translate("ChangePasswordPage", "\350\257\267\350\276\223\345\205\245\346\226\260\345\257\206\347\240\201", Q_NULLPTR));
        labelConfirmPassword->setText(QApplication::translate("ChangePasswordPage", "\347\241\256\350\256\244\345\257\206\347\240\201:", Q_NULLPTR));
        editConfirmPassword->setPlaceholderText(QApplication::translate("ChangePasswordPage", "\350\257\267\345\206\215\346\254\241\350\276\223\345\205\245\346\226\260\345\257\206\347\240\201", Q_NULLPTR));
        labelHint->setText(QApplication::translate("ChangePasswordPage", "\346\217\220\347\244\272\357\274\232\345\257\206\347\240\201\351\225\277\345\272\246\350\207\263\345\260\2216\344\275\215\357\274\214\345\273\272\350\256\256\345\214\205\345\220\253\345\255\227\346\257\215\345\222\214\346\225\260\345\255\227", Q_NULLPTR));
        btnSubmit->setText(QApplication::translate("ChangePasswordPage", "\347\241\256\350\256\244\344\277\256\346\224\271", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ChangePasswordPage: public Ui_ChangePasswordPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHANGEPASSWORDPAGE_H
