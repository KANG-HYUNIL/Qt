#ifndef CHART_LINE_H
#define CHART_LINE_H
#include "chart_base.h"
#include <QtMath>
const static double sin45Val = 0.707106781;
const static double cos45Val = 0.707106781;
const static double sin75Val = 0.9659258262890;
const static double cos75Val = 0.25881904510;
const static double sin15Val = 0.2588190;
const static double cos15Val = 0.965925826;

class Chart_Line : public Chart_Base
{
private:

    const static int containsWidth = 20;                    // 可选范围长度 @@
    const static int containsWidth1_2 = containsWidth>>1;   // 可选范围一般长度 @@
    const static int extendWidth = 30;              // 边界线段延伸长度 @@
    const static int arrayLength = 18;              // 线头绘制范围 - 长度 @@
    const static int arrayWidth = 9;                // 线头绘制范围 - 宽度 @@
    const static int arrayLineHeadLenM2 = arrayLength * arrayLength + arrayWidth * arrayWidth;
    static double extendWidth45;
    static double arrayLength45;
    static double arrayLength75;
    static double arrayWidth75;
    static double arrayLength15;
    static double arrayWidth15;


    virtual void paintChart(QPainter & p); //@@
    virtual void paintSizeEdge(QPainter & p){} //@@
    virtual void updateMagPointInfo();
    virtual void updateSizePointInfo(); //@@
    virtual void updateMagPointLine(){}
    virtual void specialWidgetUpdate(int &x,int &y, int &w, int &h);
    virtual void specialPaintUpdate(QPoint &s, QPoint &e);

#define OPTIMIZATION 0 // 类型优化，减少switch语句，1开启*9/
#if (OPTIMIZATION == 1)
    void angle0(QPainterPath &linePath, int &sx,int &sy, int &ex, int &ey);
    void anglePos90(QPainterPath &linePath, int &sx,int &sy, int &ex, int &ey);
    void angleNeg90(QPainterPath &linePath, int &sx,int &sy, int &ex, int &ey);
    void angle1EastWest(QPainterPath &linePath, int &sx,int &sy, int &ex, int &ey);
    void angle1SouthNorth(QPainterPath &linePath, int &sx,int &sy, int &ex, int &ey);
#endif
    QPoint startPos;    // 开始坐标 @@
    QPoint endPos;      // 结束坐标 @@

    ORIENTION startDirect = ORIENTION::STARTPOINT;  // 开始点方向 @@
    ORIENTION endDirect = ORIENTION::ENDPOINT;      // 结束点方向 @@

    //이 아래 변수들은 어떻게 사용되는지 확인이 필요함.
    //flowChart에서 자석점 이동 및 활용에 사용되는 변수들.
    int startMagIndex = 0;                  // 起始点索引值
    int endMagIndex = 0;                    // 结束点索引值

    // 이 위의 변수까지.

    LINE_HEAD_TYPE startLineHeadType = LINE_HEAD_TYPE::ARROW0;   // 起始点线头方向 @@
    LINE_HEAD_TYPE endLineHeadType = LINE_HEAD_TYPE::ARROW1;     // 结束点线头方向 @@

    void drawLineHead(const ORIENTION o,const LINE_HEAD_TYPE lht,const int x,const int y,QPainter & p,QPainterPath &linePath, QPainterPath &graphPath); // 绘制线头 @@
    void drawStraightLine(int sx, int sy, int ex, int ey,QPainterPath &linePath, QPainterPath &graphPath);          // 直连线绘制 @@

public:

    //added codes
    Chart_Base *startChartMag = nullptr;    // 起始图形
    Chart_Base *endChartMag = nullptr;      // 终止图形

