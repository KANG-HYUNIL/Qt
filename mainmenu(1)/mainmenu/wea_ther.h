#ifndef wea_ther_H
#define wea_ther_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui {
class wea_ther;
}
QT_END_NAMESPACE

class wea_ther : public QMainWindow
{
    Q_OBJECT

public:
    wea_ther(QWidget *parent = nullptr);
    ~wea_ther();

private slots:



    void on_pushButton_clicked();

    void on_comboBox_currentTextChanged(QString);

    void on_weatherback_clicked();

signals:

    void back();


private:

    Ui::wea_ther *ui;

    QUrl *areaurl;
    QString area;


};
#endif // wea_ther_H
