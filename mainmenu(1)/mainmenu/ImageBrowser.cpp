#include "ImageBrowser.h"
#include "ui_ImageBrowser.h"
#include <QGraphicsScene>
#include <QFileDialog>
#include <QListWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QTransform>
#include <QList>
#include <QAction>
#include <QDir>
#include <QDebug>
#include "imagewidget.h"

#define ISNULL(IMG) if(IMG.isNull()){return;}
constexpr int rotateAngle = 90;

ImageBrowser::ImageBrowser(QWidget *parent) :
    QMainWindow(parent)
    ,totalNumber(0)
    ,fileDirOpen(false)
    ,imageIndex(0)
    , ui(new Ui::ImageBrowser)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("图片浏览器"));
    this->setAcceptDrops(true);

    slotConnect();
}

ImageBrowser::~ImageBrowser()
{
    delete ui;
}


void ImageBrowser::slotConnect()
{
    connect(ui->ImageBack,SIGNAL(clicked()),this,SLOT(on_ImageBack_clicked()));


    connect(ui->actionOpenFile,&QAction::triggered,this,&ImageBrowser::openFileTriggered);

    connect(ui->actionZoomIn,&QAction::triggered,this,&ImageBrowser::zoomInTriggered);
    connect(ui->actionZoomOut,&QAction::triggered,this,&ImageBrowser::zoomOutTriggered);

    //connect(ui->actionDelete,&QAction::triggered,this,&ImageBrowser::deleteImageTriggered);
    connect(ui->actionLeftRotate,&QAction::triggered,this,&ImageBrowser::leftRotateTriggered);
    connect(ui->actionRightRotate,&QAction::triggered,this,&ImageBrowser::rightRotateTriggered);
    connect(ui->actionhorizontalFlip,&QAction::triggered,this,&ImageBrowser::horizontalFlipTriggered);
    connect(ui->actionverticalFlip,&QAction::triggered,this,&ImageBrowser::verticalFlipTrigerred);


    connect(ui->actionExit,&QAction::triggered,this,[]{exit(0);});
    connect(ui->previousButton,&QPushButton::clicked,this,&ImageBrowser::previousImage);
    connect(ui->nextButton,&QPushButton::clicked,this,&ImageBrowser::nextImage);
    connect(ui->imageList,&QListWidget::currentRowChanged,this,&ImageBrowser::listWidgetSwitch);

}

void ImageBrowser::loadImage(QString fillAllPath)  //show the first image
{
    QImage temp;
    fileDirOpen = true;
    temp.load(fillAllPath);
    zoomImage.load(fillAllPath);
    ISNULL(temp)
    QPixmap ConvertPixmap=QPixmap::fromImage(temp);//The QPixmap class is an off-screen image representation that can be used as a paint device
    QGraphicsScene  *qgraphicsScene = new QGraphicsScene;//要用QGraphicsView就必须要有QGraphicsScene搭配着用
    ImageWidget  *imageWidget= new ImageWidget(&ConvertPixmap);//实例化类
    int nwidth = ui->ImageView->width();//获取界面控件Graphics View的宽度
    int nheight = ui->ImageView->height();//获取界面控件Graphics View的高度
    //QGraphicsRectItem *m_image = new QGraphicsRectItem(0, 0, nwidth, nheight); //将界面控件Graphics View的width和height传进类m_Image中
    imageWidget->setQGraphicsViewWH(nwidth,nheight);
    qgraphicsScene->addItem(imageWidget);//将QGraphicsItem类对象放进QGraphicsScene中
    ui->ImageView->setSceneRect(QRectF(-(nwidth/2),-(nheight/2),nwidth,nheight));//使视窗的大小固定在原始大小，不会随图片的放大而放大（默认状态下图片放大的时候视窗两边会自动出现滚动条，并且视窗内的视野会变大），防止图片放大后重新缩小的时候视窗太大而不方便观察图片
    ui->ImageView->setScene(qgraphicsScene);//Sets the current scene to scene. If scene is already being viewed, this function does nothing.
    ui->ImageView->setFocus();//将界面的焦点设置到当前Graphics View控件
    ui->ImageView->show();
    //ui->ImageView->setScene(qgraphicsScene);
    showInfo(fillAllPath,imageImportIndex,totalNumber);
    imageImportIndex = totalNumber; //refresh first img when reopen folder/drag in

}

void ImageBrowser::switchImage(int index) //load image to label when button pressed
{
    QImage image;
    QString imagePath = fullPathOfFiles.at(index);
    zoomImage.load(imagePath);
    image.load(imagePath);
    ISNULL(image);
    loadImage(imagePath);
    showInfo(imagePath,index,totalNumber);
    imageIndex = index;
    ui->imageList->setCurrentRow(index);
}

