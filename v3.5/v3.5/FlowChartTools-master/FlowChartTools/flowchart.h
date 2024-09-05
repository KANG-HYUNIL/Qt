﻿#ifndef FLOWCHART_H
#define FLOWCHART_H
//#pragma execution_character_set("utf-8")
#include <map>
#include <stdexcept>

//added codes
#include <QShortcut>

#include <QWidget>
#include <QPainter>
#include <vector>
#include <QDebug>
#include <QMouseEvent>
#include <QPolygon>
#include<QStyleOption>

#include <QUndoCommand>
#include <QUndoStack>

#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QLabel>
#include <QFont>
#include <QPalette>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextDocument>
#include "class_Charts/chart_base.h"
#include "class_Charts/chart_rect.h"
#include "class_Charts/chart_diamond.h"
#include "class_Charts/chart_roundrect.h"
#include "class_Charts/chart_ellipse.h"
#include "class_Charts/chart_line.h"
#include "class_Charts/chart_trapezoid.h"
#include "class_Charts/chart_capsule.h"
#include "class_Charts/chart_circle.h"
#include "class_Charts/chart_document.h"
#include "class_Charts/chart_parallelogram.h"
#include "class_Charts/chart_subroutine.h"
#include "class_Charts/Textbox.h"

#include "comdef.h"

class FlowChart : public QWidget
{
    Q_OBJECT
private:
    int rectX;
    int rectY;
    friend class Chart_Base;
    Chart_Base *curPaintChart = nullptr;                        // 将要放置的图形
    PaintChartType curPaintChartType = PaintChartType::NONE;    // 绘制的图形的类型

    Chart_Base *curSelecChart = nullptr;    // 选中的画板上的图形
    QPoint curSelecChartPos;                // 选中时指针在图像对象上的位置信息

    //added codes
    Chart_Base *copiedChart = nullptr;


    Chart_Line *newLineChart = nullptr;             // 要创建的磁力点的连线的指针
    Chart_Base *newLineToSelectChart = nullptr;     // 要连接到的磁力点的图形的指针
    Chart_Base *newLineFromSelectChart = nullptr;   // 要创建的磁力点的图形的指针
    ORIENTION magPointDirect = ORIENTION::NONE;     // 选中时要创建的起始磁力点的方向类型
    int magPointFromIndex;                          // 要创建的磁力点的索引
    int magPointToIndex;                            // 要创建的磁力点的索引
    int oldx;
    int oldy;
    QPoint curChartPos;

    Chart_Base *lineSelectChart = nullptr;  // 运行时线的吸附到的图形的指针
    int magPointIndex;                      // 运行时线吸附到的图形的磁力点的索引

    ORIENTION sizePointDirect = ORIENTION::NONE;    // 选中时要改变大小的大小点的方向类型

    std::vector<Chart_Base*> charts;    // 画板上图形的集合
    std::vector<Chart_Base*> line;      // 画板上线的集合

    //add new vector for text search
    std::vector<Chart_Base*> textSearchResult; //save the

    MOUSE_EVENT_TYPE mouseEventType = MOUSE_EVENT_TYPE::NONE;   // 当前鼠标事件类型

    bool fileIsOpened = false;      // 是否打开了文件
    bool fileIsSaved = true;        // 是否保存了文件
    QString filePath;               // 打开文件的路径

    void initVar();             // 变量默认初始化函数
    void resetFlowChartPanel(); // 重置流图画板
    void setFileSetSaved(bool isSaved);     // 设置文件未保存
    void setFileNameShow(bool isSaved);     // 设置窗体文件名
    void clearChartsLine(){                 // 释放图形、连线内存
        for(auto it = charts.begin();it!=charts.end();it++) {
            if(*it) delete *it;
            *it = nullptr;
        }
        for(auto it = line.begin();it!=line.end();it++) {
            if(*it) delete *it;
            *it = nullptr;
        }
    }

