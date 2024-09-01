#ifndef musicplayer_H
#define musicplayer_H

#include <QMainWindow>
#include <QList>
#include <QString>
#include <QMediaPlayer>
#include <QFileDialog>
#include <QAudioOutput>
#include <QUrl>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QStringList>


QT_BEGIN_NAMESPACE
namespace Ui {
class musicplayer;
}
QT_END_NAMESPACE




class musicplayer : public QMainWindow
{    Q_OBJECT

public:
    musicplayer(QWidget *parent = nullptr);
    ~musicplayer();

private slots:
    void on_openButton_clicked();

    void on_playButton_clicked();

    void on_nextButton_clicked();

    void on_lastButton_clicked();

    void on_listwdt_doubleClicked(const QModelIndex &index);


    void on_pushButton_clicked();

    void on_musicBack_clicked();

    void on_pushButton_2_clicked();

signals:
    void back();

private:
    Ui::musicplayer *ui;
    QList<QUrl> playList;//播放列表
    QMediaPlayer* mediaPlayer;
    QAudioOutput* output;
    int index = 0;

};
#endif // musicplayer_H
