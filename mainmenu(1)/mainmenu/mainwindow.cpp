#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ImageBrowser.h"
#include "Clock.h"
#include "musicplayer.h"
#include "videoplayer.h"
#include "client.h"
#include "wea_ther.h"

#include <QPushButton>
#include <QObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


 /*   // 创建两个个QLabel用于显示时间
    timeLabel = new QLabel(this);
    timeLabel->setObjectName("timeLabel");
    yearLabel = new QLabel(this);
    yearLabel->setObjectName("yearLabel");*/

    // 使用自定义格式
    // 获取当前时间，并格式化为字符串
    QString currentTime = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString yearTime = QDateTime::currentDateTime().toString("yyyy年MM月dd日 ");

    //将获取的星期几加入到yearTime字符串中
    yearTime += getWeek();

    //设置时间字符串
    ui->timeLabel->setText(currentTime);
    ui->yearLabel->setText(yearTime);

    // 定时更新时间显示
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTime);
    timer->start(1000); // 每秒更新一次时间

  /*  //获取当前屏幕分辨率
    auto screen = QApplication::primaryScreen();

    // 获取屏幕的宽和高
    QSize screenSize = screen->size();
    auto width = screenSize.width();
    auto height = screenSize.height();

    //设置位置与大小
    timeLabel->setGeometry(QRect(width/2-width/9*2, height/2-width/9*2-width/20, width/2, width/8));
    yearLabel->setGeometry(QRect(width/2-width/9*2+width/50, height/2-width/20, width/2, width/30));

*/

    //初始化窗口
    imagebrowser = new ImageBrowser();
    clock = new Clock();
    music_player = new musicplayer();
    video_player = new videoplayer();
    bitmap = new BitMap();
    clients = new client();
    weather = new wea_ther();

    //隐藏窗口
    clock->hide();
    imagebrowser->hide();
    music_player->hide();
    video_player->hide();
    bitmap->hide();
    clients->hide();
    weather->hide();

    //按钮槽函数
    connect(ui->ImageBrowserButton,SIGNAL(Clicked()),this,SLOT(on_ImageBrowserButton_clicked()));
    connect(ui->ClockButton,SIGNAL(Clicked()),this,SLOT(on_ClockButton_clicked()));
    connect(ui->VideoButton,SIGNAL(Clicked()),this,SLOT(on_VideoButton_clicked()));
    connect(ui->MusicButton,SIGNAL(Clicked()),this,SLOT(on_MusicButton_clicked()));
    connect(ui->MapButton,SIGNAL(Clicked()),this,SLOT(on_MapButton_clicked()));
    connect(ui->ChatButton, SIGNAL(clicked(bool)), this, SLOT(on_ChatButton_clicked()));
    connect(ui->wea_therButton,SIGNAL(Clicked()),this,SLOT(on_wea_therButton_clicked()));

    //监听返回信号
    connect(clock,&Clock::back,[=](){
        clock->hide();
        this->show();
    });
    connect(imagebrowser,&ImageBrowser::back,[=](){
        imagebrowser->hide();
        this->show();
    });
    connect(video_player,&videoplayer::back,[=](){
        video_player->hide();
        this->show();
    });
    connect(music_player,&musicplayer::back,[=](){
        music_player->hide();
        this->show();
    });
    connect(bitmap,&BitMap::back,[=](){
        bitmap->hide();
        this->show();
    });
    connect(clients, &client::back, [=]() {
        clients->hide();
        this->show();
    });
    connect(weather, &wea_ther::back, [=]() {
        weather->hide();
        this->show();
    });
}

MainWindow::~MainWindow()
{

    delete ui;
}


//获取当前是星期几
QString MainWindow::getWeek()
{
    // 获取当前日期和时间
    QDateTime Time = QDateTime::currentDateTime();

    // 获取当前日期
    QDate currentDate = Time.date();

    // 获取当前是星期几（Qt::DayOfWeek枚举）
    int dayOfWeek = currentDate.dayOfWeek();

    // 将枚举值转换为可读的字符串（这里以英文为例，你可以根据需要替换为本地化的字符串）
    QString dayOfWeekString;
    switch (dayOfWeek) {
    case Qt::Sunday:
        dayOfWeekString = "星期日";
        break;
    case Qt::Monday:
        dayOfWeekString = "星期一";
        break;
    case Qt::Tuesday:
        dayOfWeekString = "星期二";
        break;
    case Qt::Wednesday:
        dayOfWeekString = "星期三";
        break;
    case Qt::Thursday:
        dayOfWeekString = "星期四";
        break;
    case Qt::Friday:
        dayOfWeekString = "星期五";
        break;
    case Qt::Saturday:
        dayOfWeekString = "星期六";
        break;
    }
    return dayOfWeekString;
}


//定时器超时调用的更新时间函数
void MainWindow::updateTime()
{
    // 使用自定义格式
    // 获取当前时间，并格式化为字符串
    QString currentTime = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString yearTime = QDateTime::currentDateTime().toString("yyyy年MM月dd日 ");

    //将获取的星期几加入到yearTime字符串中
    yearTime += getWeek();

    // 找到显示时间的QLabel并更新其文本
    ui->timeLabel->setText(currentTime);
    ui->yearLabel->setText(yearTime);
}




//显示功能窗口

void MainWindow::on_ClockButton_clicked()
{
    this->hide();
    clock->show();
}


void MainWindow::on_ImageBrowserButton_clicked()
{
    this->hide();
    imagebrowser->show();
}

void MainWindow::on_VideoButton_clicked()
{
    this->hide();
    video_player->show();
}


void MainWindow::on_MusicButton_clicked()
{
    this->hide();
    music_player->show();
}

void MainWindow::on_MapButton_clicked()
{
    this->hide();
    bitmap->show();
}

void MainWindow::on_ChatButton_clicked()
{
    this->hide();
    clients->show();
}

void MainWindow::on_wea_therButton_clicked()
{
    this->hide();
    weather->show();
}
