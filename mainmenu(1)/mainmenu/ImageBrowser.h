#ifndef IMAGEBROWSER_H
#define IMAGEBROWSER_H

#include <QMainWindow>
#include <QStringList>
#include <QImage>
#include <QPixmap>
#include <QMimeData>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QWheelEvent>
#include <QUrl>


namespace Ui {
class ImageBrowser;
}


class ImageBrowser : public QMainWindow
{
    Q_OBJECT

public:
    explicit ImageBrowser(QWidget *parent = nullptr);
    ~ImageBrowser();
    void slotConnect();
    void loadImage(QString fillAllPath);  //show the first image
    void loadImage(QPixmap pixmap);  //refresh zoomed image
    void switchImage(int index); //load image to label when button pressed
    void showInfo(QString pathName,int curIndex,int total); //pin the info of image under label
    void showTip(QString tipStr);

private slots:
    void openFileTriggered(); //open folder and show image list in QListWidget
    //void deleteImageTriggered();
    void listWidgetSwitch(int currentRow); //switch image by choosing from list

    void zoomInTriggered();
    void zoomOutTriggered();

    void previousImage();
    void nextImage();

    void leftRotateTriggered();
    void rightRotateTriggered();

    void horizontalFlipTriggered();
    void verticalFlipTrigerred();

    void on_ImageBack_clicked();

signals:

    void back();

private:
    QString filePath;
    QStringList fullPathOfFiles;

    int totalNumber;
    bool fileDirOpen;
    QImage zoomImage;

    int imageIndex;
    int imageImportIndex=0;



private:
    Ui::ImageBrowser *ui;
};
#endif // IMAGEBROWSER_H
