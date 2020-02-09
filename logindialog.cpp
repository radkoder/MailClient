#include "logindialog.h"
#include "ui_logindialog.h"
#include <QLineEdit>
#include <QMessageBox>
LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::accept()
{
    if(!checkInput()) return;

    loginData.server = ui->serverInput->text();
    loginData.mail = ui->mailInput->text();
    loginData.pass = ui->passInput->text();
    done(QDialog::Accepted);
}

void LoginDialog::reject()
{
    QMessageBox::warning(this,"Uwaga","Jeżeli przejdziesz dalej bez zalogowania, nie będziesz mógł przeglądać maila");
    done(QDialog::Rejected);
}

bool LoginDialog::checkInput()
{
    if(ui->serverInput->text().length() == 0)
    {
        ui->errLabel->setText("Proszę podać adres serwera");
        return false;
    }
    else if(ui->mailInput->text().length() == 0)
    {
        ui->errLabel->setText("Proszę podać poprawny login");
        return false;
    }
    else if(ui->passInput->text().length() == 0)
    {
        ui->errLabel->setText("Proszę podać hasło");
        return false;
    }
    else
    {
        return true;
    }
}

