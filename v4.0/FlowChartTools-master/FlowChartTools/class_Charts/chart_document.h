#ifndef CHART_DOCUMENT_H
#define CHART_DOCUMENT_H
#include "chart_base.h"

class Chart_Document : public Chart_Base
{
private:
    virtual void paintChart(QPainter & p);

public:
    Chart_Document(QWidget *parent = nullptr, PaintChartType type = PaintChartType::DOCUMENT) : Chart_Base(parent,type,false,true){}
    Chart_Document(Chart_Document &cr):Chart_Base(cr){}
    Chart_Document( int x1, int y1, int x2, int y2, QWidget *parent = nullptr,PaintChartType type = PaintChartType::DOCUMENT) : Chart_Base(x1,y1,x2,y2,parent,type){}
    ~Chart_Document(){}

};

#endif // CHART_DOCUMENT_H
