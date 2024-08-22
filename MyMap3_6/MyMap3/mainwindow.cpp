#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostInfo>
#include <QDebug>
#include <QNetworkInterface>
#include <QNetworkRequest>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //注册事件过滤
    installEventFilter(this);

    //初始化
    init();
    //获取当前的外网ip
    getIp();

    //connect with btns
    connect(ui->searchBtn, &QPushButton::clicked, this, &MainWindow::on_searchBtn_clicked);
    connect(ui->enlargeBtn, &QPushButton::clicked, this, &MainWindow::on_enlargeBtn_clicked);
    connect(ui->reduceBtn, &QPushButton::clicked, this, &MainWindow::on_reduceBtn_clicked);

}

void MainWindow::init(){
    //网络管理对象
    m_ipManager =new QNetworkAccessManager(this);
    //请求响应事件
    connect(m_ipManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(onGetIp(QNetworkReply*)));

    //网络管理对象
    m_locManager =new QNetworkAccessManager(this);
    //请求响应事件
    connect(m_locManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(onGetCurrentLoc(QNetworkReply*)));

    m_searchManager=new QNetworkAccessManager(this);
    connect(m_searchManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(onSearchLoc(QNetworkReply*)));

    //网络管理对象
    m_mapManager =new QNetworkAccessManager(this);

}

//获取当前主机ip   发送请求
 void MainWindow::getIp(){
     //此处无法获取外网ip
//     QHostInfo info = QHostInfo::fromName(QHostInfo::localHostName());
//     QString ip;
//     foreach(QHostAddress address, QNetworkInterface::allAddresses()) {
//           if (address.protocol() == QAbstractSocket::IPv4Protocol) {
//                  qDebug() << "Current IP:" << address.toString();
//                ip= address.toString();
//            }
//     }

     //请求http://httpbin.org/ip 获取外网ip

      //声明url 请求地址
      QUrl url("http://httpbin.org/ip");
      //声明请求对象
      QNetworkRequest request(url);
       //通过网络管理对象 发送get请求
      m_ipManager->get(request);

 }

 //处理服务器响应内容, get cur ip address
 void MainWindow::onGetIp(QNetworkReply *reply){
     //reply->readAll()  服务响响应的正文内容
     //qDebug() << "Current IP:" << reply->readAll();
     QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
     QJsonObject jsonObj = jsonDoc.object();
     currentIp=jsonObj.value("origin").toString();
     //qDebug() << "Current IP:" << currentIp<<endl;

     //调用获取当前经纬度的函数
     getCurrentLoc();
 }

 //根据ip获取经纬度  发送请求
 void MainWindow::getCurrentLoc(){
    //url
    QString host= "http://api.map.baidu.com/location/ip";
    QString query_str=QString("ip=%1&coor=bd09ll&ak=%2")
            .arg(currentIp).arg(ak);
    //请求地址
    QUrl url(host+"?"+query_str);
    //请求对象
    //声明请求对象
    QNetworkRequest request(url);
     //发送请求
    m_locManager->get(request); //얻은 ip를 통해 url만들어 접속, m_locManager 통해 연결 시도
 }

 //根据ip获取经纬度  处理服务器响应内容
 void MainWindow::onGetCurrentLoc(QNetworkReply *reply){
//    qDebug() << reply->readAll() <<endl;
     QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
     QJsonObject jsonObj = jsonDoc.object();
     QJsonObject jsonContent=jsonObj.value("content").toObject();
     QJsonObject jsonPonit=jsonContent.value("point").toObject();
     //jsonPonit 에서 x, y 값 가져오기
     m_lng=jsonPonit.value("x").toString().toDouble();
     m_lat=jsonPonit.value("y").toString().toDouble();
     //jsonContent에서 city 값 가져오기
     m_city=jsonContent.value("address_detail").toObject().value("city").toString();
    // qDebug() << jsonPonit.value("x").toString() <<endl;
    // qDebug() << jsonPonit.value("y").toString() <<endl;
     sendMapRequest();
 }


 // 发送请求  获取地图图片
 void  MainWindow::sendMapRequest(){
     //how

     //断调前一次请求
     if(m_mapReply!=NULL){
         m_mapReply->disconnect();
         //断掉事件连接
         disconnect(m_mapReply,&QIODevice::readyRead,this,&MainWindow::onSendMapRequest);
     }

     //开始新的请求
     //url
     //请求地址
     QString host= "http://api.map.baidu.com/staticimage/v2";
     //请求参数
     QString query_str=QString("ak=%1&width=512&height=256&center=%2,%3&zoom=%4")
             .arg(ak).arg(m_lng).arg(m_lat).arg(m_zoom);
     QUrl url(host+"?"+query_str);

     //qDebug()<<host+"?"+query_str<<endl;

     //url 요청 보내기
     QNetworkRequest request(url);
     //此处与前面的请求不同，等待服务器响应，
     m_mapReply= m_mapManager->get(request);
     //连接事件，处理服务器返回的 文件流
     connect(m_mapReply,&QIODevice::readyRead,this,&MainWindow::onSendMapRequest);
 }


 //处理服务器返回地图图片
 void  MainWindow::onSendMapRequest(){

    //删除原有的地图图片 使用系统命令删除
    system("del map.png");
    QFile::remove(m_mapFileName);
    //文件对象
    mapFile.setFileName(m_mapFileName);

     //读取文件流，保存文件到本地,
     //open 没有则新建文件，打开
    //읽기 전용 open, truncate로 기존 내용 잘라내고 삭제
    //mapFile.open(QIODevice::WriteOnly| QIODevice::Truncate);

    connect(m_mapReply, &QIODevice::readyRead, this, [=]() {

        mapFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
        mapFile.write(m_mapReply->readAll());
    });




    // m_timer.start(2);
    connect(m_mapReply, &QNetworkReply::finished,this, [=](){

        // m_timer.stop();
        mapFile.write(m_mapReply->readAll());
        mapFile.close();

        QPixmap pixmap;
        if(pixmap.load(m_mapFileName)){
            ui->mapWidget->setStyleSheet("QWidget{border-image:url(./map.png);}");
        }

    });

 }



