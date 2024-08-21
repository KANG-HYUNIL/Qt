#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

//생성자 호출,
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , networkManager(new QNetworkAccessManager(this))

{

    ui->setupUi(this);

    //connect with btn and manager
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::networkReplyFinished);
    connect(ui -> fetchWeatherBtn, &QPushButton::clicked, this, &MainWindow::on_fetchWeatherBtn_clicked);
}

//try to connect network
void MainWindow::on_fetchWeatherBtn_clicked(){

    int curIdx = ui ->CityComboBox->currentIndex();
    QString apiUrl;
    switch (curIdx) {
        case 0:

            break;
        case 1:

            break;
        case 2:

            break;
        case 3:

            break;

        case 4:

            break;

    }

    QUrl url(apiUrl);
    QNetworkRequest req(url);
    networkManager -> get(req);
}

//
void MainWindow::networkReplyFinished(QNetworkReply *reply) {

    //check connection has error
    if (reply -> error() == QNetworkReply::NoError) {
        QByteArray resData = reply -> readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(resData);

        //get json obj
        if (jsonDoc.isObject()) {
            QJsonObject jsonObj = jsonDoc.object();

            //get data from json obj
            QString cityName = jsonObj.value("").toString();
            QString weather = jsonObj.value("").toString();
            double temp = jsonObj.value("").toDouble();
            double pm25 = jsonObj.value("").toDouble();

            //update ui
            ui -> City -> setText(cityName);
            ui ->Temp -> setText(QString::number(temp));
            ui -> Pm25 -> setText(QString::number(pm25));

            if (weather == "") {
                ui ->Weather ->setPixmap(QPixmap(":/images"));
            }

            else if (weather == "") {
                ui ->Weather ->setPixmap(QPixmap(":/images"));
            }

            else if (weather == "") {
                ui ->Weather ->setPixmap(QPixmap(":/images"));
            }

            else {
                ui ->Weather ->setPixmap(QPixmap(":/images"));
            }


        }

        else {
            return;
        }

    }

    else {
        return;
    }

    reply -> deleteLater();
}

//소멸자 호출
MainWindow::~MainWindow()
{

    delete ui;
}
