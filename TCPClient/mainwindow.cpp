#include "mainwindow.h"
#include "ui_mainwindow.h"

#include    <QHostAddress>
#include    <QHostInfo>
#include <QUuid>
//get IpV4 address
QString MainWindow::getLocalIP()
{
    //sendId
    //자신 host의 이름 가져오기
    QString hostName=QHostInfo::localHostName();  //本地主机名

    //자신 host 이름으로 QHostInfo 생성
    QHostInfo   hostInfo=QHostInfo::fromName(hostName);

    QString   localIP="";
    //hostName에 있는 모든 주소들 가져와 저장
    QList<QHostAddress> addList=hostInfo.addresses();

    //hostName이 비어 있다면 빈 문자열인 localIP 반환
    if (addList.isEmpty())
        return localIP;

    //addList에 있는 QHostAddress들에 접근
    foreach(QHostAddress aHost, addList)

        //IpV4 프로토콜이라면
        if (QAbstractSocket::IPv4Protocol==aHost.protocol())
        {
            //ip주소를 문자열로 변환 후 돌려주기
            localIP=aHost.toString();
            break;
        }
    return localIP;
}

bool MainWindow::getNameFromServer(QString clientId) {

    if (tcpClient->state() == QAbstractSocket::ConnectedState) {
        QByteArray queryRequest = "QUERY_NAME:" + clientId.toUtf8() + "\n";
        tcpClient->write(queryRequest);

        QEventLoop loop;

        connect(tcpClient, &QTcpSocket::readyRead, &loop, &QEventLoop::quit);
        QTimer::singleShot(5000, &loop, &QEventLoop::quit); // set timer
        loop.exec(); // wait for server response

        while (tcpClient->canReadLine()) {
            QString line = tcpClient->readLine().trimmed();
            ui->textEdit->appendPlainText("[in] " + line);

            QStringList parts = line.split(';');
            QString command = parts[0].split(':')[0];

            if (command == "QUERY_NAME_RESULT") {
                QString resultClientId = parts[0].split(':')[1];
                QString resultUserName = parts[1].split(':')[1];

                if (resultUserName == "null") {
                    this->userName = clientId;
                }
                else {
                    this->userName = resultUserName;
                }

                return true;
            }
        }
        return true;
    }
    return false;
}

//create unique Id and save at local
void MainWindow::createId()
{
    QString clientId;
    QFile file("client_id.txt"); //set file

    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        clientId = file.readAll().trimmed();
    }

    else {
        clientId = QUuid::createUuid().toString(); //create u id?
        if (file.open(QIODevice::WriteOnly)) {
            file.write(clientId.toUtf8());
            file.close();
        }

    }
    //set client id
    this->clientId = clientId;
    // //set default user name as client id
    // QString userName = clientId;

    //이 아래에 사용자 이름을 서버에서 가지고 와, 사용자 이름 변수에 저장하는 과정 필요
    if (!getNameFromServer(this->clientId)) {
        this->userName = clientId;
    }

}

void MainWindow::openDatabase()
{
    DB=QSqlDatabase::addDatabase("QSQLITE");  //添加 SQLITE数据库驱动
    DB.setDatabaseName("db");   //设置数据库文件

    if (!DB.open()) {
        qDebug() << "Failed to connect to database:" << DB.lastError();
    }

}

//change user name and update it to server
void MainWindow::on_btnChangeName_clicked() {

    QRegularExpression invalidCharPattern("^[a-zA-Z0-9_]+$");

    QString newUserName = ui->editUserName->text();

    if (newUserName.isEmpty()) {
        newUserName = clientId;
    }

    QRegularExpressionMatch match = invalidCharPattern.match(newUserName);

    if (!match.hasMatch()) {
        ui->textEdit->appendPlainText("**Error: User name cannot contain special characters like ';' or ':'.");
        return;
    }

    //this->userName = newUserName;

    QByteArray changeRequest = "UPDATE_NAME:" + clientId.toUtf8() + ";NAME:" + newUserName.toUtf8() + "\n";
    tcpClient->write(changeRequest);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //create QTcpSocket
    tcpClient=new QTcpSocket(this);    //创建socket变量

    //create QLabel, add into statusBar
    labSocketState=new QLabel("Socket状态：");  //状态栏标签
    labSocketState->setMinimumWidth(250);
    ui->statusBar->addWidget(labSocketState);

    //get local Ip
    QString localIP=getLocalIP();      //获取本机IP
    this->setWindowTitle(this->windowTitle()+"----本机IP地址："+localIP);

    //add local Ip in comboBox
    ui->comboServer->addItem(localIP);

    //QTcpSocket과 슬롯 함수 연결, 각각 연결 시, 비연결 시, 상태 변환 시, 읽기 가능 시의 경우
    connect(tcpClient,SIGNAL(connected()),   this,SLOT(do_connected()));
    connect(tcpClient,SIGNAL(disconnected()),this,SLOT(do_disconnected()));

    connect(tcpClient,&QTcpSocket::stateChanged,this,&MainWindow::do_socketStateChange);

    connect(tcpClient,SIGNAL(readyRead()), this,SLOT(do_socketReadyRead()));
}

