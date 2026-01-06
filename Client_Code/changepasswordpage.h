#ifndef CHANGEPASSWORDPAGE_H
#define CHANGEPASSWORDPAGE_H

#include <QWidget>
#include <QString>

namespace Ui {
class ChangePasswordPage;
}

class ChangePasswordPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChangePasswordPage(QWidget *parent = nullptr);
    ~ChangePasswordPage();

    void setUserId(const QString& userId);

signals:
    void backToSettings();
    void passwordChanged();

private slots:
    void onBackClicked();
    void onSubmitClicked();

private:
    Ui::ChangePasswordPage *ui;
    QString m_userId;

    bool validateInputs();
    void clearInputs();
};

#endif // CHANGEPASSWORDPAGE_H