#ifndef CLOCK_H
#define CLOCK_H

#include <QMainWindow>
#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QtMath>
#include <QTimer>
#include <QDate>
#include <QCalendarWidget>
#define PI 3.14

QT_BEGIN_NAMESPACE
namespace Ui {
    class Clock;
}
QT_END_NAMESPACE

class Clock: public QMainWindow
{
    Q_OBJECT

public:
    Clock(QMainWindow *parent = 0);
    ~Clock();
    void paintEvent(QPaintEvent *); //重写绘制事件函数
    void Init_Parameter();//初始化参数函数
    void Set_Mask(QPainter *);//设置窗体透明
    void Draw_Clock(QPainter *);//绘制时钟
    void Draw_Broad(QPainter *);//绘制时钟盘
    void Draw_Dial(QPainter *);//绘制刻度盘
    void Draw_Text(QPainter *);//绘制刻度值
    void Draw_Pointer(QPainter *);//绘制指针
    QString getDate();

public slots:
    void openCalendar();
    //QCalendarWidget *c;

//返回主界面信号
signals:
    void back();

private slots:
    void on_ClockBack_clicked();

private:
    Ui::Clock* ui;
    QTimer *timer;//定时器
    QPoint Center_pos;//时钟圆心坐标
    int R_Edge;//外部圆半径
    int R_Inside;//内部圆半径
    int R_Center;//中心小圆半径
    int R_Pointer;//中心指针圆半径
    QColor Color_Edge = QColor(0,0,0,255);//外部圆颜色
    QColor Color_Inside = QColor(255,255,255,255);//内部圆颜色
    QColor Color_Center = QColor(0,0,0,255);//中心小圆颜色
    int Div_Max = 12;//大刻度值
    int Div_Min = 5;//小刻度值
    float BaseAngle = 270;//基础仰角
    int H;//时
    int M;//分
    int S;//秒
};
#endif // CLOCK_H
