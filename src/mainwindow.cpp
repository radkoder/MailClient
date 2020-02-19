#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "debugconsole.h"
#include "logindialog.h"
#include "ui_mailbodyview.h"
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
        //OMG JUST LIKE RUST XDD
        auto res = mailBox.fetchInfo(10,10*pageNum)
                .onReady([this](auto msg){
                mailModel.setMails(mailBox.getLatest(10,10*pageNum));
        });
    });
    connect(ui->prevButton,&QPushButton::clicked,[this](bool c){
        pageNum--;
        if(pageNum<0)pageNum=0;
        mailModel.setMails(mailBox.getLatest(10,10*pageNum));
    });
    connect(ui->mailListView,&QTableView::doubleClicked,[this](const QModelIndex& index){
        auto uid = index.data(Qt::UserRole);
        mailBox.fetchBody(uid.toString()).onReady([&,this](auto msg)
        {
            auto* mailview = new QWidget(this);
            Ui_Form mui;
            mui.setupUi(mailview);
            mui.bodyText->setHtml("<b>B</b>igga");
            mailview->show();
        });
    });
    //get credentials
    LoginDialog ldiag;
    if(ldiag.exec() == QDialog::Accepted)
    {
        mailBox.open(ldiag.loginData.server);
        mailBox.login(ldiag.loginData.mail,ldiag.loginData.pass);
        mailBox.select("INBOX");
        mailBox.fetchInfo(20).onReady([this](auto msg)
        {
           mailModel.setMails(mailBox.getLatest(10));
        });
    }else
    {
        dconsole->write("nie zalogowany");
    }
    dconsole->show();
    ui->mailListView->setModel(&mailModel);
    ui->mailListView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

MainWindow::~MainWindow()
{
    delete ui;
}


