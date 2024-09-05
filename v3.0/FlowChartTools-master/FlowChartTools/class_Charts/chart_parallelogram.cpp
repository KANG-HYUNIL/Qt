#include "chart_parallelogram.h"

void Chart_Parallelogram::paintChart(QPainter & p)
{
    QPen tmp = p.pen();
    p.setPen(paintChartDrawPen);

    int sx = paintStart.rx(),sy = paintStart.ry(),ex = paintEnd.rx(),ey = paintEnd.ry();//矩形的左上角、右下角

    if(graphPath) delete graphPath;
    graphPath = new QPainterPath;
    graphPath->moveTo(sx+((ey - sy)>>1),sy);
    graphPath->lineTo(ex,sy);
    graphPath->lineTo(ex-((ey - sy)>>1),ey);
    graphPath->lineTo(sx,ey);
    graphPath->lineTo(sx+((ey - sy)>>1),sy);

    p.fillPath(*graphPath,paintChartFillPen);
    p.drawPath(*graphPath);

    p.setPen(tmp);
}
