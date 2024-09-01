#include "chart_base.h"


int Chart_Base::sizePointWidth = 9;
int Chart_Base::sizePointWidth1_2 = sizePointWidth/2;
int Chart_Base::chartIDCount = 0;



//constructors
Chart_Base::Chart_Base(QWidget *parent, PaintChartType type,  bool mov, int spc):QWidget(parent)
    ,sizePoint(spc)
    ,movable(mov)
{
    chartType = type;
    widgetPosInit();
    varInit();
    paintInit();
    pointInit();
    setMouseTracking(true);
}

Chart_Base::Chart_Base( int x, int y, int w, int h, QWidget *parent, PaintChartType type):QWidget(parent)
    ,sizePoint(4)
{
    chartType = type;
    widgetPosInit(x,y,w,h);
    varInit();
    paintInit();
    pointInit();
    setMouseTracking(true);
}


Chart_Base::Chart_Base(Chart_Base &cb)
{
    chartType = cb.chartType;
    paintStart = cb.paintStart;
    paintEnd = cb.paintEnd;
    sizePoint = cb.sizePoint;

    varInit();
    paintInit();
    pointInit();
    setMouseTracking(true);
}

//init methods
void Chart_Base::widgetPosInit(int x,int y,int w,int h)
{
    widgetStart.setX(x);
    widgetStart.setY(y);
    widgetEnd.setX(x+w);
    widgetEnd.setY(y+h);
    updateWidgetPosInfo();
}

void Chart_Base::varInit(int spw)
{
    qDebug()<<chartIDCount;
    ID = chartIDCount++;
    Chart_Base::sizePointWidth = spw;
    updatePaddingInfo();
    qDebug()<<"Draw";
}

void Chart_Base::paintInit()
{
    updatePaintInfo();
}


void Chart_Base::pointInit()
{

    for(auto it = sizePoint.i_point.begin();it != sizePoint.i_point.end(); it++)
    {
        (*it) = new i_pointbase;
        (*it)->i_pos = new QPoint;
    }

    updateSizePointInfo();
    updateSizePointPath();
}



//update ~~ info methods
void Chart_Base::updatePaddingInfo()
{
    sizePointWidth1_2 = sizePointWidth>>1;
}


void Chart_Base::updateWidgetPosInfo()
{

    int w;;
    int h;
    int x,y;
    if(widgetEnd.rx() < widgetStart.rx())
    {
        x = widgetEnd.rx();
        w = widgetStart.rx() - widgetEnd.rx();
    }
    else
    {
        x = widgetStart.rx();
        w = widgetEnd.rx() - widgetStart.rx();
    }
    if(widgetEnd.ry() < widgetStart.ry())
    {
        y = widgetEnd.ry();
        h = widgetStart.ry() - widgetEnd.ry();
    }
    else
    {
        y = widgetStart.ry();
        h = widgetEnd.ry() - widgetStart.ry();
    }

    setGeometry(x,y,w,h);

}






