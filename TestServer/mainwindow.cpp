#include "mainwindow.h"
#include "ui_mainwindow.h"
#include    <QtNetwork>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //open, initialize database
    openDatabase();
    initializeDatabase();
    //create new QTcpServer
    tcpServer=new QTcpServer(this);
    activeServer();
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(do_newConnection()));

    //get chatRoom data from database, save it in memory
    QSqlQuery query(DB);
    query.exec("SELECT id, name FROM chat_rooms");

    while (query.next()) {
        int roomId = query.value(0).toInt();
        QString roomName = query.value(1).toString();

        // Store the chat room information in memory
        chatRooms.insert(roomId, roomName);
    }
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
    query.exec("CREATE TABLE IF NOT EXISTS users (id TEXT PRIMARY KEY, name TEXT)");

    query.exec("CREATE TABLE IF NOT EXISTS chat_rooms ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL, "
               "created_by TEXT, "
               "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
               "FOREIGN KEY (created_by) REFERENCES users(id));");

    query.exec("CREATE TABLE IF NOT EXIST "
               "chat_room_members ("
               "chat_room_id INTEGER,"
               "user_id TEXT,"
               "joined_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
               "PRIMARY KEY (chat_room_id, user_id),"
               "FOREIGN KEY (chat_room_id) REFERENCES chat_rooms(id),"
               "FOREIGN KEY (user_id) REFERENCES users(id));");

    query.exec("CREATE TABLE IF NOT EXIST "
               "chat_messages ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "chat_room_id INTEGER,"
               "user_id TEXT,"
               "message TEXT NOT NULL,"
               "sent_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
               "FOREIGN KEY (chat_room_id) REFERENCES chat_rooms(id),"
               " FOREIGN KEY (user_id) REFERENCES users(id));");

}


void MainWindow::closeDatabase() {
    if (DB.isOpen()) {
        DB.close();
    }
}

void MainWindow::do_socketReadyRead()
{//读取缓冲区的行文本
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());

    while(clientSocket->canReadLine()) {

        QString line = clientSocket->readLine().trimmed();
        ui->textEdit->appendPlainText("[in] "+line);
        QStringList parts = line.split(';');
        QString command = parts[0].split(':')[0];

        //if client registered
        if (command == "REGISTER") {
            QString clientId = parts[0].split(':')[1];
            QString userName = parts[1].split(':')[1];

            // save id as key, socket as value
            clients.insert(clientId, clientSocket);
            registerClient(clientId, userName);

            // send response to client
            QString response = "REGISTER_SUCCESS\n";
            ui->textEdit->appendPlainText("[out] "+response);
            clientSocket->write(response.toUtf8());
        }

        else if (command == "UPDATE_NAME") {
            QString clientId = parts[0].split(':')[1];
            QString newUserName = parts[1].split(':')[1];

            // update user name in db
            updateUserName(clientId, newUserName);

            // send update response to client
            QString response = "UPDATE_NAME_RESULT:SUCCESS; New User Name:" + newUserName + "\n";
            ui->textEdit->appendPlainText("[out] "+response);
            clientSocket->write(response.toUtf8());
        }

        else if (command == "QUERY_NAME") {
            QString clientId = parts[0].split(':')[1];
            QString userName = queryUserNameFromDatabase(clientId);

            //send query result to client
            QString response;
            if (!userName.isEmpty()) {
                response = "QUERY_NAME_RESULT:" + clientId + ";NAME:" + userName + "\n";
            }

            else {
                response = "QUERY_NAME_RESULT:" + clientId + ";NAME:null\n";
            }
            ui->textEdit->appendPlainText("[out] "+response);
            clientSocket->write(response.toUtf8());
        }

        //client create chat room
        else if (command == "CREATE_ROOM") {

            if (parts.size() < 3) {
                qDebug() << "Invalid CREATE_ROOM request format.";
                return;
            }

            QString roomName = parts[2];
            QString creatorId = getClientIdFromSocket(clientSocket); // Get client ID from socket

            int roomId = createChatRoom(creatorId, roomName);
            if (roomId != -1) {
                addUserToChatRoom(roomId, creatorId); // Add creator to the chat room
                QString response = "CREATE_ROOM_RESULT:SUCCESS;ROOM_ID:" + QString::number(roomId) + ";NAME:" + roomName + "\n";
                ui->textEdit->appendPlainText("[out] "+response);
                clientSocket->write(response.toUtf8());
            } else {
                QString response = "CREATE_ROOM_RESULT:FAILURE\n";
                ui->textEdit->appendPlainText("[out] "+response);
                clientSocket->write(response.toUtf8());
            }
        }

        else if (command == "GET_ROOM_USERS") {
            QString roomId = parts[0].split(':')[1]; .

            //
            QString roomName = getChatRoomName(roomId);

            //
            QList<QString> userNames = getChatRoomUsers(roomId);

            // response
            QString response = "ROOM_USERS_RESULT:" + roomId + ";" + roomName;
            for (const QString &userName : userNames) {
                response += ";" + userName;
            }
            response += "\n";

            ui->textEdit->appendPlainText("[out] " + response);
            clientSocket->write(response.toUtf8());
        }

    }

}

//find username for id and return it
QString MainWindow::queryUserNameFromDatabase(QString clientId) {
    QSqlQuery query(DB);


    query.prepare("SELECT name FROM users WHERE id = :clientId");
    query.bindValue(":clientId", clientId);

    if (query.exec() && query.next()) {
        return query.value(0).toString();
    } else {
        return QString();
    }
}

