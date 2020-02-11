#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}
struct LoginData
{
    QString server,mail,pass;
};

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog() override;

    LoginData loginData;
public slots:
    void accept() override;
    void reject() override;
private:
    bool checkInput();
    Ui::LoginDialog *ui;

};

#endif // LOGINDIALOG_H
