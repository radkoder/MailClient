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
    connect(ui->nextButton,&QPushButton::clicked,[this](bool c){
        pageNum++;
        mailBox.fetchInfo(10,10*pageNum);
        mailBox.onFetchReady([this]()
        {
           mailModel.setMails(mailBox.getLatest(10,10*pageNum));
        });

    });
    connect(ui->prevButton,&QPushButton::clicked,[this](bool c){
        pageNum--;
        if(pageNum<0)pageNum=0;
        mailModel.setMails(mailBox.getLatest(10,10*pageNum));
    });
    //get credentials
    LoginDialog ldiag;
    if(ldiag.exec() == QDialog::Accepted)
    {
        mailBox.open(ldiag.loginData.server);
        mailBox.login(ldiag.loginData.mail,ldiag.loginData.pass);
        mailBox.select("INBOX");
        mailBox.fetchInfo(20);
    }else
    {
        dconsole->write("nie zalogowany");
    }
    dconsole->show();
    ui->mailListView->setModel(&mailModel);
    ui->mailListView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mailBox.onFetchReady([this]()
    {
       mailModel.setMails(mailBox.getLatest(10));
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}


