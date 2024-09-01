#include "wea_ther.h"
#include "./ui_wea_ther.h"
//include network module
#include <QNetworkAccessManager>  //网络管理器
#include <QNetworkRequest>  //网络请求
#include <QNetworkReply>  //网络回复
#include <QUrl>
#include <QTime>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>
#include <QByteArray>
#include <QDate>
#include <QGraphicsView>
#include <QPalette>
#include <QColor>
#include <QDebug>


wea_ther::wea_ther(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::wea_ther)
{
    ui->setupUi(this);


    ui->comboBox->addItem("北京，北京");
    ui->comboBox->addItem("上海，上海");
    ui->comboBox->addItem("四川，成都");
    ui->comboBox->addItem("广东，广州");
    ui->comboBox->addItem("天津，天津");
    ui->comboBox->addItem("海南，三亚");
    ui->comboBox->addItem("辽宁，大连");
    ui->comboBox->addItem("江苏，南京");

    connect(ui->comboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(on_comboBox_currentTextChanged(QString)));

}


wea_ther::~wea_ther()
{
    delete ui;
}


void wea_ther::on_pushButton_clicked()
{

    //ui->IconInfo->clear();
    ui->TemperatureInfo->clear();
    ui->cloudCoverInfo->clear();
    ui->dateInfo->clear();
    ui->precipitationInfo->clear();
    ui->summaryInfo->clear();
    ui->windInfo->clear();

    //by url:http://t.wea_ther.itboy.net/api/wea_ther/city/101010100  query wea_ther
    //create network manager
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);   //create network manager
    //create network request
    QNetworkRequest request;
    //set request url

    request.setUrl(QUrl("https://www.meteosource.com/api/v1/free/point?place_id=" + area + "&sections=all&timezone=UTC&language=en&units=metric&key=f0tpigh1jg53ar2azcjmtcopmmucr86qf3kiutov"));
    //send request
    QNetworkReply *reply = manager->get(request);  //send request
    //receive data
    connect(reply, &QNetworkReply::finished, [=](){
        //receive data
        if (reply->error() != QNetworkReply::NoError) {
            // 网络请求出错
            QString errorString = reply->errorString();
            qDebug() << "Network request error:" << errorString;
            // 在GUI上显示错误提示
        } else {
            // 网络请求成功
            QByteArray data = reply->readAll();
                //convert data to json object
            QJsonParseError json_error;
            QJsonDocument parse_doucment = QJsonDocument::fromJson(data, &json_error);  //convert data to json object
            // get parse result's city info
            QJsonObject obj = parse_doucment.object();
            qDebug() <<"---obj:"<< obj<<"==========endl";

            if (json_error.error != QJsonParseError::NoError) {
                QString errorString = json_error.errorString();
                qDebug() << "JSON parse error:" << errorString;
            } else {


                    if(obj.contains("current"))  //判断是否包含current
                    {
                        QJsonValue currentValue = obj.value("current");  //获取current数据
                        qDebug()<<"++++++++++++++++++++++:"<<currentValue;

                        QJsonObject currentObj = currentValue.toObject();

                        // 获取当前温度
                        QJsonValue temperatureValue = currentObj.value("temperature");
                        double temperature = temperatureValue.toDouble();
                        QString temperatureStr = QString::number(temperature, 'f', 1);
                        ui->TemperatureInfo->setText("\nTemperature: " + temperatureStr + "°C");

                        // 获取云量
                        QJsonValue cloudCoverValue = currentObj.value("cloud_cover");
                        int cloudCover = cloudCoverValue.toInt();
                        ui->cloudCoverInfo->setText(ui->cloudCoverInfo->text() + "\nCloud Cover: " + QString::number(cloudCover) + "%");

                        // 获取天气图标
                        QJsonValue iconValue = currentObj.value("icon_num");
                        int iconInt = iconValue.toInt();
                        QString icon = QString::number(iconInt);
                        //ui->IconInfo->setText(ui->IconInfo->text() + "\nIcon: " + icon);

                        // 加载天气图标并显示
                        qDebug()<<QString(":/weather_icons/weather_icons/%1.png").arg(icon);
                        QPixmap pixmap(QString(":/weather_icons/weather_icons/%1.png").arg(icon));//(QString("C:/Users/User/OneDrive/Documents/year3pre/DRnDriveSystem/weather/weather/weather_icons/"+icon+".png"));
                        pixmap.scaled(ui->IconView->size(),Qt::KeepAspectRatio);
                        ui->IconView->setScaledContents(true);
                        ui->IconView->setPixmap(pixmap);

                        // 获取降水信息
                        QJsonObject precipitationObj = currentObj.value("precipitation").toObject();
                        double precipitationTotal = precipitationObj.value("total").toDouble();
                        QString precipitationType = precipitationObj.value("type").toString();
                        ui->precipitationInfo->setText(ui->precipitationInfo->text() + "\nPrecipitation: " + QString::number(precipitationTotal) + "mm (" + precipitationType + ")");

                        // 获取风信息
                        QJsonObject windObj = currentObj.value("wind").toObject();
                        double windAngle = windObj.value("angle").toDouble();
                        QString windDir = windObj.value("dir").toString();
                        double windSpeed = windObj.value("speed").toDouble();
                        ui->windInfo->setText(ui->windInfo->text() + "\nWind: " + QString::number(windSpeed) + "m/s from " + windDir + " (" + QString::number(windAngle) + "°)");

                        // 获取天气概要
                        QJsonValue summaryValue = currentObj.value("summary");
                        QString summary = summaryValue.toString();
                        ui->summaryInfo->setText(ui->summaryInfo->text() + "\nSummary: " + summary);

                        // 获取当前日期
                        QDate currentDate = QDate::currentDate();
                        QString dateStr = currentDate.toString("yyyy-MM-dd");
                        ui->dateInfo->setText("Date: " + dateStr);

                    }
                }
            }
        reply->deleteLater(); // 释放网络请求资源
    });
}



void wea_ther::on_comboBox_currentTextChanged(QString selected)
{
    //areaurl = new QUrl();

    if(selected=="北京，北京") area = "beijing";
    else if(selected=="上海，上海") area="shanghai";
    else if(selected=="广东，广州") area="guangzhou";
    else if(selected=="天津，天津") area="tianjin";
    else if(selected=="四川，成都") area="chengdu";
    else if(selected=="海南，三亚") area="sanya";
    else if(selected=="辽宁，大连") area="dalian";
    else if(selected=="江苏，南京") area="nanjing";

    //areaurl = new QUrl("https://www.meteosource.com/api/v1/free/point?place_id=" + area + "&sections=all&timezone=UTC&language=en&units=metric&key=f0tpigh1jg53ar2azcjmtcopmmucr86qf3kiutov");
}

void wea_ther::on_weatherback_clicked()
{
    emit back();
}
