#include "videoplayer.h"
#include "ui_videoplayer.h"
#include <QVideoWidget>

videoplayer::videoplayer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::videoplayer)
{

    ui->setupUi(this);
    setWindowTitle("视频播放器");

    audiooutput= new QAudioOutput;
    mediaPlayer=new QMediaPlayer;
    output=new QVideoWidget(this->ui->label);
    mediaPlayer->setVideoOutput(output);
    output->resize(ui->label->size());
    mediaPlayer->setAudioOutput(audiooutput);

    connect(ui->videoBack,SIGNAL(clicked()),this,SLOT(on_videoBack_clicked()));


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
        audiooutput->setVolume(volume);
    });

}

videoplayer::~videoplayer()
{
    delete ui;
}

void videoplayer::on_openButton_clicked()
{
    mediaPlayer->stop();
    playList.clear();
    ui->listWidget->clear();
    QString currentPath = QDir::currentPath();
    qDebug() << "Current working directory:" << currentPath;

    auto path=QFileDialog::getExistingDirectory(this,"选择视频","../../videos");
    QDir dir(path);
    auto musiclist=dir.entryList(QStringList()<<"*.mp3"<<"*.mp4"<<"*.flv"<<"*.f4v"<<"*.mov"<<"*.avi");
    ui->listWidget->addItems(musiclist);
    ui->listWidget->setCurrentRow(0);
    for(auto file:musiclist)
        playList.append(QUrl::fromLocalFile(path+"/"+file));

}


void videoplayer::on_playButton_clicked()
{
    if(playList.empty())
        return;
    switch(mediaPlayer->playbackState())
    {
    case QMediaPlayer::PlaybackState::StoppedState:
    {
        index=ui->listWidget->currentRow();
        mediaPlayer->setSource(playList[index]);
        mediaPlayer->play();
        break;
    }

    case QMediaPlayer::PlaybackState::PausedState:
    {
       // index=ui->listWidget->currentRow();
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


void videoplayer::on_nextButton_clicked()
{
    index=(index+1)%playList.size();
    ui->listWidget->setCurrentRow(index);
    mediaPlayer->setSource(playList[index]);
    mediaPlayer->play();
}


void videoplayer::on_lastButton_clicked()
{
    index=(index-1+4)%playList.size();
    ui->listWidget->setCurrentRow(index);
    mediaPlayer->setSource(playList[index]);
    mediaPlayer->play();
}


void videoplayer::on_listWidget_doubleClicked(const QModelIndex &_index)
{
    index=_index.row();
    mediaPlayer->setSource(playList[_index.row()]);
    mediaPlayer->play();
}



void videoplayer::on_videoBack_clicked()
{
    mediaPlayer->pause();
    emit back();
}