MainWindow::~MainWindow()
{
    delete ui;
}

//搜索地址  发送请求
void MainWindow::on_searchBtn_clicked()
{
    //1、取文本框的值
   // qDebug()<<ui->edit_search->text()<<endl;

    //2、url
    QString host="http://api.map.baidu.com/place/v2/search";


    QString query_str=QString("query=%1&location=%2,%3&output=json&ak=%4&radius=20000")
            .arg(ui->edit_search->text()).arg(m_lat).arg(m_lng).arg(ak);

    QList<QString> list;
    list.append("湖北");
    list.append("湖南");
    list.append("北京");
    if(list.contains(ui->edit_search->text())){
        query_str.append("&region=%1").arg(ui->edit_search->text());
    }else {
        query_str.append("&region=%1").arg(m_city);
    }

    QUrl url(host+"?"+query_str);
    //3、request
    QNetworkRequest request(url);
    //4、发送请求
    m_searchManager->get(request);

}
//搜索地址  处理响应内容
void MainWindow::onSearchLoc(QNetworkReply *reply){
//    qDebug()<<reply->readAll()<<endl;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
    reply->deleteLater(); //ensure that object can be deleted after event finished
     QJsonObject jsonObj = jsonDoc.object();
     QJsonArray addrArray= jsonObj.value("results").toArray();
     QJsonObject addrJson=addrArray.at(0).toObject();
     QJsonObject xyJson=addrJson.value("location").toObject();
     m_lng=xyJson.value("lng").toDouble();
     m_lat=xyJson.value("lat").toDouble();
     m_city=addrJson.value("city").toString();

     //qDebug()<<m_lng<<endl;
     //qDebug()<<m_lat<<endl;

     m_zoom=17;
     //调取地图图片的函数
     sendMapRequest();
}

//放大按钮，点击后改变m_zoom [3-18]
void MainWindow::on_enlargeBtn_clicked()
{
    //判断m_zoom是否小于18，
    if(m_zoom<19){
        m_zoom+=1;
        //调用函数重新获取地图图片
        sendMapRequest();
    }
}

//缩小按钮，点击后改变m_zoom [3-18]
void MainWindow::on_reduceBtn_clicked()
{
    if(m_zoom>3){
        m_zoom-=1;
        sendMapRequest();
    }
}



//过滤事件
bool  MainWindow::eventFilter(QObject *watched, QEvent *event){

//    qDebug()<<event->type()<<endl;
    if(event->type()==QEvent::MouseButtonPress){
        //qDebug()<<event->type()<< cursor().pos().x()<<":"<<cursor().pos().y()<<endl;
        isPress=true;
        startPoint.setX(cursor().pos().x());
        startPoint.setY(cursor().pos().y());
    }

    if(event->type()==QEvent::MouseButtonRelease){
        //qDebug()<<event->type()<< cursor().pos().x()<<":"<<cursor().pos().y()<<endl;
        isRelease=true;
        endPoint.setX(cursor().pos().x());
        endPoint.setY(cursor().pos().y());
    }

    if(isPress&&isRelease){
        isPress=false;
        isRelease=false;
       //计算距离差
        mx=startPoint.x()-endPoint.x();
        my=startPoint.y()-endPoint.y();

        if(qAbs(mx)>5||qAbs(my)>5){
            m_lng+=mx*0.000002*(19-m_zoom)*(19-m_zoom);
            m_lat-=my*0.000002*(19-m_zoom)*(19-m_zoom);
            sendMapRequest();
        }
    }

    return QWidget::eventFilter(watched,event);
}
