#ifndef videoplayer_H
#define videoplayer_H

#include <QMainWindow>
#include <QList>
#include <QString>
#include <QMediaPlayer>
#include <QFileDialog>
#include <QAudioOutput>
#include <QUrl>
#include <QVideoWidget>



QT_BEGIN_NAMESPACE
namespace Ui {
class videoplayer;
}
QT_END_NAMESPACE




class videoplayer : public QMainWindow
{
    Q_OBJECT

public:
    videoplayer(QWidget *parent = nullptr);
    ~videoplayer();

private slots:
    void on_openButton_clicked();

    void on_playButton_clicked();

    void on_nextButton_clicked();

    void on_lastButton_clicked();

    void on_listWidget_doubleClicked(const QModelIndex &index);


    void on_videoBack_clicked();


signals:
    void back();

private:
    Ui::videoplayer *ui;
    QList<QUrl> playList;//播放列表
    QMediaPlayer* mediaPlayer;
    QVideoWidget* output;
    int index = 0;
    QAudioOutput* audiooutput;

};
#endif // videoplayer_H
