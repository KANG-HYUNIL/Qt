#ifndef CHART_CIRCLE_H
#define CHART_CIRCLE_H
#include "chart_base.h"

class Chart_Circle : public Chart_Base
{
private:
    virtual void paintChart(QPainter & p);

public:
    Chart_Circle(QWidget *parent = nullptr, PaintChartType type = PaintChartType::CIRCLE) : Chart_Base(parent,type,false,true){}
    Chart_Circle(Chart_Circle &cr):Chart_Base(cr){}
    Chart_Circle( int x1, int y1, int x2, int y2, QWidget *parent = nullptr,PaintChartType type = PaintChartType::CIRCLE) : Chart_Base(x1,y1,x2,y2,parent,type){}
    ~Chart_Circle(){}

};

#endif // CHART_CIRCLE_H