    //added codes
    void copyMagPointData(Chart_Base *copiedChart, Chart_Base *curSelecChart);



public:
    explicit FlowChart(QWidget *parent = nullptr , Qt::WindowFlags f = Qt::WindowFlags());
    virtual ~FlowChart()
    {
        //delete[] curPaintChart;
        //delete[] curSelecChart;
        //delete[] newLineChart;
        //delete[] newLineToSelectChart;
        newChartFile();
        clearChartsLine();

    }


    Chart_Base * getCurPaintChart();    // 获取新的图形
    Chart_Base * getCurSeleChart();     // 获取当前选中的图形
    void addChart(Chart_Base *cb);        // 添加图形到图形容器
    bool delChart(Chart_Base *&cb);      // 删除图形
    void addLine(Chart_Base *cb);       // 添加线条到线条容器
    bool delLine(Chart_Base *&cb);       // 添加图形
    void hideMagSizeAll();              // 隐藏所有元素
    bool saveFile(QString filePath);    // 保存文件
    bool loadFile(QString filePath);    // 读取文件

    QUndoStack* undoStack;

protected:
virtual void paintEvent(QPaintEvent *event);
virtual void mousePressEvent(QMouseEvent *event);
virtual void mouseMoveEvent(QMouseEvent *event);
virtual void mouseReleaseEvent(QMouseEvent *event);
// virtual void keyPressEvent(QKeyEvent *ev);
// virtual void keyReleaseEvent(QKeyEvent *ev);

//bool eventFilter(QObject *target, QEvent *event);
signals:
    void sendChartStyle(QPen &qp, QBrush &qb);                                                          // 发送图形颜色信息
    void sendLineStyle(QPen &qp, LINE_HEAD_TYPE &startLineHeadType, LINE_HEAD_TYPE &endLineHeadType);   // 发送线条颜色和线头类型信息
    void disableStyle();                        // 关闭主窗体颜色信息设置
    void updateWindowTitle(const QString &);    // 更新主窗体标题
public slots:
    void setPen(QPen &qp){curSelecChart->paintChartDrawPen = qp;}                                                       // 设置笔刷
    void setBrush(QBrush &qb){curSelecChart->paintChartFillPen = qb;}                                                   // 设置填充颜色
    void setLineWidth(int i){curSelecChart->paintChartDrawPen.setWidth(i);}                                             // 设置线宽
    void setLineStartStyle(int i){dynamic_cast<Chart_Line *>(curSelecChart)->setStartLineHeadType(LINE_HEAD_TYPE(i));}  // 设置起点线头类型
    void setLineEndStyle(int i){dynamic_cast<Chart_Line *>(curSelecChart)->setEndLineHeadType(LINE_HEAD_TYPE(i));}      // 设置终点线头类型
public:
    void setSelChartLineColor(const QColor &color);     // 设置图形线条颜色
    void setSelChartFillColor(const QColor &color);     // 设置图形填充颜色

    //add for text search and replace
    void textSearch(QString searchText);
    void onHighlight(QString searchText);
    void offHighlight();
    void replaceText(QString searchText, QString replaceText);

    void insertRect();
    void insertText();

public slots:
    void setMousePressedFlag(MOUSE_EVENT_TYPE);         // 设置鼠标按下事件类型
    void setPaintChart();                           // 设置将要绘制的图形
    void setSelecChart(Chart_Base *, int ,int);     // 设置选中的画板的图像

