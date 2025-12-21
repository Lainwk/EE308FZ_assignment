/********************************************************************************
** Form generated from reading UI file 'dailyassessmentpage.ui'
**
** Created by: Qt User Interface Compiler version 5.9.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DAILYASSESSMENTPAGE_H
#define UI_DAILYASSESSMENTPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DailyAssessmentPage
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *headerWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *backButton;
    QLabel *titleLabel;
    QSpacerItem *horizontalSpacer;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QWidget *footerWidget;
    QVBoxLayout *verticalLayout_2;
    QLabel *progressLabel;
    QPushButton *submitButton;

    void setupUi(QWidget *DailyAssessmentPage)
    {
        if (DailyAssessmentPage->objectName().isEmpty())
            DailyAssessmentPage->setObjectName(QStringLiteral("DailyAssessmentPage"));
        DailyAssessmentPage->resize(375, 812);
        DailyAssessmentPage->setStyleSheet(QLatin1String("QWidget {\n"
"    background-color: #F8F8F8;\n"
"}\n"
"\n"
"QPushButton#submitButton {\n"
"    background-color: #07C160;\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 22px;\n"
"    font-size: 16px;\n"
"    font-weight: bold;\n"
"    padding: 12px;\n"
"}\n"
"\n"
"QPushButton#submitButton:hover {\n"
"    background-color: #06AD56;\n"
"}\n"
"\n"
"QPushButton#submitButton:disabled {\n"
"    background-color: #CCCCCC;\n"
"}\n"
"\n"
"\n"
"QPushButton#backButton\n"
"{\n"
"	background-color: transparent; \n"
"	border-image: url(:/image/return.png);\n"
"}\n"
"\n"
"QLabel#progressLabel\n"
"{\n"
"	 background-color: transparent;\n"
"}"));
        verticalLayout = new QVBoxLayout(DailyAssessmentPage);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        headerWidget = new QWidget(DailyAssessmentPage);
        headerWidget->setObjectName(QStringLiteral("headerWidget"));
        headerWidget->setMinimumSize(QSize(0, 56));
        headerWidget->setMaximumSize(QSize(16777215, 56));
        headerWidget->setStyleSheet(QLatin1String("QWidget#headerWidget {\n"
"    background-color: white;\n"
"    border-bottom: 1px solid #EEEEEE;\n"
"}"));
        horizontalLayout = new QHBoxLayout(headerWidget);
        horizontalLayout->setSpacing(12);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(16, 0, 16, 0);
        backButton = new QPushButton(headerWidget);
        backButton->setObjectName(QStringLiteral("backButton"));
        backButton->setMinimumSize(QSize(25, 25));
        backButton->setMaximumSize(QSize(25, 25));

        horizontalLayout->addWidget(backButton);

        titleLabel = new QLabel(headerWidget);
        titleLabel->setObjectName(QStringLiteral("titleLabel"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        titleLabel->setFont(font);
        titleLabel->setStyleSheet(QLatin1String("QLabel {\n"
"    font-size: 18px;\n"
"    font-weight: bold;\n"
"    color: #333333;\n"
"}"));
        titleLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(titleLabel);

        horizontalSpacer = new QSpacerItem(60, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(headerWidget);

        scrollArea = new QScrollArea(DailyAssessmentPage);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 375, 676));
        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollArea);

        footerWidget = new QWidget(DailyAssessmentPage);
        footerWidget->setObjectName(QStringLiteral("footerWidget"));
        footerWidget->setMinimumSize(QSize(0, 80));
        footerWidget->setMaximumSize(QSize(16777215, 80));
        footerWidget->setStyleSheet(QLatin1String("QWidget#footerWidget {\n"
"    background-color: white;\n"
"    border-top: 1px solid #EEEEEE;\n"
"}"));
        verticalLayout_2 = new QVBoxLayout(footerWidget);
        verticalLayout_2->setSpacing(8);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(16, 12, 16, 12);
        progressLabel = new QLabel(footerWidget);
        progressLabel->setObjectName(QStringLiteral("progressLabel"));
        progressLabel->setStyleSheet(QLatin1String("QLabel {\n"
"    font-size: 14px;\n"
"    color: #999999;\n"
"}"));
        progressLabel->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(progressLabel);

        submitButton = new QPushButton(footerWidget);
        submitButton->setObjectName(QStringLiteral("submitButton"));
        submitButton->setMinimumSize(QSize(0, 44));

        verticalLayout_2->addWidget(submitButton);


        verticalLayout->addWidget(footerWidget);


        retranslateUi(DailyAssessmentPage);

        QMetaObject::connectSlotsByName(DailyAssessmentPage);
    } // setupUi

    void retranslateUi(QWidget *DailyAssessmentPage)
    {
        DailyAssessmentPage->setWindowTitle(QApplication::translate("DailyAssessmentPage", "\346\257\217\346\227\245\345\277\203\347\220\206\346\265\213\350\257\204", Q_NULLPTR));
        backButton->setText(QString());
        titleLabel->setText(QApplication::translate("DailyAssessmentPage", "\346\257\217\346\227\245\345\277\203\347\220\206\346\265\213\350\257\204", Q_NULLPTR));
        progressLabel->setText(QApplication::translate("DailyAssessmentPage", "\345\267\262\345\256\214\346\210\220 0/0 \351\242\230", Q_NULLPTR));
        submitButton->setText(QApplication::translate("DailyAssessmentPage", "\346\217\220\344\272\244\346\265\213\350\257\204", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class DailyAssessmentPage: public Ui_DailyAssessmentPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DAILYASSESSMENTPAGE_H