//when client connect to server
void MainWindow::registerClient(const QString& clientId, const QString& userName) {
    QSqlQuery query(DB);
    QString rUserName;
    if (userName == "") {
        rUserName = clientId;
    }
    else {
        rUserName = userName;
    }

    query.prepare("SELECT COUNT(*) FROM users WHERE id = :clientId");
    query.bindValue(":clientId", clientId);
    if (query.exec() && query.next()) {
        int count = query.value(0).toInt();
        if (count > 0) {
            qDebug() << "Client already registered with ID:" << clientId;
            return;
        }
    }
    query.prepare("INSERT INTO users (id, name) VALUES (:client_id, :user_name)");
    query.bindValue(":client_id", clientId);
    query.bindValue(":user_name", rUserName);
    if (!query.exec()) {
        qDebug() << "Failed to register client:" << query.lastError();
    }
}

//update user name
void MainWindow::updateUserName(const QString& clientId, const QString& newUserName) {
    QSqlQuery query(DB);
    query.prepare("UPDATE users SET name = :user_name WHERE id = :client_id");
    query.bindValue(":user_name", newUserName);
    query.bindValue(":client_id", clientId);
    if (!query.exec()) {
        qDebug() << "Failed to update user name:" << query.lastError();
    }
    else {
        QString updateText = "UPDATE_NAME_RESULT:SUCCESS; New User Name :" + newUserName;

        //tcpSocket->write("Update UserName into" + newUserName +"\n");
        QByteArray  str=updateText.toUtf8();
        str.append('\n');       //添加一个换行符
        //tcpSocket->write(str);
    }
}

int MainWindow::createChatRoom(const QString& creatorId, const QString& roomName){

    QSqlQuery query(DB);
    query.prepare("INSERT INTO chat_rooms (name, created_by) VALUES (:room_name, :creator_id)");
    query.bindValue(":room_name", roomName);
    query.bindValue(":creator_id", creatorId);

    if (!query.exec()) {
        qDebug() << "Failed to create chat room:" << query.lastError();
        return -1; // Failure
    }

    // Get the ID of the newly created chat room
    int roomId = query.lastInsertId().toInt();

    // Store the chat room information in memory
    chatRooms.insert(roomId, roomName);

    return roomId;
}

bool MainWindow::addUserToChatRoom(int roomId, const QString& userId){
    QSqlQuery query(DB);

    query.prepare("INSERT INTO chat_room_members (chat_room_id, user_id) VALUES (:room_id, :user_id)");
    query.bindValue(":room_id", roomId);
    query.bindValue(":user_id", userId);

    if (!query.exec()) {
        qDebug() << "Failed to add user to chat room:" << query.lastError();
        return false;
    }
    return true;
}

bool MainWindow::removeUserFromChatRoom(int roomId, const QString& userId) {
    QSqlQuery query(DB);
    query.prepare("DELETE FROM chat_room_members WHERE chat_room_id = :room_id AND user_id = :user_id");
    query.bindValue(":room_id", roomId);
    query.bindValue(":user_id", userId);

    if (!query.exec()) {
        qDebug() << "Failed to remove user from chat room:" << query.lastError();
        return false;
    }
    return true;
}

//search chat room users from room Id
QList<QString> MainWindow::getChatRoomUsers(int roomId) {
    QSqlQuery query(DB);
    QList<QString> userNames;

    query.prepare("SELECT users.name FROM users "
                  "JOIN chat_room_members ON users.id = chat_room_members.user_id "
                  "WHERE chat_room_members.chat_room_id = :room_id");
    query.bindValue(":room_id", roomId);

    if (query.exec()) {
        while (query.next()) {
            QString userName = query.value(0).toString();
            userNames.append(userName);
        }
    } else {
        qDebug() << "Failed to get users in chat room:" << query.lastError();
    }

    return userNames;
}

QString MainWindow::getChatRoomName(const QString& roomId) {
    QSqlQuery query(DB);

    query.prepare("SELECT name FROM chat_rooms WHERE id = :room_id");
    query.bindValue(":room_id", roomId);

    if (query.exec() && query.next()) {
        return query.value(0).toString();
    } else {
        qDebug() << "Failed to get chat room name:" << query.lastError();
        return QString();
    }
}


//active server
void MainWindow::activeServer() {
    QHostAddress   address(IP);
    tcpServer->listen(address,port);
}

//client가 server에 접속 시
void MainWindow::do_newConnection()
{
    //새 client와 통신할 QTcpSocket 생성
    QTcpSocket* clientSocket = tcpServer->nextPendingConnection(); //创建socket

    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(do_socketReadyRead()));
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(do_clientDisconnected()));


}


void MainWindow::do_clientConnected() {

}

void MainWindow::do_clientDisconnected()
{//客户端断开连接时
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());

    if (clientSocket) {
        QString clientId = clients.key(clientSocket, QString());

        if (!clientId.isEmpty()) {
            clients.remove(clientId);
        }

        clientSocket->deleteLater();
        ui->textEdit->appendPlainText("**client socket disconnected");
    }
}

QString MainWindow::getClientIdFromSocket(QTcpSocket* socket) {

    for (auto it = clients.begin(); it != clients.end(); ++it) {
        if (it.value() == socket) {
            return it.key(); // return client id connected with socket
        }
    }

    return QString();
}


MainWindow::~MainWindow()
{
    closeDatabase();
    delete ui;
}





