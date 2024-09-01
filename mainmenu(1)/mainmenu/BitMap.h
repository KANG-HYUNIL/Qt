#ifndef BITMAP_H
#define BITMAP_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include<QTimer>
#include<QFile>

#include <QPainter>
#include <QXmlStreamReader>
#include <unordered_map>
#include <QDebug>
#include <cmath>
#include <vector>
#include <QString>
#include <set>

namespace Ui {
class BitMap;
}



class BitMap : public QMainWindow
{
    Q_OBJECT

public:
    explicit BitMap(QWidget *parent = nullptr);
    ~BitMap();

private slots:
    //处理获取外网ip请求的 槽
    void onGetIp(QNetworkReply*);
    //根据ip获取经纬度  处理服务器响应内容
    void onGetCurrentLoc(QNetworkReply*);
    //处理服务器返回地图图片
    void onSendMapRequest();
    //搜索地址 发送请求
    void on_searchBtn_clicked();
    //搜索地址 处理响应内容
    void onSearchLoc(QNetworkReply*);

    void on_enlargeBtn_clicked();

    void on_reduceBtn_clicked();

    void on_showroadBtn_clicked(){

        shouldPaint = !shouldPaint;
        update();
    };
    void on_backBtn_clicked();

signals:
    void back();

private:
    Ui::BitMap *ui;
    QNetworkAccessManager *m_ipManager;
    QNetworkAccessManager *m_locManager;
    QNetworkAccessManager *m_searchManager;
    //网络管理对象  发送地图图片
    QNetworkAccessManager *m_mapManager;
    //响应对象 接收地图图片
    QNetworkReply *m_mapReply=NULL;

    //初始化
    void init();
    //baidu App key
    QString ak="yYSnHDIgAbSRlyktehYXmvlzA0s3eEhX";
    //存储当前主机ip
    QString currentIp;
    //经度
    double m_lng;
    //纬度
    double m_lat;
    //控制好地图的精度  高清或低清[3-18]
    int m_zoom=12;
    //当前所在的城市
    QString m_city;
    //地图文件名

    QString m_mapFileName="map.png";
    QTimer m_timer;
    QFile mapFile;

    bool shouldPaint=false;
    bool ifmove=false;
    //是否按下鼠标按钮
    bool isPress=false;
    //是否释放鼠标按钮
    bool isRelease=false;
    //按下鼠标的坐标点
    QPoint startPoint;
    //释放鼠标的坐标点
    QPoint endPoint;
    QPoint delta;
    //x轴距离差
    double mx;
    //y轴距离差
    double my;
    double xbi=1.9;
    double ybi=2;








    //获取当前主机ip
    void getIp();
    //根据ip获取经纬度  发送请求
    void getCurrentLoc();
    // 发送请求  获取地图图片
    void sendMapRequest();
    //绘制道路
    void paintEvent(QPaintEvent *event);
    //过滤事件
    bool eventFilter(QObject *watched, QEvent *event);



};

struct coordinates  //对应的经纬度坐标(double)
{
    int id;
    double lon;
    double lat;
};
struct coordinatesStr  //对应的经纬度坐标(QString)
{
    QString id;
    QString lon;
    QString lat;
};
struct coordinatesInt //对应的经纬度坐标(int)
{
    QString id;
    int x;
    int y;
};

class resolve_xml
{
public:
    double degreesToRadians(double degrees);
    double radiansToDegrees(double radians);
    double calculateDistance(coordinates node1, coordinates node2);
    void saveNode(int id,coordinates node);
    void strChangeDouble(coordinatesStr node);
    void findCommonNode();
    void changeCoor(QString strId);
    int imortant();
    std::map<QString,coordinatesInt>doubleCoorMap;
    std::vector<coordinatesInt>doubleCoor;
    std::vector<coordinatesInt>doubleWay;
    double minLat;     //最小纬度
    double maxLat;     //最大经度
    double minLon;
    double maxLon;
};



#endif // BITMAP_H
