#ifndef CHART_SUBROUTINE_H
#define CHART_SUBROUTINE_H
#include "chart_base.h"

class Chart_Subroutine : public Chart_Base
{
private:
    virtual void paintChart(QPainter & p);

public:
    Chart_Subroutine(QWidget *parent = nullptr, PaintChartType type = PaintChartType::SUBROUTINE) : Chart_Base(parent,type,false,true){}
    Chart_Subroutine(Chart_Subroutine &cr):Chart_Base(cr){}
    Chart_Subroutine( int x1, int y1, int x2, int y2, QWidget *parent = nullptr,PaintChartType type = PaintChartType::SUBROUTINE) : Chart_Base(x1,y1,x2,y2,parent,type){}
    ~Chart_Subroutine(){}

};;

#endif // CHART_SUBROUTINE_H