void ImageBrowser::showInfo(QString pathName,int curIndex,int total) //pin the info of image under label
{
    QString curInfo = "当前文件名" + pathName;
    curInfo = curInfo + "  " + "第" + QString::number(curIndex+1)+ "/" + QString::number(total) + "张";
    ui->imageInfo->setText(curInfo);
}

void ImageBrowser::showTip(QString tipStr)
{
    QMessageBox::warning(this,tr("文件提示："),tipStr,QMessageBox::Ok);
}

void ImageBrowser::openFileTriggered() //open folder and show image list in QListgraphicsView
{
    filePath = QFileDialog::getExistingDirectory(this,"Open Dir","../");
    if(filePath.isEmpty()){
        return;
    }
    QDir openDir(filePath);
    QStringList filter;
    filter<<"*.jpg"<<"*.png"<<"*.bmp"<<"*.jpeg"<<"*.ppm"<<
        "*.JPG"<<"*.PNG"<<"*.JPEG";
    openDir.setNameFilters(filter);

    QStringList imageFileList = openDir.entryList(filter);
    for(int i = 0; i < imageFileList.size(); i++){
        QString fullDir = filePath + "/" + imageFileList.at(i);
        totalNumber == 0 ? fullPathOfFiles.insert(i,fullDir) : fullPathOfFiles.insert(totalNumber + i, fullDir);

        QListWidgetItem *signalImageItem = new QListWidgetItem(QIcon(fullDir),fullDir);
        ui->imageList->addItem(signalImageItem);
    }

    ui->imageList->setCurrentRow(totalNumber);
    totalNumber += imageFileList.size();
    loadImage(fullPathOfFiles.at(0));
}
/*
void ImageBrowser::deleteImageTriggered()
{
    int curIndex = ui->imageList->currentRow();
    if(curIndex < 0){
        fileDirOpen = false;
        return;
    }

    fullPathOfFiles.removeAt(curIndex);
    ui->imageList->takeItem((curIndex));
    totalNumber = fullPathOfFiles.size();
    imageImportIndex = totalNumber;
    if(totalNumber>0){
        QString str = ui->imageList->currentItem()->text();
        showInfo(str,ui->imageList->currentRow(),totalNumber);
    }
    else{
        ui->graphicsView->clear();
        return;
    }
}
*/
void ImageBrowser::listWidgetSwitch(int currentRow) //switch image by choosing from list
{
    if(currentRow==-1) return;

    QImage curImage;
    QString currentPic = ui->imageList->item(currentRow)->text();
    zoomImage.load(currentPic);
    curImage.load(currentPic);
    ISNULL(curImage)
    loadImage(currentPic);
    showInfo(currentPic,currentRow,totalNumber);
}

void ImageBrowser::zoomInTriggered()
{
    double scaleFactor=1.2;
    ui->ImageView->scale(scaleFactor,scaleFactor);
}

void ImageBrowser::zoomOutTriggered()
{
    double scaleFactor=1/1.2;
    ui->ImageView->scale(scaleFactor,scaleFactor);
}


void ImageBrowser::previousImage()
{
    if(false==fileDirOpen){
        showTip("请先选择需要打开的文件");
        return;
    }

    imageIndex = ui->imageList->currentRow();
    --imageIndex;
    if(imageIndex<0){
        showTip("当前是列表中第一张图片，将切换到最后一张");
        imageIndex = fullPathOfFiles.size()-1;
    }

    switchImage(imageIndex);
}

void ImageBrowser::nextImage()
{
    if(false==fileDirOpen){
        showTip("请先选择需要打开的文件");
        return;
    }

    imageIndex = ui->imageList->currentRow();
    ++imageIndex;
    if(imageIndex>fullPathOfFiles.size()-1){
        showTip("已浏览至最后一张图片，将从第一张图片开始播放");
        imageIndex = 0;
    }

    switchImage(imageIndex);
}

void ImageBrowser::leftRotateTriggered()
{
    ISNULL(zoomImage)
    QTransform leftTransform;
    leftTransform.rotate(rotateAngle);
    ui->ImageView->setTransform(ui->ImageView->transform()*leftTransform);

}

void ImageBrowser::rightRotateTriggered()
{
    ISNULL(zoomImage)
    QTransform rightTransform;
    rightTransform.rotate(-rotateAngle);
    ui->ImageView->setTransform(ui->ImageView->transform()*rightTransform);

}

void ImageBrowser::horizontalFlipTriggered()
{
    ISNULL(zoomImage)
    QTransform horizontalFlip;
    horizontalFlip.scale(-1,1);
    ui->ImageView->setTransform(ui->ImageView->transform()*horizontalFlip);

}
void ImageBrowser::verticalFlipTrigerred()
{
    ISNULL(zoomImage)
    QTransform verticalFlip;
    verticalFlip.scale(1,-1);
    ui->ImageView->setTransform(ui->ImageView->transform()*verticalFlip);


}




void ImageBrowser::on_ImageBack_clicked()
{
    emit back();
}

