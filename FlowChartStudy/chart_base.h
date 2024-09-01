#ifndef CHART_BASE_H
#define CHART_BASE_H

#include <QPainter>
#include <QPen>
#include <QPainterPath>
#include <QPolygon>
#include <QWidget>
#include <QPoint>
#include <QDebug>
#include <QMouseEvent>
#include <QLabel>
#include <QLineEdit>
#include <QSizePolicy>
#include <vector>
#include <algorithm>
#include <fstream>
#include <QDataStream>


#include "enums.h"


#define ENABLE 1    // 使能
#define DIABLE 0    // 失能
#define RUNTIME_PAINTMAG DIABLE     // 实时磁力点绘制
#define RUNTIME_PAINTSIZE DIABLE    // 实时大小点绘制


class Chart_Base : public QWidget
{
    Q_OBJECT

private:

    //static variables
    static int sizePointWidth; // width of size control point
    static int sizePointWidth1_2; // half of sizePointWidth
    static int chartIDCount;  //count of id


    //static method
    static void resetStaticVal(){

        Chart_Base::sizePointWidth = 9;
        Chart_Base::sizePointWidth1_2 = sizePointWidth/2;
        Chart_Base::chartIDCount = 0;

    }

    static void updatePaddingInfo(); //update padding settings


    //methods
    void varInit(int spw=9);//
    void widgetPosInit(int x=0,int y=0,int w=minSizeWH,int h=minSizeWH);    // 窗体位置信息默认初始化函数
    void paintInit();        // 图形绘制信息默认初始化
    void pointInit();        // 大小点和控制点默认初始化
    void updateWidgetPosInfo();         // 更新Widget位置信息  @@
    void updatePaintInfo();             // 更新绘图范围信息  @@
    void updateSizePointInfo(); // 更新大小点位置信息  @@
    void updateSizePointPath();         // 更新大小点绘制范围信息  @@
    void paintChart(QPainter & p) = 0;  // 绘制图形  @@
    void paintSizePoint(QPainter & p);          // 绘制大小点  @@@@@@
    void paintSizeEdge(QPainter & p);   // 绘制大小边界  @@
    bool inPath(const QPointF &p);
    bool inGraphisPath(const QPointF &p) {if(graphPath)return graphPath->contains(p);else return false;}// 是否在图形范围内  @@                                        // 是否在磁力点范围内
    bool inSizePath(const QPointF &p, ORIENTION &b) const;



    ORIENTION lastType = ORIENTION::NONE; //
    ORIENTION curIndex = ORIENTION::NONE;  //direction of chosen size control points
    MOUSE_EVENT_TYPE curFlag = MOUSE_EVENT_TYPE::NONE; //type of current mouse events
    PaintChartType chartType = PaintChartType::NONE; // type of shape will draw
    int ID;




protected:

    //protected inner class
    class i_pointbase   // 点基本信息  @@
    {
    public:
        QPoint *i_pos = nullptr;        // 点位置
        QPainterPath *i_path = nullptr; // 点范围
        ORIENTION rotate = ORIENTION::NONE;                 // 点方向

        i_pointbase():i_pos(nullptr),i_path(nullptr),rotate(ORIENTION::NONE){

        }
        ~i_pointbase(){
            if(i_pos){
                delete i_pos;
                i_pos = nullptr;
            }
            if(i_path){
                delete i_path;
                i_path = nullptr;
            }
        }

        //inner class methods
        void setX(int x){i_pos->setX(x);}
        void setY(int y){i_pos->setY(y);}
        int getX() const{return i_pos->rx();}
        int getY() const{return i_pos->ry();}
        void setRotate(ORIENTION r){rotate = r;}
        const ORIENTION & getRotate()const {return rotate;}
        void setPath(const QPainterPath& p){if(i_path) delete i_path;i_path = new QPainterPath(p);}
        QPainterPath* newPath(){if(i_path) delete i_path;i_path = new QPainterPath;return i_path;}
        QPainterPath* getPath() const{return i_path;}
        bool inPath(const QPointF a){ return i_path->contains(a);}


    };


    class i_sizepoint{   // 大小点信息类结构  @@
    public:
        std::vector<i_pointbase*> i_point;  // 大小点指针容器

        explicit i_sizepoint(int c=0):i_point(c){}
        ~i_sizepoint()
        {
            for(auto it = i_point.begin();it!=i_point.end();it++) {
                if(*it) delete *it;
                *it = nullptr;
            }
        }
    };

    //protected variables
    i_sizepoint sizePoint;     // 大小点信息  @@
    QPainterPath *graphPath = nullptr;  // 图形绘制范围  @@
    QPoint paintStart;      // 图形绘制范围起点  @@
    QPoint paintEnd;        // 图形绘制范围终点  @@
    QPoint widgetStart;     // widget在画布上的起始位置：易变  @@
    QPoint widgetEnd;       // widget在画布上的结束位置：易变  @@
    bool movable = true;    // 图形位置可改变   @@


    //protected methods
    virtual void paintEvent(QPaintEvent *event);            // 绘制控件事件  @@
    virtual void mousePressEvent(QMouseEvent *event);       // 鼠标按下事件  @@
    virtual void mouseMoveEvent(QMouseEvent *event);        // 鼠标移动事件  @@
    virtual void mouseReleaseEvent(QMouseEvent *event);     // 鼠标释放事件  @@
    virtual void mouseDoubleClickEvent(QMouseEvent *event); // 鼠标双击事件  @@
    virtual void leaveEvent(QEvent *event);                 // 离开widget事件  @@


signals:
    void sendThisClass(Chart_Base *,int x,int y);       // 发送自己给画布  @@
    void setTypeChangeSize(ORIENTION i);                            // 设置画布中的鼠标状态为改变大小  @@





public:

    //constructors
    explicit Chart_Base(QWidget *parent = nullptr, PaintChartType type = PaintChartType::NONE, bool textType = false, bool mov = true, int mpc=4, int spc=4); // @@
    Chart_Base( int x, int y, int w, int h, QWidget *parent = nullptr, PaintChartType type = PaintChartType::NONE); // can automatic change type @@
    explicit Chart_Base(Chart_Base &);  // @@
    virtual ~Chart_Base() //소멸자 @@
    {
        if(graphPath)
        {
            delete graphPath;
            graphPath = nullptr;
        }
    }


    //methods
    void setXY(int x, int y);                           // 设置位置  @@
    void setWidthHeight(int x, int y);                  // 设置大小、更新数据，用于创建时  @@
    void setWidthHeight(int x, int y, ORIENTION type);  // 设置大小、更新数据，用于创建好之后  @@
    void applyWidthHeight();                            // 更新数据，用于读取时  @@
    void overlapChartMousePressed(QMouseEvent *event);  // 鼠标点击事件Z-index检测  @@
    void overlapChartMouseMove(QMouseEvent *event);     // 鼠标移动事件Z-index检测  @@
    void setMovalbe(bool f);            // 设置可否移动位置  @@
    int & getID(void){return ID;}       // 获得唯一ID值  @@
    static void saveStaticValue(QDataStream &fout); // 保存静态数据成员  @@
    static void loadStaticValue(QDataStream &fin);  // 加载静态数据成员  @@
    friend QDataStream &operator<<(QDataStream &fout, const  Chart_Base &cb);   // 输出运算符重载  @@
    friend QDataStream &operator>>(QDataStream &fin, Chart_Base &cb);           // 输入运算符重载  @@


public slots:


};

#endif // CHART_BASE_H