    //생성자들은 Base 클래스의 변경 사항에 따라 조정이 필요하다
    Chart_Line(QWidget *parent = nullptr, PaintChartType type = PaintChartType::LINE) : Chart_Base(parent,type,true,false,0,2)
    {
        extendWidth45 = extendWidth * sin45Val;
        arrayLength45 = arrayLength * sin45Val;
#if 1
        double tmpRad45AddX = qDegreesToRadians(45.0) + qAtan(double(arrayWidth)/arrayLength);
        double tmpRad45SubX = qDegreesToRadians(45.0) - qAtan(double(arrayWidth)/arrayLength);
        double arrayLineHeadLen = sqrt(arrayLineHeadLenM2);
        arrayLength75 = arrayLineHeadLen * qCos(tmpRad45AddX);
        arrayWidth75 = arrayLineHeadLen * qSin(tmpRad45AddX);
        arrayLength15 = arrayLineHeadLen * qCos(tmpRad45SubX);
        arrayWidth15 = arrayLineHeadLen * qSin(tmpRad45SubX);
        qDebug()<<arrayLength<<" "<<sqrt((arrayLength * arrayLength + arrayWidth * arrayWidth))*qCos(qAtan(double(arrayWidth)/arrayLength));
#endif
        qDebug()<<qRadiansToDegrees(qAtan(double(arrayWidth)/arrayLength))<<" "<<qDegreesToRadians(45.0)<<" "<<qDegreesToRadians(30.0)<<" "<<qDegreesToRadians(15.0);
        qDebug()<<arrayLength45<<" "<<arrayLength75<<" "<<arrayWidth75<<" "<<arrayLength15<<" "<<arrayWidth15;
    }
    Chart_Line(Chart_Line &cr):Chart_Base(cr)
    {
        // chartText.textType1->setAttribute(Qt::WA_StyledBackground,true);
        // chartText.textType1->setAutoFillBackground(true);
        // chartText.textType1->setPalette(QPalette(QPalette::Window, QColor(255,255,255,150)));
        // chartText.textType1->setStyleSheet("QLabel{background:#00FF00;}");
        chartText.textType2->setAttribute(Qt::WA_StyledBackground, true);
        chartText.textType2->setAutoFillBackground(true);
        chartText.textType2->setPalette(QPalette(QPalette::Base, QColor(255, 255, 255, 150)));
        chartText.textType2->setStyleSheet("QPlainTextEdit{background:#00FF00;}");

    }
    Chart_Line( int x1, int y1, int x2, int y2, QWidget *parent = nullptr,PaintChartType type = PaintChartType::LINE) : Chart_Base(x1,y1,x2,y2,parent,type)
    {
        // chartText.textType1->setAttribute(Qt::WA_StyledBackground,true);
        // chartText.textType1->setAutoFillBackground(true);
        // chartText.textType1->setPalette(QPalette(QPalette::Window, QColor(255,255,255,150)));
        // chartText.textType1->setStyleSheet("QLabel{background:#00FF00;}");
        chartText.textType2->setAttribute(Qt::WA_StyledBackground, true);
        chartText.textType2->setAutoFillBackground(true);
        chartText.textType2->setPalette(QPalette(QPalette::Base, QColor(255, 255, 255, 150)));
        chartText.textType2->setStyleSheet("QPlainTextEdit{background:#00FF00;}");
    }
    ~Chart_Line(){}

    //여기 아래는 확인이 필요하다. startChartMag, endChartMag가 어떤 식으로 사용되는지를 확인해야 함.
    //flowChart에서 자석점 이동 및 활용에 사용되는 함수들.
    void setStartChart(Chart_Base* cb){startChartMag = cb;} // 设置起点指向的图形
    void resetStartChart()                                  // 重置起点指向的图形
    {
        if(startChartMag)
        {
            startChartMag->delMagiPointStartLine(startMagIndex,this);
        }
        startChartMag = nullptr;
    }
    Chart_Base *getStartChart(){return startChartMag;}      // 获取起点指向的图形
    void setEndChart(Chart_Base* cb){endChartMag = cb;}     // 设置终点指向的图形
    void resetEndChart()                                    // 重置终点指向的图形
    {
        if(endChartMag)
        {
            endChartMag->delMagiPointEndLine(endMagIndex,this);
        }
        endChartMag = nullptr;
    }
    Chart_Base *getEndChart(){return endChartMag;}          // 获取终点指向的图形
    //여기 위의 함수까지 확인 필요.


    LINE_HEAD_TYPE & getStartLineHeadType(){return startLineHeadType;}              // 获取起点线头类型 @@
    void setStartLineHeadType(LINE_HEAD_TYPE l){startLineHeadType = l;update();}    // 设置起点线头类型 @@
    LINE_HEAD_TYPE & getEndLineHeadType(){return endLineHeadType;}                  // 获取终点线头类型 @@
    void setEndLineHeadType(LINE_HEAD_TYPE l){endLineHeadType = l;update();}        // 设置终点线头类型 @@

    void setStartMagIndex(int i){startMagIndex = i;}        // 设置起点指向的图形的磁力点索引
    int getStartMagIndex(){return startMagIndex;}           // 获取起点指向的图形的磁力点索引
    void setEndMagIndex(int i){endMagIndex = i;}            // 设置终点指向的图形的磁力点索引
    int getEndMagIndex(){return endMagIndex;}               // 获取终点指向的图形的磁力点索引

    void setStartDirect(ORIENTION direct){startDirect = direct;}    // 设置连线起点的朝向 @@
    void resetStartDirect(){startDirect = ORIENTION::NONE;}         // 重置连线起点的朝向 @@
    void setEndDirect(ORIENTION direct){endDirect = direct;}        // 设置连线终点的朝向 @@
    void resetEndDirect(){endDirect = ORIENTION::NONE;}             // 重置连线终点的朝向 @@



    friend QDataStream &operator<<(QDataStream &fout, const Chart_Line &cl);    // 重载输出运算符 @@
    friend QDataStream &operator>>(QDataStream &fin, Chart_Line &cl);           // 重载输入运算符 @@

    //added codes
    void copyLineData(Chart_Base* other);

signals:
    void sendLineStyle(QPen &qp, QBrush &qb, LINE_HEAD_TYPE &startLineHeadType, LINE_HEAD_TYPE &endLineHeadType);   // 发送连线信息 @@
};

#endif // CHART_LINE_H
