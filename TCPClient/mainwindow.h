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
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QTcpSocket  *tcpClient;   //socket
    QLabel  *labSocketState;  //状态栏显示标签
    QSqlDatabase  DB;   //数据库

    QSqlQueryModel  *qryModel;  //数据模型
    QString  getLocalIP();    //获取本机IP地址
    QString clientId;
    QString userName = "";
    void createId(); //create unique ID
    bool getNameFromServer(QString cliendId); // get Name data from server
    void setIdLabel();
    void setNameLabel();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void openDatabase();


private slots:
//自定义槽函数
    void    do_connected();
    void    do_disconnected();
    void    do_socketStateChange(QAbstractSocket::SocketState socketState);
    void    do_socketReadyRead();//读取socket传入的数据
//
    void on_actConnect_triggered();

    void on_actDisconnect_triggered();

    void on_actClear_triggered();

    void on_btnSend_clicked();
    void on_btnChangeName_clicked(); // change user name btn

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
