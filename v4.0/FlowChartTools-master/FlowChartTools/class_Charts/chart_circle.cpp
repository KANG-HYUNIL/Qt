#include "chart_circle.h"

void Chart_Circle::paintChart(QPainter & p)
{
    QPen tmp = p.pen();
    p.setPen(paintChartDrawPen);

    int sx = paintStart.rx(),sy = paintStart.ry(),ex = paintEnd.rx(),ey = paintEnd.ry();
    int r = ex-sx-ey+sy>0?ey-sy:ex-sx;

    if(graphPath) delete graphPath;
    graphPath = new QPainterPath;
    graphPath->addEllipse(sx,sy,r,r);//矩形左上角与右下角作内切圆

    p.fillPath(*graphPath,paintChartFillPen);
    p.drawPath(*graphPath);

    p.setPen(tmp);
}
