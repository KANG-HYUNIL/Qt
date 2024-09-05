#ifndef TEXTBOX_H
#define TEXTBOX_H

#include "chart_base.h"

class Textbox
    : public Chart_Base
{
private:
    virtual void paintChart(QPainter & p);
    virtual void updateMagPointInfo();

public:
    Textbox(QWidget *parent = nullptr, PaintChartType type = PaintChartType::TEXT) : Chart_Base(parent,type,false,true,8){}
    Textbox(Textbox &cr):Chart_Base(cr){}
    Textbox( int x1, int y1, int x2, int y2, QWidget *parent = nullptr,PaintChartType type = PaintChartType::TEXT) : Chart_Base(x1,y1,x2,y2,parent,type){}
    ~Textbox(){}


};
#endif // TEXTBOX_H
