#ifndef DEBUGCONSOLE_H
#define DEBUGCONSOLE_H

#include <QDialog>
#include <QColor>
namespace Ui {
class DebugConsole;
}

class DebugConsole : public QDialog
{
    Q_OBJECT

public:
    explicit DebugConsole(QWidget *parent = nullptr);
    ~DebugConsole();
public slots:
    void write(QString line);
    void writeUrgent(QString line);
private:
    Ui::DebugConsole *ui;
};

#endif // DEBUGCONSOLE_H
