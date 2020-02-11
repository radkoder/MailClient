#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QMainWindow>
#include "imap_mailbox.h"
#include "debugconsole.h"
#include "maillistmodel.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    //virtual void resizeEvent(QResizeEvent* event) override;
private:
    Ui::MainWindow *ui;
    imap::MailBox mailBox;
    DebugConsole* dconsole;
    MailListModel mailModel;
    int pageNum=0;
};
#endif // MAINWINDOW_H
