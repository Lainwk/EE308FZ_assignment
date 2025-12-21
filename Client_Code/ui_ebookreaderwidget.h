/********************************************************************************
** Form generated from reading UI file 'ebookreaderwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.9.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EBOOKREADERWIDGET_H
#define UI_EBOOKREADERWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_EBookReaderWidget
{
public:

    void setupUi(QWidget *EBookReaderWidget)
    {
        if (EBookReaderWidget->objectName().isEmpty())
            EBookReaderWidget->setObjectName(QStringLiteral("EBookReaderWidget"));
        EBookReaderWidget->resize(400, 600);
        EBookReaderWidget->setMinimumSize(QSize(400, 0));
        EBookReaderWidget->setMaximumSize(QSize(400, 16777215));

        retranslateUi(EBookReaderWidget);

        QMetaObject::connectSlotsByName(EBookReaderWidget);
    } // setupUi

    void retranslateUi(QWidget *EBookReaderWidget)
    {
        EBookReaderWidget->setWindowTitle(QApplication::translate("EBookReaderWidget", "\347\224\265\345\255\220\344\271\246\351\230\205\350\257\273\345\231\250", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class EBookReaderWidget: public Ui_EBookReaderWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EBOOKREADERWIDGET_H
