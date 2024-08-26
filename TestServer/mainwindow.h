#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include    <QTcpServer>
#include    <QLabel>
#include    <QtSql>
#include    <QDataWidgetMapper>
#include <QUuid>
#include <QFile>
#include    <QtNetwork>
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
    QSqlDatabase  DB;   //数据库
    QSqlQueryModel  *qryModel;  //数据模型
    QTcpServer *tcpServer;      //TCP服务器
    QTcpSocket *tcpSocket=nullptr;      //TCP通讯的Socket

    QString IP = "127.0.0.1";
    quint16 port = 2000;

    QMap<QString, QTcpSocket*> clients;
    QMap<int, QString> chatRooms;

    void openDatabase();
    void initializeDatabase();
    //void saveClientInfo(QString id, QString name);
    void closeDatabase();
    void activeServer();
    void registerClient(const QString& clientId, const QString& userName);
    void updateUserName(const QString& clientId, const QString& newUserName);
    QString queryUserNameFromDatabase(QString clientId);
    QString getClientIdFromSocket(QTcpSocket* socket);
    int createChatRoom(const QString& creatorId, const QString& roomName);
    bool addUserToChatRoom(int roomId, const QString& userId);
    bool removeUserFromChatRoom(int roomId, const QString& userId);
    QList<QString> getChatRoomUsers(int roomId);
    QString getChatRoomName(const QString& roomId);


private slots:
    //自定义槽函数
    void    do_newConnection();         //关联QTcpServer的newConnection()信号
    void    do_clientConnected();       //客户端 socket 已连接
    void    do_clientDisconnected();    //客户端 socket 已断开
    void    do_socketReadyRead();       //读取socket传入的数据



};
#endif // MAINWINDOW_H
