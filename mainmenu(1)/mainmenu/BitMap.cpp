#include "BitMap.h"
#include "ui_BitMap.h"
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

#include <map>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneEvent>
#include <QIODevice>


#include <QApplication>



std::map<QString,coordinatesStr>mp;
std::map<QString,coordinates>doubleMap;
std::vector<QString>commonNode;
std::map<QString,std::vector<QString>>mapWay;

resolve_xml Xml;

BitMap::~BitMap()
{
    delete ui;
}

BitMap::BitMap(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BitMap)
{
    ui->setupUi(this);


    //注册事件过滤
    //installEventFilter(this);

    //初始化
    init();

    //获取当前的外网ip
    //getIp();

}



void BitMap::init(){
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

    m_lng=116.1729483;
    m_lat=39.7457874;
    m_zoom=14;
//116.1729483°，纬度39.7457874°
    sendMapRequest();

    ui->MyGraphicsView->scale(1.3,2.28);
    ui->MyGraphicsView->viewport()->installEventFilter(this);
    //QGraphicsView的鼠标事件会被iewport拦截，因此将过滤器装给后者



}


//获取当前主机ip   发送请求
 void BitMap::getIp(){
      //声明url 请求地址
      QUrl url("http://httpbin.org/ip");
      //声明请求对象
      QNetworkRequest request(url);
       //通过网络管理对象 发送get请求
      m_ipManager->get(request);

 }

 //处理服务器响应内容
 void BitMap::onGetIp(QNetworkReply *reply){
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
 void BitMap::getCurrentLoc(){
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
    m_locManager->get(request);
 }

 //根据ip获取经纬度  处理服务器响应内容
 void BitMap::onGetCurrentLoc(QNetworkReply *reply){
//    qDebug() << reply->readAll() <<endl;
     QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
     QJsonObject jsonObj = jsonDoc.object();
     QJsonObject jsonContent=jsonObj.value("content").toObject();
     QJsonObject jsonPonit=jsonContent.value("point").toObject();
     m_lng=jsonPonit.value("x").toString().toDouble();
     m_lat=jsonPonit.value("y").toString().toDouble();
     m_city=jsonContent.value("address_detail").toObject().value("city").toString();
     //qDebug() << jsonPonit.value("x").toString() <<endl;
     //qDebug() << jsonPonit.value("y").toString() <<endl;
     sendMapRequest();
 }

 // 发送请求  获取地图图片
 void  BitMap::sendMapRequest(){
     //how

     //断调前一次请求
     if(m_mapReply!=NULL){
         m_mapReply->disconnect();
         //断掉事件连接
         disconnect(m_mapReply,&QIODevice::readyRead,this,&BitMap::onSendMapRequest);
     }

     //开始新的请求
     //url
     //请求地址
     QString host= "http://api.map.baidu.com/staticimage/v2";
     //请求参数
     QString query_str=QString("ak=%1&width=512&height=256&center=%2,%3&zoom=%4")
             .arg(ak).arg(m_lng).arg(m_lat).arg(m_zoom);
     QUrl url(host+"?"+query_str);

     qDebug()<<host+"?"+query_str<<Qt::endl;

     QNetworkRequest request(url);
     //此处与前面的请求不同，等待服务器响应，
     m_mapReply= m_mapManager->get(request);
     //连接事件，处理服务器返回的 文件流
     connect(m_mapReply,&QIODevice::readyRead,this,&BitMap::onSendMapRequest);
 }

 //处理服务器返回地图图片
 void  BitMap::onSendMapRequest(){


     //删除原有的地图图片 使用系统命令删除
     system("del map.png");
     //qDebug()<<"shanchulema"<<Qt::endl;
     //文件对象

    mapFile.setFileName(m_mapFileName);

     //open 没有则新建文件，打开
    mapFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
    m_timer.start(2);
    connect(&m_timer,&QTimer::timeout,[=](){
        m_timer.stop();
        mapFile.write(m_mapReply->readAll());
        mapFile.close();

        QPixmap pixmap;
        if(pixmap.load(m_mapFileName)){
            ui->MyGraphicsView->setStyleSheet("QGraphicsView{border-image:url(./map.png);}");
        }

    });

 }

 //道路显示
 void BitMap::paintEvent(QPaintEvent *event)
 {
     //Graph grap;
     QMainWindow::paintEvent(event);  // 调用父类的 paintEvent 以确保其他部分绘制正确

     QGraphicsScene *scene = new QGraphicsScene(this);
     ui->MyGraphicsView->setScene(scene);
     ui->MyGraphicsView->setRenderHint(QPainter::Antialiasing); //抗锯齿

     std::vector<std::vector<QPointF>>roads;
     for(auto it = mapWay.begin();it != mapWay.end();it++)
     {
         std::vector<QString> vectorStr = it->second;
         std::vector<QPointF>roadWay;
         for(int i = 0;i < vectorStr.size();i++)
         {
             QString wayStr = vectorStr[i];
             coordinatesInt coorWay;
             coorWay = Xml.doubleCoorMap[wayStr];
             QPoint pointWay;
             pointWay.setX(coorWay.x);
             pointWay.setY(coorWay.y);
             roadWay.push_back(pointWay);
         }
         roads.push_back(roadWay);
         roadWay.clear();
     }

     for (const std::vector<QPointF> &road : roads) {


         QPainterPath pathsss;
         pathsss.moveTo(road[0]);
         for (int i = 1; i < road.size(); ++i)
         {
             pathsss.lineTo(road[i]);
         }
         QGraphicsPathItem *pathItem = new QGraphicsPathItem(pathsss);

         // 可选：设置道路的样式
         if(shouldPaint){
             scene->addItem(pathItem);
             QPen roadPen(Qt::blue, 0.5);  // 示例：蓝色，线宽为2
             pathItem->setPen(roadPen);
             qDebug() << "画了" <<Qt::endl;
         }
         if(ifmove)
         {
         pathItem ->moveBy(delta.x()/xbi, delta.y()/ybi);
         //ifmove= false;
         }


     }
 }



//搜索地址  发送请求
void BitMap::on_searchBtn_clicked()
{
    //1、取文本框的值
    qDebug()<<ui->edit_search->text()<<Qt::endl;

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
void BitMap::onSearchLoc(QNetworkReply *reply){
    qDebug()<<reply->readAll()<<Qt::endl;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
    reply->deleteLater();
     QJsonObject jsonObj = jsonDoc.object();
     QJsonArray addrArray= jsonObj.value("results").toArray();
     QJsonObject addrJson=addrArray.at(0).toObject();
     QJsonObject xyJson=addrJson.value("location").toObject();
     m_lng=xyJson.value("lng").toDouble();
     m_lat=xyJson.value("lat").toDouble();
     m_city=addrJson.value("city").toString();

     qDebug()<<m_lng<<Qt::endl;
     qDebug()<<m_lat<<Qt::endl;

     m_zoom=3;
     //调取地图图片的函数
     sendMapRequest();
}

//放大按钮，点击后改变m_zoom [3-18]
void BitMap::on_enlargeBtn_clicked()
{
    //判断m_zoom是否小于18，
    if(m_zoom<19){
        m_zoom+=1;
        //调用函数重新获取地图图片
        sendMapRequest();
    }
    double scaleFactor = 1.9;
    xbi = xbi * 1.5;
    ybi = ybi * 1.6;
    ui->MyGraphicsView->scale(scaleFactor, scaleFactor);
}

//缩小按钮，点击后改变m_zoom [3-18]
void BitMap::on_reduceBtn_clicked()
{
    if(m_zoom>3){
        m_zoom-=1;
        sendMapRequest();
    }
    double scaleFactor = 1.9;
    xbi = xbi / 1.3;
    ybi = ybi / 1.4;
    ui->MyGraphicsView->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
}

//过滤事件
bool  BitMap::eventFilter(QObject *watched, QEvent *event){

    //qDebug()<<event->type()<<Qt::endl;
    if(event->type()==QGraphicsSceneEvent::MouseButtonPress){
        qDebug()<<event->type()<< cursor().pos().x()<<":"<<cursor().pos().y()<<Qt::endl;
        isPress=true;
        startPoint.setX(cursor().pos().x());
        startPoint.setY(cursor().pos().y());

    }

    if(event->type()==QGraphicsSceneEvent::MouseButtonRelease){
        //qDebug()<<event->type()<< cursor().pos().x()<<":"<<cursor().pos().y()<<endl;
        isRelease=true;
        endPoint.setX(cursor().pos().x());
        endPoint.setY(cursor().pos().y());

        delta += endPoint - startPoint;
        ifmove=true;
        update();

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


double resolve_xml::degreesToRadians(double degrees) {
    return degrees * M_PI / 180;
}
double resolve_xml::radiansToDegrees(double radians) {
    return radians * 180 / M_PI;
}
double resolve_xml::calculateDistance(coordinates node1, coordinates node2) { //经纬度坐标
    double dLat = degreesToRadians(node2.lat - node1.lat);   //Haversine公式
    double dLon = degreesToRadians(node2.lon - node1.lon);
    double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
               std::cos(degreesToRadians(node1.lat)) * std::cos(degreesToRadians(node2.lat)) *
                   std::sin(dLon / 2) * std::sin(dLon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    double distance = 6378137 * c;
    return distance;
}
void resolve_xml::strChangeDouble(coordinatesStr node)
{
    coordinates coor;
    coor.id = node.id.toInt();
    coor.lat = node.lat.toDouble();
    coor.lon = node.lon.toDouble();
    doubleMap[node.id] = coor;
}

void resolve_xml::findCommonNode() //查找公共交点
{
    QFile file(":/qrc/map (3).osm");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "无法打开XML文件.";
        //exit(0);
    }
    QXmlStreamReader xml(&file);

    std::set<QString> wayNodes;

    int wayCount = 0;
    bool flag = false;

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement) {
            if (xml.name().toString() == "way") {
                flag = true;
            }
            else if (xml.name().toString() == "nd" && flag == true) {
                QString ndRef = xml.attributes().value("ref").toString();
                if (wayNodes.find(ndRef) != wayNodes.end()) {
                    commonNode.push_back(ndRef);
                }
                else {
                    wayNodes.insert(ndRef);
                }
            }
            else if(xml.name().toString() == "relation"){
                flag = false;
            }
            else if(xml.name().toString() == "member"){
                flag = false;
            }
        }
    }
    file.close();
}

void resolve_xml::changeCoor(QString strId)
{
    coordinates coo = doubleMap[strId];
    coordinatesInt cooint ;
    int width = (maxLon - minLon) * 10000;
    int length = (maxLat - minLat) * 10000;
    int x = (maxLat - coo.lat ) * 10000 ;
    int y = (coo.lon - minLon) * 10000 ;
    cooint.id = strId;
    cooint.x = y;
    cooint.y = x;
    Xml.doubleCoor.push_back(cooint);
    Xml.doubleCoorMap[strId] = cooint;
}

int resolve_xml::  imortant()
{

    std::vector<QString>strNode;
    std::vector<QString>strWay;
    coordinatesStr coorStr;   //QString类型经纬度的结构体
    QString wayId;      //way的Id
    QString nodeId;     //node的ID
    QString nodeLon;    //node的经度
    QString nodeLat;    //node的纬度
    QFile fileName(":qss/map (3).osm");
    if (!fileName.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return 1;
    }
    QXmlStreamReader xml(&fileName);
    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartElement) {
            if(xml.name().toString() == "node"){
                nodeId = xml.attributes().value("id").toString();
                nodeLon = xml.attributes().value("lon").toString();
                nodeLat = xml.attributes().value("lat").toString();
                coorStr.id = nodeId;
                coorStr.lat = nodeLat;
                coorStr.lon = nodeLon;
                mp[nodeId] = coorStr;
                strNode.push_back(nodeId);
            }
            else if (xml.name().toString() == "way") {
                // 提取way元素的id属性值
                strWay.clear();
                wayId = xml.attributes().value("id").toString();
                //qDebug() << "Way ID:" << wayId;
            } else if (xml.name().toString() == "nd") {
                // 提取nd元素的ref属性值
                QString ndRef = xml.attributes().value("ref").toString();
                strWay.push_back(ndRef);
                mapWay[wayId] = strWay;
                //qDebug() << "ND Ref:" << ndRef;
            }
            else if(xml.name().toString() == "bounds")
            {
                //提取数据中的最大最小经纬度
                Xml.minLat = xml.attributes().value("minlat").toString().toDouble();
                Xml.minLon = xml.attributes().value("minlon").toString().toDouble();
                Xml.maxLat = xml.attributes().value("maxlat").toString().toDouble();
                Xml.maxLon = xml.attributes().value("maxlon").toString().toDouble();
            }
        }
    }
    if (xml.hasError())
    {
        qDebug() << "XML解析错误: " << xml.errorString();
        return 1;
    }
    for(auto it = mp.begin();it != mp.end();it++) //将QString的经纬度转化成double类型的经纬度
    {
        QString Id = it->first;
        coordinatesStr jw = it->second;
        Xml.strChangeDouble(jw);
        Xml.changeCoor(Id);
        //qDebug()<<Id;
        //qDebug()<<jw.lat<<" "<<jw.lon;
    }
    for(auto it = mapWay.begin();it != mapWay.end();it++)
    {
        std::vector<QString> Strway = it->second;
        for(int i = 0;i < Strway.size();i++)
        {
            QString Qstr = Strway[i];
            double x = doubleMap[Qstr].lat;
            double y = doubleMap[Qstr].lon;
            minLat = std::min(minLat,x);
            minLon = std::min(minLon,y);
            maxLat = std::max(maxLat,x);
            maxLon = std::max(maxLon,y);
        }
    }
    // qDebug()<<"xxxx"<<minLat<<minLon<<maxLat<<maxLon; //输出最大最小经纬度
    for(auto it = mapWay.begin();it != mapWay.end();it++) //保存所有way的点
    {
        QString way = it->first;
        std::vector<QString> Strway = it->second;
        for(int i = 0;i < Strway.size();i++)
        {
            QString Qstr = Strway[i];
            //Xml.changeCoor(Qstr);
        }
    }
    for(auto it = mapWay.begin();it != mapWay.end();it++) //根据经纬度计算点与点之间距离，连接成路
    {
        QString way = it->first;
        //qDebug()<<way;
        std::vector<QString> Strway = it->second;
        //qDebug()<<"way nodeNumber:"<<Strway.size();
        int distanceWay = 0;
        for(int i = 1;i < Strway.size();i++)
        {
            QString Qstr1 = Strway[i-1];
            QString Qstr2 = Strway[i];
            coordinates node1 = doubleMap[Qstr1];
            coordinates node2 = doubleMap[Qstr2];
            distanceWay += (int)Xml.calculateDistance(node1,node2);
        }
        //qDebug()<<"way distance:"<<distanceWay;
    }
    Xml.findCommonNode();
    for(int i = 0;i < commonNode.size();i++)
    {
        //qDebug()<<"commonNode"<<commonNode[i];
    }
    //qDebug()<<"commonNode size"<<commonNode.size()<<"way size:"<<mapWay.size()<<"village size"<<mapVillage.size();
    //qDebug()<<"---"<<Xml.doubleCoor.size();
    fileName.close();
    return 0;


}

void BitMap::on_backBtn_clicked()
{
    emit this->back();
}

