#include "mainwindow.h"
#include "ui_mainwindow.h"
#include    <QtNetwork>


QString MainWindow::getLocalIP()
{//获取本机IPv4地址
    QString hostName=QHostInfo::localHostName();    //本地主机名
    QHostInfo hostInfo=QHostInfo::fromName(hostName);
    QString   localIP="";

    QList<QHostAddress> addList=hostInfo.addresses();  //本机IP地址列表
    if (addList.isEmpty())
        return localIP;

    foreach(QHostAddress aHost, addList)
        if (QAbstractSocket::IPv4Protocol==aHost.protocol())
        {
            localIP=aHost.toString();
            break;
        }
    return localIP;
}

void MainWindow::openDatabase() {
    DB=QSqlDatabase::addDatabase("QSQLITE");  //添加 SQLITE数据库驱动
    DB.setDatabaseName("db.sqlite");   //设置数据库文件

    if (!DB.open()) {
        qDebug() << "Failed to connect to database:" << DB.lastError();
    }
}

void MainWindow::initializeDatabase() {
    QSqlQuery query(DB);
    //execute create table
    if (!query.exec
         ("CREATE TABLE IF NOT EXISTS users (id TEXT PRIMARY KEY, name TEXT)"))
    {
        qDebug() << "Failed to create table:" << query.lastError();
    }
}

void MainWindow::closeDatabase() {
    if (DB.isOpen()) {
        DB.close();
    }
}

//void MainWindow::closeEvent(QCloseEvent *event)
//{//关闭窗口时停止监听
//    if (tcpServer->isListening())
//        tcpServer->close();;//停止网络监听
//    event->accept();
//}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    openDatabase();
    initializeDatabase();

    labListen=new QLabel("监听状态:");
    labListen->setMinimumWidth(150);
    ui->statusBar->addWidget(labListen);

    labSocketState=new QLabel("Socket状态：");
    labSocketState->setMinimumWidth(200);
    ui->statusBar->addWidget(labSocketState);

    QString localIP=getLocalIP();   //获取本机IP
    this->setWindowTitle(this->windowTitle()+"----本机IP地址："+localIP);
    ui->comboIP->addItem(localIP);

    //create new QTcpServer
    tcpServer=new QTcpServer(this);
    //client가 서버에 접속할 경우, slot method 실행
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(do_newConnection()));


}

MainWindow::~MainWindow()
{//析构函数
    if (tcpSocket != nullptr)
    {
        if (tcpSocket->state()==QAbstractSocket::ConnectedState)
            tcpSocket->disconnectFromHost();    //断开与客户端的连接
    }

    if (tcpServer->isListening())
        tcpServer->close();     //停止网络监听

    closeDatabase();
    delete ui;
}

//client가 server에 접속 시
void MainWindow::do_newConnection()
{
    //새 client와 통신할 QTcpSocket 생성
    tcpSocket = tcpServer->nextPendingConnection(); //创建socket

    //QTcpSocket과 연결 완료 시 slot method 실행
    connect(tcpSocket, SIGNAL(connected()),this, SLOT(do_clientConnected()));
    do_clientConnected();   //执行一次槽函数，显示状态

    connect(tcpSocket, SIGNAL(disconnected()),this, SLOT(do_clientDisconnected()));

    connect(tcpSocket,&QTcpSocket::stateChanged,this,&MainWindow::do_socketStateChange);
    do_socketStateChange(tcpSocket->state());   //执行一次槽函数，显示状态

    //소켓에 새로운 데이터가 도착했을 때 slot method 실행
    connect(tcpSocket,SIGNAL(readyRead()),  this,SLOT(do_socketReadyRead()));
}

void MainWindow::do_socketStateChange(QAbstractSocket::SocketState socketState)
{//socket状态变化时
    switch(socketState)
    {
    case QAbstractSocket::UnconnectedState:
        labSocketState->setText("socket状态：UnconnectedState");
        break;
    case QAbstractSocket::HostLookupState:
        labSocketState->setText("socket状态：HostLookupState");
        break;
    case QAbstractSocket::ConnectingState:
        labSocketState->setText("socket状态：ConnectingState");
        break;
    case QAbstractSocket::ConnectedState:
        labSocketState->setText("socket状态：ConnectedState");
        break;
    case QAbstractSocket::BoundState:
        labSocketState->setText("socket状态：BoundState");
        break;
    case QAbstractSocket::ClosingState:
        labSocketState->setText("socket状态：ClosingState");
        break;
    case QAbstractSocket::ListeningState:
        labSocketState->setText("socket状态：ListeningState");
    }
}

void MainWindow::do_clientConnected()
{//客户端接入时
    ui->textEdit->appendPlainText("**client socket connected");
    ui->textEdit->appendPlainText("**peer address:"+
                                 tcpSocket->peerAddress().toString());
    ui->textEdit->appendPlainText("**peer port:"+
                                 QString::number(tcpSocket->peerPort()));
}

