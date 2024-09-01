#include "musicplayer.h"
#include "ui_musicplayer.h"
#include <QAudioOutput>

musicplayer::musicplayer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::musicplayer)
{

    ui->setupUi(this);
    setWindowTitle("音乐播放器");
    output=new QAudioOutput(this);
    mediaPlayer=new QMediaPlayer(this);
    mediaPlayer->setAudioOutput(output);

    connect(ui->musicBack,SIGNAL(clicked()),this,SLOT(on_musicBack_clicked()));


    //获取媒体总时长
    connect(mediaPlayer,&QMediaPlayer::durationChanged,this,[=](qint64 duration)
            {
        ui->totaltime->setText(QString("%1:%2").arg(duration/1000/60).arg(duration/1000%60,2,10,QChar('0')));
        ui->playSlider->setRange(0,duration);
    });
    //当前时长
    connect(mediaPlayer,&QMediaPlayer::positionChanged,this,[=](qint64 pos)
            {
        ui->currenttime->setText(QString("%1:%2").arg(pos/1000/60).arg(pos/1000%60,2,10,QChar('0')));
        ui->playSlider->setValue(pos);
            });
    //控制进度
    connect(ui->playSlider,&QSlider::sliderMoved,mediaPlayer,&QMediaPlayer::setPosition);

    //音量
    ui->soundSlider->setRange(0,100.0);
    ui->soundSlider->setValue(50);
    connect(ui->soundSlider,&QSlider::sliderMoved,this,[=](qint64 tickPosition)
            {
        ui->volumn->setText(QString("%1").arg(tickPosition));
            });
    //connect(ui->soundSlider,&QSlider::sliderMoved,output,&QAudioOutput::setVolume);
    connect(ui->soundSlider, &QSlider::sliderMoved, [=](int value) {
        qreal volume = value / 100.0;
        output->setVolume(volume);
    });


}

musicplayer::~musicplayer()
{
    delete ui;
}

void musicplayer::on_openButton_clicked()
{
    mediaPlayer->pause();
    index=0;
    on_pushButton_2_clicked();
    auto path=QFileDialog::getExistingDirectory(this,"选择音乐","../../songs");
    QDir dir(path);
    auto musiclist=dir.entryList(QStringList()<<"*.mp3");
    ui->listwdt->addItems(musiclist);
    ui->listwdt->setCurrentRow(0);

    for(auto file:musiclist)
        playList.append(QUrl::fromLocalFile(path+"/"+file));

}

void musicplayer::on_pushButton_clicked()
{
  /*  QUrl url("http://netradio.radio4.dk/radio4");
    mediaPlayer->setSource(url);
    mediaPlayer->play();
*/
    mediaPlayer->pause();
    index=0;
  on_pushButton_2_clicked();
    QFile file("../../urls.txt");
    QList<QString> musiclist;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
       QTextStream in(&file);
       while (!in.atEnd()) {
            QString line = in.readLine().trimmed(); // 读取一行并去除前后空白
            if (!line.isEmpty()) {
                QUrl url(line); // 创建 QUrl 对象
               if (url.isValid()) { // 检查 URL 是否有效
                   playList.append(url); // 将有效的 URL 添加到 QList 中
               } else {
                     qWarning() << "Invalid URL:" << line;
               }

               musiclist.append(line);
           }
        }
        file.close();
    } else {
         qWarning() << "Failed to open file:" << file.errorString();
        }

    ui->listwdt->addItems(musiclist);
    ui->listwdt->setCurrentRow(0);

  // 输出 playList 中的 URL 以进行验证
  for (const QUrl &url : playList) {
      qDebug() << url.toString();
    }
 // qDebug()<<playList.size();

}



void musicplayer::on_playButton_clicked()
{
    if(playList.empty())
        return;
    switch(mediaPlayer->playbackState())
    {
    case QMediaPlayer::PlaybackState::StoppedState:
    {
        index=ui->listwdt->currentRow();
        qDebug()<<index;
        mediaPlayer->setSource(playList[index]);
        mediaPlayer->play();
        break;
    }

    case QMediaPlayer::PlaybackState::PausedState:
    {
       // index=ui->listwdt->currentRow();
        qDebug()<<index;
       // mediaPlayer->setSource(playList[index]);
        mediaPlayer->play();
        break;
    }

    case QMediaPlayer::PlaybackState::PlayingState:
    {
        mediaPlayer->pause();
        break;
    }

    }

}


void musicplayer::on_nextButton_clicked()
{
    index=(index+1)%playList.size();
    qDebug()<<index;
    ui->listwdt->setCurrentRow(index);
    mediaPlayer->setSource(playList[index]);
    mediaPlayer->play();
}


void musicplayer::on_lastButton_clicked()
{
    index=(index-1+playList.size())%playList.size();
    qDebug()<<index;
    ui->listwdt->setCurrentRow(index);
    mediaPlayer->setSource(playList[index]);
    mediaPlayer->play();
}


void musicplayer::on_listwdt_doubleClicked(const QModelIndex &_index)
{
    index=_index.row();
    qDebug()<<index;
    mediaPlayer->setSource(playList[_index.row()]);
    mediaPlayer->play();
}





void musicplayer::on_musicBack_clicked()
{
    mediaPlayer->pause();
    emit back();
}


void musicplayer::on_pushButton_2_clicked()
{
    mediaPlayer->stop();
    playList.clear();
    ui->listwdt->clear();

}

