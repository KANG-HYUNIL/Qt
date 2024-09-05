#include "chart_subroutine.h"

void Chart_Subroutine::paintChart(QPainter & p)
{
    QPen tmp = p.pen();
    p.setPen(paintChartDrawPen);

    int sx = paintStart.rx(),sy = paintStart.ry(),ex = paintEnd.rx(),ey = paintEnd.ry();

    if(graphPath) delete graphPath;
    graphPath = new QPainterPath;
    graphPath->addRect(sx,sy,ex-sx,ey-sy);//先作矩形
    graphPath->moveTo(sx+20,sy);
    graphPath->lineTo(sx+20,ey);
    graphPath->moveTo(ex-20,sy);
    graphPath->lineTo(ex-20,ey);//再画两条边线

    p.fillPath(*graphPath,paintChartFillPen);
    p.drawPath(*graphPath);

    p.setPen(tmp);
}