void MainWindow::do_clientDisconnected()
{//客户端断开连接时
    ui->textEdit->appendPlainText("**client socket disconnected");
    tcpSocket->deleteLater();
}

void MainWindow::do_socketReadyRead()
{//读取缓冲区的行文本
    while(tcpSocket->canReadLine()) {

        QString line = tcpSocket->readLine().trimmed();
        ui->textEdit->appendPlainText("[in] "+line);

        QStringList parts = line.split(';');
        QString command = parts[0].split(':')[0];

        if (command == "REGISTER") {
            QString clientId = parts[0].split(':')[1];
            QString userName = parts[1].split(':')[1];
            registerClient(clientId, userName);
        }
        else if (command == "UPDATE_NAME") {
            QString clientId = parts[0].split(':')[1];
            QString newUserName = parts[1].split(':')[1];
            updateUserName(clientId, newUserName);
        }
    }
}

void MainWindow::registerClient(const QString& clientId, const QString& userName) {
    QSqlQuery query(DB);
    query.prepare("INSERT INTO users (id, name) VALUES (:client_id, :user_name)");
    query.bindValue(":client_id", clientId);
    query.bindValue(":user_name", userName);
    if (!query.exec()) {
        qDebug() << "Failed to register client:" << query.lastError();
    }
}

void MainWindow::updateUserName(const QString& clientId, const QString& newUserName) {
    QSqlQuery query(DB);
    query.prepare("UPDATE users SET name = :user_name WHERE id = :client_id");
    query.bindValue(":user_name", newUserName);
    query.bindValue(":client_id", clientId);
    if (!query.exec()) {
        qDebug() << "Failed to update user name:" << query.lastError();
    }
    else {
        QString updateText = "Update UserName into" + newUserName;

        //tcpSocket->write("Update UserName into" + newUserName +"\n");
        QByteArray  str=updateText.toUtf8();
        str.append('\n');       //添加一个换行符
        tcpSocket->write(str);
    }
}



void MainWindow::on_actStart_triggered()
{//"开始监听"按钮
    QString  IP=ui->comboIP->currentText();  //IP地址字符串,如"127.0.0.1"
    quint16  port=ui->spinPort->value();     //端口
    QHostAddress   address(IP);
    tcpServer->listen(address,port);    //开始监听
    //    tcpServer->listen(QHostAddress::LocalHost,port);// Equivalent to QHostAddress("127.0.0.1").
    ui->textEdit->appendPlainText("**开始监听...");
    ui->textEdit->appendPlainText("**服务器地址："+tcpServer->serverAddress().toString());
    ui->textEdit->appendPlainText("**服务器端口："+QString::number(tcpServer->serverPort()));

    ui->actStart->setEnabled(false);
    ui->actStop->setEnabled(true);
    labListen->setText("监听状态：正在监听");
}

void MainWindow::on_actStop_triggered()
{//"停止监听"按钮
    if (tcpServer->isListening())   //tcpServer正在监听
    {
        if (tcpSocket != nullptr)
            if (tcpSocket->state()==QAbstractSocket::ConnectedState)
                tcpSocket->disconnectFromHost();
        tcpServer->close();         //停止监听
        ui->actStart->setEnabled(true);
        ui->actStop->setEnabled(false);
        labListen->setText("监听状态：已停止监听");
    }
}

void MainWindow::on_actClear_triggered()
{
    ui->textEdit->clear();
}

void MainWindow::on_btnSend_clicked()
{//"发送消息"按钮，发送一行字符串，以换行符结束
    QString  msg=ui->editMsg->text();
    ui->textEdit->appendPlainText("[out] "+msg);
    ui->editMsg->clear();
    ui->editMsg->setFocus();

    QByteArray  str=msg.toUtf8();
    str.append('\n');       //添加一个换行符
    tcpSocket->write(str);
}

void MainWindow::on_actHostInfo_triggered()
{//获取本机地址, 用于测试
    QString hostName=QHostInfo::localHostName();//本地主机名
    ui->textEdit->appendPlainText("本机主机名："+hostName+"\n");
    QHostInfo   hostInfo=QHostInfo::fromName(hostName);
    QList<QHostAddress> addList=hostInfo.addresses();
    if (addList.isEmpty())
        return;

    foreach (QHostAddress aHost, addList)
        if (QAbstractSocket::IPv4Protocol==aHost.protocol())
        {
            QString IP=aHost.toString();
            ui->textEdit->appendPlainText("本机IP地址："+aHost.toString());
            if (ui->comboIP->findText(IP)<0)
                ui->comboIP->addItem(IP);
        }
}

