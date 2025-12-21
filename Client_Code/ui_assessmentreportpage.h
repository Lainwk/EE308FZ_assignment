/********************************************************************************
** Form generated from reading UI file 'assessmentreportpage.ui'
**
** Created by: Qt User Interface Compiler version 5.9.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ASSESSMENTREPORTPAGE_H
#define UI_ASSESSMENTREPORTPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
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

class Ui_AssessmentReportPage
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
    QVBoxLayout *verticalLayout_2;
    QWidget *scoreCard;
    QVBoxLayout *verticalLayout_3;
    QLabel *label;
    QLabel *scoreLabel;
    QProgressBar *scoreProgressBar;
    QLabel *riskLevelLabel;
    QWidget *suggestionCard;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_2;
    QLabel *suggestionText;
    QPushButton *viewHistoryButton;
    QWidget *dimensionsWidget;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_3;
    QWidget *dimensionsContent;
    QWidget *trendsWidget;
    QVBoxLayout *verticalLayout_6;
    QLabel *label_4;
    QWidget *trendsContent;
    QWidget *indicatorsWidget;
    QVBoxLayout *verticalLayout_7;
    QLabel *label_5;
    QWidget *indicatorsContent;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *AssessmentReportPage)
    {
        if (AssessmentReportPage->objectName().isEmpty())
            AssessmentReportPage->setObjectName(QStringLiteral("AssessmentReportPage"));
        AssessmentReportPage->resize(375, 812);
        AssessmentReportPage->setStyleSheet(QLatin1String("QWidget {\n"
"    background-color: #F8F8F8;\n"
"}\n"
"\n"
"QPushButton#backButton {\n"
"    background-color: transparent;\n"
"    color: #07C160;\n"
"    border: none;\n"
"    font-size: 16px;\n"
"}\n"
"\n"
"QPushButton#viewHistoryButton {\n"
"    background-color: #07C160;\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 22px;\n"
"    font-size: 16px;\n"
"    font-weight: bold;\n"
"    padding: 12px;\n"
"}\n"
"\n"
"QPushButton#viewHistoryButton:hover {\n"
"    background-color: #06AD56;\n"
"}\n"
"\n"
"*\n"
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
"QPushButton#backButton\n"
"{\n"
"	background-color: transparent; \n"
"	border-image: url(:/image/return.png);\n"
"}\n"
"\n"
"QLabel\n"
"{\n"
"	background-color: transparent; \n"
"}\n"
"\n"
"QProgressBar \n"
"{\n"
"	background-color: #f0f0f0;\n"
"	border-radius: 5px"
                        ";\n"
"	text-align: center;\n"
"}\n"
"\n"
"QProgressBar::chunk \n"
"{\n"
"	border-radius: 5px;\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"         stop:0 #4a90e2, \n"
"         stop:1 #57d9a3);\n"
"}"));
        verticalLayout = new QVBoxLayout(AssessmentReportPage);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        headerWidget = new QWidget(AssessmentReportPage);
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

        scrollArea = new QScrollArea(AssessmentReportPage);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 375, 756));
        verticalLayout_2 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_2->setSpacing(16);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(16, 16, 16, 16);
        scoreCard = new QWidget(scrollAreaWidgetContents);
        scoreCard->setObjectName(QStringLiteral("scoreCard"));
        scoreCard->setStyleSheet(QLatin1String("QWidget#scoreCard {\n"
"    background-color: white;\n"
"    border-radius: 12px;\n"
"    padding: 20px;\n"
"}"));
        verticalLayout_3 = new QVBoxLayout(scoreCard);
        verticalLayout_3->setSpacing(12);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        label = new QLabel(scoreCard);
        label->setObjectName(QStringLiteral("label"));
        label->setStyleSheet(QLatin1String("QLabel {\n"
"    font-size: 14px;\n"
"    color: #999999;\n"
"}"));
        label->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(label);

        scoreLabel = new QLabel(scoreCard);
        scoreLabel->setObjectName(QStringLiteral("scoreLabel"));
        scoreLabel->setStyleSheet(QLatin1String("QLabel {\n"
"    font-size: 48px;\n"
"    font-weight: bold;\n"
"    color: #333333;\n"
"}"));
        scoreLabel->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(scoreLabel);

        scoreProgressBar = new QProgressBar(scoreCard);
        scoreProgressBar->setObjectName(QStringLiteral("scoreProgressBar"));
        scoreProgressBar->setMaximumSize(QSize(16777215, 10));
        scoreProgressBar->setValue(75);
        scoreProgressBar->setTextVisible(false);

        verticalLayout_3->addWidget(scoreProgressBar);

        riskLevelLabel = new QLabel(scoreCard);
        riskLevelLabel->setObjectName(QStringLiteral("riskLevelLabel"));
        riskLevelLabel->setStyleSheet(QLatin1String("QLabel {\n"
"    font-size: 20px;\n"
"    font-weight: bold;\n"
"    color: #07C160;\n"
"}"));
        riskLevelLabel->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(riskLevelLabel);


        verticalLayout_2->addWidget(scoreCard);

        suggestionCard = new QWidget(scrollAreaWidgetContents);
        suggestionCard->setObjectName(QStringLiteral("suggestionCard"));
        suggestionCard->setStyleSheet(QLatin1String("QWidget#suggestionCard {\n"
"    background-color: white;\n"
"    border-radius: 12px;\n"
"    padding: 16px;\n"
"}"));
        verticalLayout_4 = new QVBoxLayout(suggestionCard);
        verticalLayout_4->setSpacing(8);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        label_2 = new QLabel(suggestionCard);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setStyleSheet(QLatin1String("QLabel {\n"
"    font-size: 16px;\n"
"    font-weight: bold;\n"
"    color: #333333;\n"
"}"));

        verticalLayout_4->addWidget(label_2);

        suggestionText = new QLabel(suggestionCard);
        suggestionText->setObjectName(QStringLiteral("suggestionText"));
        suggestionText->setStyleSheet(QLatin1String("QLabel {\n"
"    font-size: 14px;\n"
"    color: #666666;\n"
"    line-height: 1.5;\n"
"}"));
        suggestionText->setWordWrap(true);

        verticalLayout_4->addWidget(suggestionText);


        verticalLayout_2->addWidget(suggestionCard);

        viewHistoryButton = new QPushButton(scrollAreaWidgetContents);
        viewHistoryButton->setObjectName(QStringLiteral("viewHistoryButton"));
        viewHistoryButton->setMinimumSize(QSize(0, 44));

        verticalLayout_2->addWidget(viewHistoryButton);

        dimensionsWidget = new QWidget(scrollAreaWidgetContents);
        dimensionsWidget->setObjectName(QStringLiteral("dimensionsWidget"));
        dimensionsWidget->setStyleSheet(QLatin1String("QWidget#dimensionsWidget {\n"
"    background-color: white;\n"
"    border-radius: 12px;\n"
"    padding: 16px;\n"
"}"));
        verticalLayout_5 = new QVBoxLayout(dimensionsWidget);
        verticalLayout_5->setSpacing(12);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        label_3 = new QLabel(dimensionsWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setStyleSheet(QLatin1String("QLabel {\n"
"    font-size: 16px;\n"
"    font-weight: bold;\n"
"    color: #333333;\n"
"}"));

        verticalLayout_5->addWidget(label_3);

        dimensionsContent = new QWidget(dimensionsWidget);
        dimensionsContent->setObjectName(QStringLiteral("dimensionsContent"));

        verticalLayout_5->addWidget(dimensionsContent);


        verticalLayout_2->addWidget(dimensionsWidget);

        trendsWidget = new QWidget(scrollAreaWidgetContents);
        trendsWidget->setObjectName(QStringLiteral("trendsWidget"));
        trendsWidget->setStyleSheet(QLatin1String("QWidget#trendsWidget {\n"
"    background-color: white;\n"
"    border-radius: 12px;\n"
"    padding: 16px;\n"
"}"));
        verticalLayout_6 = new QVBoxLayout(trendsWidget);
        verticalLayout_6->setSpacing(12);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        label_4 = new QLabel(trendsWidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setStyleSheet(QLatin1String("QLabel {\n"
"    font-size: 16px;\n"
"    font-weight: bold;\n"
"    color: #333333;\n"
"}"));

        verticalLayout_6->addWidget(label_4);

        trendsContent = new QWidget(trendsWidget);
        trendsContent->setObjectName(QStringLiteral("trendsContent"));

        verticalLayout_6->addWidget(trendsContent);


        verticalLayout_2->addWidget(trendsWidget);

        indicatorsWidget = new QWidget(scrollAreaWidgetContents);
        indicatorsWidget->setObjectName(QStringLiteral("indicatorsWidget"));
        indicatorsWidget->setStyleSheet(QLatin1String("QWidget#indicatorsWidget {\n"
"    background-color: white;\n"
"    border-radius: 12px;\n"
"    padding: 16px;\n"
"}"));
        verticalLayout_7 = new QVBoxLayout(indicatorsWidget);
        verticalLayout_7->setSpacing(12);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        label_5 = new QLabel(indicatorsWidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setStyleSheet(QLatin1String("QLabel {\n"
"    font-size: 16px;\n"
"    font-weight: bold;\n"
"    color: #333333;\n"
"}"));

        verticalLayout_7->addWidget(label_5);

        indicatorsContent = new QWidget(indicatorsWidget);
        indicatorsContent->setObjectName(QStringLiteral("indicatorsContent"));

        verticalLayout_7->addWidget(indicatorsContent);


        verticalLayout_2->addWidget(indicatorsWidget);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollArea);


        retranslateUi(AssessmentReportPage);

        QMetaObject::connectSlotsByName(AssessmentReportPage);
    } // setupUi

    void retranslateUi(QWidget *AssessmentReportPage)
    {
        AssessmentReportPage->setWindowTitle(QApplication::translate("AssessmentReportPage", "\346\265\213\350\257\204\346\212\245\345\221\212", Q_NULLPTR));
        backButton->setText(QString());
        titleLabel->setText(QApplication::translate("AssessmentReportPage", "\346\265\213\350\257\204\346\212\245\345\221\212", Q_NULLPTR));
        label->setText(QApplication::translate("AssessmentReportPage", "\346\202\250\347\232\204\345\276\227\345\210\206", Q_NULLPTR));
        scoreLabel->setText(QApplication::translate("AssessmentReportPage", "0.0", Q_NULLPTR));
        riskLevelLabel->setText(QApplication::translate("AssessmentReportPage", "\347\212\266\346\200\201\350\211\257\345\245\275", Q_NULLPTR));
        label_2->setText(QApplication::translate("AssessmentReportPage", "\360\237\222\241 \344\270\252\346\200\247\345\214\226\345\273\272\350\256\256", Q_NULLPTR));
        suggestionText->setText(QApplication::translate("AssessmentReportPage", "\345\273\272\350\256\256\345\206\205\345\256\271\345\260\206\345\234\250\350\277\231\351\207\214\346\230\276\347\244\272...", Q_NULLPTR));
        viewHistoryButton->setText(QApplication::translate("AssessmentReportPage", "\346\237\245\347\234\213\345\216\206\345\217\262\350\266\213\345\212\277", Q_NULLPTR));
        label_3->setText(QApplication::translate("AssessmentReportPage", "\360\237\223\212 \347\273\264\345\272\246\345\210\206\346\236\220", Q_NULLPTR));
        label_4->setText(QApplication::translate("AssessmentReportPage", "\360\237\223\210 \346\203\205\347\273\252\350\266\213\345\212\277", Q_NULLPTR));
        label_5->setText(QApplication::translate("AssessmentReportPage", "\360\237\216\257 \345\205\263\351\224\256\346\214\207\346\240\207", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class AssessmentReportPage: public Ui_AssessmentReportPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ASSESSMENTREPORTPAGE_H
