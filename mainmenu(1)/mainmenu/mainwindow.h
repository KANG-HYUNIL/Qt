#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ImageBrowser.h"
#include "Clock.h"
#include <QMainWindow>
#include "musicplayer.h"
#include "videoplayer.h"
#include "BitMap.h"
#include "client.h"
#include "wea_ther.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_ClockButton_clicked();

    void on_ImageBrowserButton_clicked();

    void on_VideoButton_clicked();

    void on_MusicButton_clicked();

    void on_MapButton_clicked();

    void on_ChatButton_clicked();

    void on_wea_therButton_clicked();

private:

    //获取星期
    QString getWeek();

    void updateTime();


    //加上窗口声明
    ImageBrowser *imagebrowser;
    Clock *clock;
    musicplayer *music_player;
    videoplayer *video_player;
    BitMap *bitmap;
    client *clients;
    wea_ther *weather;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
