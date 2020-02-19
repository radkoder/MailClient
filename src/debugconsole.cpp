#include "debugconsole.h"
#include "ui_debugconsole.h"
#include <QScrollBar>
#include <utility>
DebugConsole::DebugConsole(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugConsole)
{
    ui->setupUi(this);
}

DebugConsole::~DebugConsole()
{
    delete ui;
}

void DebugConsole::write(QString line)
{
    line.replace("\r","[CR]").replace("\n","[LF]").append('\n');
    QTextCursor cursor(ui->output->textCursor());
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(line);
    ui->output->verticalScrollBar()->setValue(ui->output->verticalScrollBar()->maximum());
}
void DebugConsole::writeUrgent(QString line)
{
    write("<span color=red>");
    write(std::move(line));
    write("</span>");
}
