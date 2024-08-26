#ifndef CHATROOMUI_H
#define CHATROOMUI_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStringList>
#include <QTabWidget>
#include <QTextOption>

namespace Ui {
class chatroomui;
}

class chatroomui : public QWidget
{
    Q_OBJECT

public:
    explicit chatroomui(const QString &roomId, const QString &roomName, const QStringList &userNames, QWidget *parent = nullptr);
    QString getRoomId() const;


    ~chatroomui();


private:
    Ui::chatroomui *ui;
    QString roomId;
    QString roomName;
    QStringList userNames;
};

#endif // CHATROOMUI_H
