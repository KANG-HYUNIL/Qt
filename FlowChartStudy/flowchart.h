#ifndef FLOWCHART_H
#define FLOWCHART_H

#include <QWidget>
#include <QPainter>
#include <vector>
#include <QDebug>
#include <QMouseEvent>
#include <QPolygon>
#include<QStyleOption>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>

#include "enums.h"
#include "chart_parent.h"
#include "chart_line.h"

class FlowChart : public QWidget
{
    Q_OBJECT

private:
    friend class Chart_parent; //by using freind keyword, can use all of variables and methods
    Chart_parent *curPaintChart = nullptr; //shape will be placed
    PaintChartType curPaintChartType = PaintChartType::NONE; // type of shape

    Chart_parent *curSelectChart = nullptr; //shape that selected
    QPoint curSelectChartPos; //position of pointer on selected shape

    Chart_Line *newLineChart = nullptr;
    Chart_parent *newLineToSelectChart = nullptr;
    Chart_parent *newLineFromSelectChart = nullptr;
    ORIENTION magPointDirect = ORIENTION::NONE;
    int magPointFromIndex;
    int magPointToIndex;


    Chart_parent *lineSelectChart = nullptr;  // 运行时线的吸附到的图形的指针
    int magPointIndex;                      // 运行时线吸附到的图形的磁力点的索引

    ORIENTION sizePointDirect = ORIENTION::NONE;    // 选中时要改变大小的大小点的方向类型

    std::vector<Chart_parent*> charts;    // 画板上图形的集合
    std::vector<Chart_parent*> line;      // 画板上线的集合

    MOUSE_EVENT_TYPE mouseEventType = MOUSE_EVENT_TYPE::NONE;   // 当前鼠标事件类型

    // clear the vector of lines and shapes
    void clearChart() {
        for(auto it = charts.begin();it!=charts.end();it++) {
            if(*it) delete *it;
            *it = nullptr;
        }
        for(auto it = line.begin();it!=line.end();it++) {
            if(*it) delete *it;
            *it = nullptr;
        }
    }


    //variable, method for control files
    bool fileIsOpened = false;      // 是否打开了文件
    bool fileIsSaved = true;        // 是否保存了文件
    QString filePath;               // 打开文件的路径

    void initVar();             // 变量默认初始化函数
    void resetFlowChartPanel(); // 重置流图画板
    void setFileSetSaved(bool isSaved);     // 设置文件未保存
    void setFileNameShow(bool isSaved);     // 设置窗体文件名


public:
    explicit FlowChart(QWidget *parent = nullptr , Qt::WindowFlags f = Qt::WindowFlags());
    virtual ~FlowChart()
    {

        // newChartFile();
        // clearChartsLine();

    }

    Chart_parent * getCurPaintChart();    // 获取新的图形
    Chart_parent * getCurSelectChart();     // 获取当前选中的图形
    void addChart(Chart_parent *cb);        // 添加图形到图形容器
    bool delChart(Chart_parent *&cb);      // 删除图形
    void addLine(Chart_parent *cb);       // 添加线条到线条容器
    bool delLine(Chart_parent *&cb);       // 添加图形


    void setSelChartLineColor(const QColor &color);     // 设置图形线条颜色
    void setSelChartFillColor(const QColor &color);     // 设置图形填充颜色


    void hideMagSizeAll();              // 隐藏所有元素
    bool saveFile(QString filePath);    // 保存文件
    bool loadFile(QString filePath);    // 读取文件


protected:

    //mouse, keyboard event
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *ev);
    virtual void keyReleaseEvent(QKeyEvent *ev);


signals:

    void sendChartStyle(QPen &qp, QBrush &qb);   // send chart's color data

    //send line's color and arrow data
    void sendLineStyle(QPen &qp, LINE_HEAD_TYPE &startLineHeadType, LINE_HEAD_TYPE &endLineHeadType);
    void disableStyle();                        // 关闭主窗体颜色信息设置
    void updateWindowTitle(const QString &);    // 更新主窗体标题


public slots:

    void setMousePressedFlag(MOUSE_EVENT_TYPE);         // 设置鼠标按下事件类型
    void setPaintChart();                           // set the shape will draw
    void setSelectChart(Chart_parent *, int ,int);     // 设置选中的画板的图像


    void setLineStartStyle(int i){dynamic_cast<Chart_Line *>(curSelectChart)->setStartLineHeadType(LINE_HEAD_TYPE(i));}  // 设置起点线头类型
    void setLineEndStyle(int i){dynamic_cast<Chart_Line *>(curSelectChart)->setEndLineHeadType(LINE_HEAD_TYPE(i));}      // 设置终点线头类型
    void setPen(QPen &qp){curSelectChart->paintChartDrawPen = qp;}                                                       // 设置笔刷
    void setBrush(QBrush &qb){curSelectChart->paintChartFillPen = qb;}                                                   // 设置填充颜色
    void setLineWidth(int i){curSelectChart->paintChartDrawPen.setWidth(i);}


    //setPaintChart_~~
    void resetPaintChartType(){curPaintChartType = PaintChartType::NONE;}
    void setPaintChart_Line(){curPaintChartType = PaintChartType::LINE;setPaintChart();}                // 获取一个新的线段


    //setType~~
    void setTypeChangeSize(ORIENTION i){mouseEventType = MOUSE_EVENT_TYPE::RUNTIME_CHANGE_SIZE;sizePointDirect = i; }                                                                                                       // 设置鼠标事件类型为改变大小
    void setTypeCreateMagPoint(Chart_parent *cb,ORIENTION d,int i)
    {
        newLineFromSelectChart = cb;
        mouseEventType = MOUSE_EVENT_TYPE::RUNTIME_CREATE_MAGPOINT;
        magPointDirect = d;magPointFromIndex = i;
        newLineChart = nullptr; }  // 设置鼠标事件类型为创建线段


    //control files
    bool openChartFile();   // 打开文件
    bool saveChartFile();   // 保存文件
    bool newChartFile();    // 新建文件



};







#endif // FLOWCHART_H
