#include "mainwindow.h"

#include <QApplication>
#include "imap_message.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<imap::Request>("Request");
    MainWindow w;
    w.show();
    return a.exec();
}