    //add for text search and replace method
    //void textSearch(QString searchText);
   /* void onHighlight(QString searchText);
    void offHighlight()*/;
    //void replaceText(QString searchText, QString replaceText);

public slots:
    void setPaintChart_Rect(){curPaintChartType = PaintChartType::RECT;setPaintChart();}                // 获取一个新的矩形
    void setPaintChart_Diamond(){curPaintChartType = PaintChartType::DIAMOND;setPaintChart();}          // 获取一个新的菱形
    void setPaintChart_RoundRect(){curPaintChartType = PaintChartType::ROUNDRECT;setPaintChart();}      // 获取一个新的圆角矩形
    void setPaintChart_Ellipse(){curPaintChartType = PaintChartType::ELLIPSE;setPaintChart();}          // 获取一个新的圆形
    void setPaintChart_Line(){curPaintChartType = PaintChartType::LINE;setPaintChart();}                // 获取一个新的线段
    void setPaintChart_trapezoid(){curPaintChartType = PaintChartType::TRAPEZOID;setPaintChart();}      // 获取一个新的梯形
    void setPaintChart_capsule(){curPaintChartType = PaintChartType::CAPSULE;setPaintChart();}
    void setPaintChart_circle(){curPaintChartType = PaintChartType::CIRCLE;setPaintChart();}
    void setPaintChart_document(){curPaintChartType = PaintChartType::DOCUMENT;setPaintChart();}
    void setPaintChart_parallelogram(){curPaintChartType = PaintChartType::PARALLELOGRAM;setPaintChart();}
    void setPaintChart_subroutine(){curPaintChartType = PaintChartType::SUBROUTINE;setPaintChart();}
    void resetPaintChartType(){curPaintChartType = PaintChartType::NONE;}    // 清空类型
    void setPaintChart_Text(){curPaintChartType = PaintChartType::TEXT;setPaintChart();}                // 获取一个新的矩形



public slots:
    void setTypeChangeSize(ORIENTION i){mouseEventType = MOUSE_EVENT_TYPE::RUNTIME_CHANGE_SIZE;sizePointDirect = i; }                                                                                                       // 设置鼠标事件类型为改变大小
    void setTypeCreateMagPoint(Chart_Base *cb,ORIENTION d,int i){newLineFromSelectChart = cb;mouseEventType = MOUSE_EVENT_TYPE::RUNTIME_CREATE_MAGPOINT;magPointDirect = d;magPointFromIndex = i;newLineChart = nullptr; }  // 设置鼠标事件类型为创建线段

    bool openChartFile();   // 打开文件
    bool saveChartFile();   // 保存文件
    bool newChartFile();    // 新建文件

    //added codes
    void deleteSelectChart(Chart_Base * cb);
    void undo() {
        undoStack->undo();
    }

    void redo() {
        undoStack->redo();
    }

};

class AddChartCommand : public QUndoCommand {
public:
    AddChartCommand(FlowChart* flowChart, Chart_Base* chart, QUndoCommand* parent = nullptr)
        : QUndoCommand(parent), m_flowChart(flowChart), m_chart(chart){
    }

    void undo() override {
        m_flowChart->deleteSelectChart(m_chart);
    }

    void redo() override {
    }

private:
    FlowChart* m_flowChart;
    Chart_Base* m_chart;
};

class MoveChartCommand : public QUndoCommand {
public:
    MoveChartCommand(Chart_Base* chart,QPoint oldPos, QPoint newPos, QUndoCommand* parent = nullptr)
        : QUndoCommand(parent), m_chart(chart),m_oldPos(oldPos), m_newPos(newPos) {
    }

    void undo() override {
        if(m_chart)
        {
            m_chart->setXY(m_oldPos.rx(),m_oldPos.ry());
        }
    }

    void redo() override {
        if(m_chart)
        {
            m_chart->setXY(m_newPos.rx(),m_newPos.ry());
        }
    }

private:
    Chart_Base* m_chart;
    QPoint m_oldPos;
    QPoint m_newPos;
};

class ChangeChartSizeCommand : public QUndoCommand {
public:
    ChangeChartSizeCommand(Chart_Base* chart,int oldX,int oldY,int newX, int newY,ORIENTION direction, QUndoCommand* parent = nullptr)
        : QUndoCommand(parent), m_chart(chart), x1(oldX), y1(oldY), x2(newX), y2(newY),dir(direction) {
    }

    void undo() override {
        m_chart->setWidthHeight(x1,y1,dir);
    }

    void redo() override {
        m_chart->setWidthHeight(x2,y2,dir);
    }

private:
    Chart_Base* m_chart;
    int x1;
    int y1;
    int x2;
    int y2;
    ORIENTION dir;

};
#endif // FLOWCHART_H

