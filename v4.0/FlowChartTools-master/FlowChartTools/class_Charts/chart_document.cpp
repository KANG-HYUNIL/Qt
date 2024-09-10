#include "chart_document.h"

void Chart_Document::paintChart(QPainter & p)
{
    QPen tmp = p.pen();
    p.setPen(paintChartDrawPen);

    int sx = paintStart.rx(),sy = paintStart.ry(),ex = paintEnd.rx(),ey = paintEnd.ry();
    int y1 =(ey-sy)/12,x1 = (ex-sx)/6;

    if(graphPath) delete graphPath;
    graphPath = new QPainterPath;
    graphPath->moveTo(sx,sy);
    graphPath->lineTo(sx,ey);//先到左下角
    graphPath->cubicTo(sx+x1,ey+y1,sx+2*x1,ey+y1,sx+3*x1,ey);//draw the left half of the curve
    graphPath->cubicTo(ex-2*x1,ey-y1,ex-x1,ey-y1,ex,ey);//the rt half
    graphPath->lineTo(ex,sy);
    graphPath->lineTo(ex,sy);
    graphPath->lineTo(sx,sy);

    p.fillPath(*graphPath,paintChartFillPen);
    p.drawPath(*graphPath);

    p.setPen(tmp);
}
