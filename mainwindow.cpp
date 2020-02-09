#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "debugconsole.h"
#include "logindialog.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dconsole = new DebugConsole(this);

    connect(&mailBox,&imap::MailBox::log,dconsole,&DebugConsole::write);
    connect(&mailBox,&imap::MailBox::error,dconsole,&DebugConsole::writeUrgent);

    //get credentials
    LoginDialog ldiag;
    if(ldiag.exec() == QDialog::Accepted)
    {
        mailBox.open(ldiag.loginData.server);
        mailBox.login(ldiag.loginData.mail,ldiag.loginData.pass);
        mailBox.select("INBOX");
        mailBox.fetchLatest(10);
    }else
    {
        dconsole->write("nie zalogowany");
    }
    dconsole->show();

}

MainWindow::~MainWindow()
{
    delete ui;
}

