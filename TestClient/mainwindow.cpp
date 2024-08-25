#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //create QTcpSocket
    tcpClient=new QTcpSocket(this);    //创建socket变量
    connectServer();
    connect(tcpClient,SIGNAL(connected()),   this,SLOT(do_connected()));
    connect(tcpClient,SIGNAL(readyRead()), this,SLOT(do_socketReadyRead()));
    connect(tcpClient,&QTcpSocket::stateChanged,this,&MainWindow::do_socketStateChange);
}

//try to connect with server
void MainWindow::connectServer() {
    tcpClient->connectToHost(addr,port);
}


void MainWindow::do_connected() {
    createId();
    QByteArray registrationRequest = "REGISTER:" + clientId.toUtf8() + ";NAME:" + userName.toUtf8() + "\n";
    //ui->textEdit->appendPlainText("[out] "+registrationRequest);
    tcpClient->write(registrationRequest); // send id and name data to server
    setIdLabel();
    setNameLabel();

}

void MainWindow::createId() {
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

}



void MainWindow::on_btnSend_clicked()
{//“发送数据”按钮, not finished
    QString  msg;

    QByteArray  str=msg.toUtf8();
    str.append('\n');
    tcpClient->write(str);
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
        //ui->textEdit->appendPlainText("**Error: User name cannot contain special characters like ';' or ':'.");
        return;
    }

    QByteArray changeRequest = "UPDATE_NAME:" + clientId.toUtf8() + ";NAME:" + newUserName.toUtf8() + "\n";
    tcpClient->write(changeRequest);
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
                ui->serverTextEdit->appendPlainText("**User name updated successfully to " + updatedUserName);
                setNameLabel();

            } else {
                ui->serverTextEdit->appendPlainText("**Error: Failed to update user name.");
            }
        }

        else if(command == "CREATE_ROOM_RESULT") {
            QString result = parts[0].split(':')[1];
            if (result == "SUCCESS") {
                QString roomId = parts[1].split(':')[1];
                QString roomName = parts[2].split(':')[1];

                // 채팅방 생성 성공, UI 업데이트
                showNewChatRoom(roomId, roomName);
            } else {
                // 생성 실패 처리
                qDebug() << "Failed to create chat room.";
            }
        }

        else if (command == "QUERY_NAME_RESULT") {

            if (command == "QUERY_NAME_RESULT") {
                QString resultClientId = parts[0].split(':')[1];
                QString resultUserName = parts[1].split(':')[1];

                ui->serverTextEdit->appendPlainText(line);

                //search client's name
                if (resultClientId == this->clientId) {

                    if (resultUserName == "null") {
                        this->userName = this->clientId;
                    }
                    else {
                        this->userName = resultUserName;
                    }

                }
                //search other user's name
                else {

                }
            }
        }

        else if (1) {

        }
    }
}

void MainWindow::showNewChatRoom(QString& roomId, QString& roomName) {

}

void MainWindow::on_btnCreateRoom_clicked() {

    QString roomName = ui->roomNameEdit->text();

    if (roomName.isEmpty()) {
        // Handle error: Room name cannot be empty
        return;
    }

    QByteArray request = "CREATE_ROOM:NAME:" + roomName.toUtf8() + ";" + "\n";
    tcpClient->write(request);
}

void MainWindow::setIdLabel(){
    ui->userIdLabel->setText("User ID : " + clientId);
}

void MainWindow::setNameLabel(){

    ui->userNameLabel->setText("User Name : " + this->userName);
}

void MainWindow::do_socketStateChange(QAbstractSocket::SocketState socketState)
{//stateChange()信号的槽函数
    switch(socketState)
    {
    case QAbstractSocket::UnconnectedState:
        ui->serverTextEdit->appendPlainText("socket状态：UnconnectedState");
        break;
    case QAbstractSocket::HostLookupState:
        ui->serverTextEdit->appendPlainText("socket状态：HostLookupState");
        break;
    case QAbstractSocket::ConnectingState:
        ui->serverTextEdit->appendPlainText("socket状态：ConnectingState");
        break;
    case QAbstractSocket::ConnectedState:
        ui->serverTextEdit->appendPlainText("socket状态：ConnectedState");
        break;
    case QAbstractSocket::BoundState:
        ui->serverTextEdit->appendPlainText("socket状态：BoundState");
        break;
    case QAbstractSocket::ClosingState:
        ui->serverTextEdit->appendPlainText("socket状态：ClosingState");
        break;
    case QAbstractSocket::ListeningState:
        ui->serverTextEdit->appendPlainText("socket状态：ListeningState");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