MainWindow::~MainWindow()
{
//    if (tcpClient->state()==QAbstractSocket::ConnectedState)
//        tcpClient->disconnectFromHost();
    delete ui;
}

//tcp연결이 성공적으로 되었을 때, text 표시로 이를 화면에 나타내고 연결 버튼 비활성화, 끊기 버튼 활성화
void MainWindow::do_connected()
{ //connected()信号的槽函数
    ui->textEdit->appendPlainText("**已连接到服务器");
    ui->textEdit->appendPlainText("**peer address:"+
                                   tcpClient->peerAddress().toString());
    ui->textEdit->appendPlainText("**peer port:"+
                                   QString::number(tcpClient->peerPort()));
    ui->actConnect->setEnabled(false);
    ui->actDisconnect->setEnabled(true);

    createId();
    QByteArray registrationRequest = "REGISTER:" + clientId.toUtf8() + ";NAME:" + userName.toUtf8() + "\n";
    //ui->textEdit->appendPlainText("[out] "+registrationRequest);
    tcpClient->write(registrationRequest); // send id and name data to server

    setIdLabel();
    setNameLabel();
}

//통신 끊어졌을 때, 연결 버튼 활성화, 비연결 버튼 비활성화
void MainWindow::do_disconnected()
{//disConnected()信号的槽函数
    ui->textEdit->appendPlainText("**已断开与服务器的连接");
    ui->actConnect->setEnabled(true);
    ui->actDisconnect->setEnabled(false);
}

void MainWindow::do_socketReadyRead()
{//readyRead()信号的槽函数
    while(tcpClient->canReadLine()) {
        QString line = tcpClient->readLine().trimmed();
        //ui->textEdit->appendPlainText("[in] "+line);

        QStringList parts = line.split(';');
        QString command = parts[0].split(':')[0];

        if (command == "UPDATE_NAME_RESULT") {
            QString status = parts[0].split(':')[1];

            if (status == "SUCCESS") {
                QString updatedUserName = parts[1].split(':')[1];
                this->userName = updatedUserName;
                //ui->textEdit->appendPlainText("**User name updated successfully to " + updatedUserName);
                //setNameLabel();

            } else {
                //ui->textEdit->appendPlainText("**Error: Failed to update user name.");
            }
        }
    }
}

//socket의 상태가 바뀔 때 그 상태를 text로 ui 화면에 띄우는 함수
void MainWindow::do_socketStateChange(QAbstractSocket::SocketState socketState)
{//stateChange()信号的槽函数
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

void MainWindow::on_actConnect_triggered()
{//“连接服务器”按钮

    QString   addr=ui->comboServer->currentText();
    quint16   port=ui->spinPort->value();
    //addr ip 주소와 port 의 포트 번호를 가진 서버에 연결 요청을 보냄
    tcpClient->connectToHost(addr,port);
    //    tcpClient->connectToHost(QHostAddress::LocalHost,port);
}

void MainWindow::on_actDisconnect_triggered()
{//“断开连接”按钮
    if (tcpClient->state()==QAbstractSocket::ConnectedState)
        tcpClient->disconnectFromHost();
}

void MainWindow::on_actClear_triggered()
{
    ui->textEdit->clear();
}

void MainWindow::on_btnSend_clicked()
{//“发送数据”按钮
    QString  msg=ui->editMsg->text();
    ui->textEdit->appendPlainText(userName + " [out]: "+msg);
    ui->editMsg->clear();
    ui->editMsg->setFocus();

    QByteArray  str=msg.toUtf8();
    str.append('\n');
    tcpClient->write(str);
}

void MainWindow::setIdLabel(){
    ui->userIdLabel->setText("User ID : " + clientId);
}

void MainWindow::setNameLabel(){
    ui->userNameLabel->setText("User Name : " + userName);
}


