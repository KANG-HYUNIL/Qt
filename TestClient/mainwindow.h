#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include    <QTcpSocket>
#include    <QLabel>
#include    <QtSql>
#include    <QDataWidgetMapper>
#include <QUuid>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTcpSocket  *tcpClient;   //socket
    QLabel  *labSocketState;  //状态栏显示标签
    QString clientId;
    QString userName = "";
    QString addr = "127.0.0.1";
    quint16 port = 2000;


    void connectServer();
    void createId(); //create unique ID
    QString getNameFromServer(QString cliendId); // get Name data from server
    void setIdLabel();
    void setNameLabel();
    void showNewChatRoom(QString& roomId, QString& roomName);

private slots:
    void    do_connected();
    void on_btnSend_clicked();
    void on_btnChangeName_clicked(); // change user name btn
    void do_socketReadyRead();//读取socket传入的数据
    void on_btnCreateRoom_clicked();
    void    do_socketStateChange(QAbstractSocket::SocketState socketState);

};
#endif // MAINWINDOW_H
