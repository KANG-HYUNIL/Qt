#include "chart_capsule.h"

void Chart_Capsule::paintChart(QPainter & p)
{
    QPen tmp = p.pen();
    p.setPen(paintChartDrawPen);

    int sx = paintStart.rx(),sy = paintStart.ry(),ex = paintEnd.rx(),ey = paintEnd.ry();
    int a = (ex-sx)/8, b = ey-sy;

    if(graphPath) delete graphPath;
    graphPath = new QPainterPath;
    graphPath->addRect(sx+a,sy,6*a,b);//the middle rect
    graphPath->moveTo(sx+a,sy+b/2);
    graphPath->arcTo(sx,sy,2*a,b,90.0f,180.0f);  //draw counterclockwise,left circle
    graphPath->moveTo(sx+7*a,sy+b/2);
    graphPath->arcTo(sx+6*a,sy,2*a,b,270.0f,180.0f);//right circle

    p.fillPath(graphPath->simplified(),paintChartFillPen);
    p.drawPath(graphPath->simplified());

    p.setPen(tmp);
}
