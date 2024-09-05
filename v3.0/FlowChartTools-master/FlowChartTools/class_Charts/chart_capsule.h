#ifndef CHART_CAPSULE_H
#define CHART_CAPSULE_H
#include "chart_base.h"

class Chart_Capsule : public Chart_Base
{
private:
    virtual void paintChart(QPainter & p);

public:
    Chart_Capsule(QWidget *parent = nullptr, PaintChartType type = PaintChartType::CAPSULE) : Chart_Base(parent,type,false,true){}
    Chart_Capsule(Chart_Capsule &cr):Chart_Base(cr){}
    Chart_Capsule( int x1, int y1, int x2, int y2, QWidget *parent = nullptr,PaintChartType type = PaintChartType::CAPSULE) : Chart_Base(x1,y1,x2,y2,parent,type){}
    ~Chart_Capsule(){}

};

#endif // CHART_CAPSULE_H
