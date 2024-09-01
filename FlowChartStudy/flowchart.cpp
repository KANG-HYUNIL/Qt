#include "flowchart.h"



FlowChart::FlowChart(QWidget *parent, Qt::WindowFlags f) : QWidget(parent,f)
{
    this->grabKeyboard();
    //    installEventFilter(this);
    initVar();
    setMouseTracking(true);
    update();
}

void FlowChart::initVar()
{
    mouseEventType = MOUSE_EVENT_TYPE::NONE;
    curPaintChart = nullptr;
    curSelectChart = nullptr;
    newLineChart = nullptr;
    newLineFromSelectChart = nullptr;
    newLineToSelectChart = nullptr;
}

void FlowChart::resetFlowChartPanel()
{

}

//file methods
void FlowChart::setFileSetSaved(bool isSaved)
{

}


void FlowChart::setFileNameShow(bool isSaved)
{

}


bool FlowChart::openChartFile()
{

}

bool FlowChart::saveChartFile()
{

}

bool FlowChart::newChartFile()
{

}

bool FlowChart::saveFile(QString filePath)
{

}


bool FlowChart::loadFile(QString filePath)
{

}




Chart_parent * FlowChart::getCurPaintChart()
{
    return curPaintChart;
}

void FlowChart::setSelChartLineColor(const QColor &color)
{
    curSelectChart->paintChartDrawPen.setColor(color);
    curSelectChart->update();
}

void FlowChart::setSelChartFillColor(const QColor &color)
{
    curSelectChart->paintChartFillPen.setColor(color);
    curSelectChart->update();
}


void FlowChart::setMousePressedFlag(MOUSE_EVENT_TYPE f)
{
    mouseEventType = f;
}

//set the shape will draw
void FlowChart::setPaintChart()
{
    if(curPaintChart != nullptr)
        delete curPaintChart;

    switch(curPaintChartType)
    {

    case PaintChartType::LINE:
    {
        curPaintChart = new Chart_Line(this);
    }break;

    default:case PaintChartType::NONE:{
        curPaintChart = nullptr;
    }break;

    }

    if(curPaintChart)
    {
        mouseEventType = MOUSE_EVENT_TYPE::CREATING_CHANGE_SIZE;
    }
}



void FlowChart::setSelectChart(Chart_parent * cb, int x, int y)
{

    emit disableStyle();
    this->grabKeyboard();
    if(curSelectChart != nullptr) curSelectChart->hideMagSize();
    curSelectChart = cb;
    curSelectChart->showMagSize();
    curSelectChartPos = QPoint(x,y);
    mouseEventType = MOUSE_EVENT_TYPE::RUNTIME_CHANGE_POS;
    Chart_Line *cl = nullptr;

    if(nullptr == (cl = dynamic_cast<Chart_Line*>(cb)))
    {
        emit sendChartStyle(cb->paintChartDrawPen,cb->paintChartFillPen);
    }else
    {
        emit sendLineStyle(cl->paintChartDrawPen,cl->getStartLineHeadType(),cl->getEndLineHeadType());
    }
}



Chart_parent * FlowChart::getCurSelectChart()
{
    return curSelectChart;
}



void FlowChart::addChart(Chart_parent *cb)
{
    charts.push_back(cb);
}



bool FlowChart::delChart(Chart_parent *&cb)
{
    for(auto it = charts.begin();it != charts.end();it++)
    {
        if(*it == cb)
        {
            Chart_parent *tmp = cb;
            cb = nullptr;
            charts.erase(it);
#if 1
            for(auto magit = tmp->magPoint.i_point.begin();magit!=tmp->magPoint.i_point.end();magit++)
            {
                for(auto magLineStIt = (*magit)->i_lineStart.begin();magLineStIt != (*magit)->i_lineStart.end();magLineStIt++)
                {
                    dynamic_cast<Chart_Line*>(*magLineStIt) ->resetEndChart();
                    if(!delLine(*magLineStIt))
                    {
                        qDebug()<<"Error";
                    }else{
                        qDebug()<<"Succeed.";
                    }
                }
                for(auto magLineEnIt = (*magit)->i_lineEnd.begin();magLineEnIt != (*magit)->i_lineEnd.end();magLineEnIt++)
                {
                    dynamic_cast<Chart_Line*>(*magLineEnIt) ->resetStartChart();
                    if(!delLine(*magLineEnIt))
                    {
                        qDebug()<<"Error";
                    }else{
                        qDebug()<<"Succeed.";
                    }
                }
            }
#endif
            delete[] tmp;

            return true;
        }
    }
    return false;
}


void FlowChart::addLine(Chart_parent *cb)
{
    line.push_back(cb);
}


bool FlowChart::delLine(Chart_parent *&cb)
{
    for(auto it = line.begin();it != line.end();it++)
    {
        if(*it == cb)
        {
            Chart_Line *tmp = dynamic_cast<Chart_Line*>(*it);
            cb = nullptr;
            line.erase(it);
            qDebug()<<tmp->magPoint.i_point.size();


            tmp->resetEndChart();
            tmp->resetStartChart();
            delete tmp;
            return true;
        }
    }
    return false;
}


void FlowChart::hideMagSizeAll()
{
    for(auto it = charts.begin();it!=charts.end();it++)
    {
        (*it)->hideMagSize();
    }
    for(auto it = line.begin();it!=line.end();it++)
    {
        (*it)->hideMagSize();
    }
    curSelectChart = nullptr;
    mouseEventType = MOUSE_EVENT_TYPE::NONE;
    emit disableStyle();
}


//events

void FlowChart::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setPen(QColor(150,150,150));
    const int gap = 30;
    for(int i = 1;i * gap < width();i++)
    {
        if(i%5 == 0)
            p.drawLine(QLineF(i*gap,0,i*gap,height()));
    }
    for(int i = 1;i * gap < height();i++)
    {
        if(i%5 == 0)
            p.drawLine(QLineF(0,i*gap,width(),i*gap));
    }

    p.setPen(QColor(200,200,200,150));
    //p.setPen(Qt::DotLine);
    //p.setOpacity(200);
    for(int i = 1;i * gap < width();i++)
    {
        if(i%5 == 0) continue;
        p.drawLine(QLineF(i*gap,0,i*gap,height()));
    }
    for(int i = 1;i * gap < height();i++)
    {
        if(i%5 == 0) continue;
        p.drawLine(QLineF(0,i*gap,width(),i*gap));
    }


    QStyleOption opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);

    //event->accept();
}


void FlowChart::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
    switch(mouseEventType)
    {
    case MOUSE_EVENT_TYPE::CREATING_CHANGE_SIZE:{
        int x = event->pos().rx();
        int y = event->pos().ry();
        if(curSelectChart) curSelectChart->hideMagSize();
        curSelectChart = curPaintChart;
        curPaintChart->setXY(x,y);
        if(curPaintChart->chartType == PaintChartType::LINE)
        {
            connect(curPaintChart,SIGNAL(sendThisClass(Chart_Base *, int,int)),this,SLOT(setSelecChart(Chart_Base *, int,int)));
            connect(curPaintChart,SIGNAL(setTypeChangeSize(ORIENTION)),this,SLOT(setTypeChangeSize(ORIENTION)));
            addLine(curPaintChart);
            Chart_Line *cl = dynamic_cast<Chart_Line*>(curSelectChart);
            emit sendLineStyle(cl->paintChartDrawPen,cl->getStartLineHeadType(),cl->getEndLineHeadType());
        }else
        {
            connect(curPaintChart,SIGNAL(sendThisClass(Chart_Base *, int,int)),this,SLOT(setSelecChart(Chart_Base *, int,int)));
            connect(curPaintChart,SIGNAL(setTypeChangeSize(ORIENTION)),this,SLOT(setTypeChangeSize(ORIENTION)));
            connect(curPaintChart,SIGNAL(setTypeCreateMagPoint(Chart_Base *,ORIENTION,int)),this,SLOT(setTypeCreateMagPoint(Chart_Base *,ORIENTION,int)));
            addChart(curPaintChart);
            emit sendChartStyle(curSelectChart->paintChartDrawPen,curSelectChart->paintChartFillPen);
        }
        curPaintChart->update();
        curPaintChart->show();

        event->accept();
    }break;
    default:case MOUSE_EVENT_TYPE::NONE:{
        for(auto it = charts.begin();it!=charts.end();it++)
        {
            (*it)->overlapChartMousePressed(event);
            if(event->isAccepted()) return;
        }
        for(auto a:line)
        {
            a->overlapChartMousePressed(event);
            if(event->isAccepted()) return;
        }
        hideMagSizeAll();
    }break;
    }
}



void FlowChart::mouseMoveEvent(QMouseEvent *event)
{

    event->ignore();
    switch(mouseEventType)
    {
    default:case MOUSE_EVENT_TYPE::NONE:
    {
        for(auto it = charts.begin();it!=charts.end();it++)
        {
            (*it)->overlapChartMouseMove(event);
            if(event->isAccepted()) return;
        }
        for(auto it = line.begin();it!=line.end();it++)
        {
            (*it)->overlapChartMouseMove(event);
            if(event->isAccepted()) return;
        }
        if(curSelectChart == nullptr)
        {
            setCursor(QCursor(Qt::ArrowCursor));
            hideMagSizeAll();
        }
    }break;
    case MOUSE_EVENT_TYPE::CREATING_CHANGE_SIZE:
    {
        int x = event->pos().rx(),y = event->pos().ry();
        if(curSelectChart)
        {
            if(curSelectChart->chartType == PaintChartType::LINE)
            {
                for(auto it = charts.begin();it!=charts.end();it++)
                {
                    if((*it)->autoSetMagi(x,y,magPointIndex))
                    {
                        lineSelectChart = *it;
                        break;
                    }else{
                        lineSelectChart = nullptr;
                    }
                }
            }
            curPaintChart->setWidthHeight(x,y);
        }
        event->accept();
    }break;
    case MOUSE_EVENT_TYPE::RUNTIME_CHANGE_SIZE:
    {
        int x = event->pos().rx(),y = event->pos().ry();

        if(curSelectChart->chartType == PaintChartType::LINE)
        {
            qDebug()<<"hehe";
            for(auto it = charts.begin();it!=charts.end();it++)
            {
                if((*it)->autoSetMagi(x,y,magPointIndex))
                {
                    lineSelectChart = *it;
                    break;
                }else{
                    lineSelectChart = nullptr;
                }
            }
        }
        curSelectChart->setWidthHeight(x,y,sizePointDirect);
        event->accept();
    }break;
    case MOUSE_EVENT_TYPE::RUNTIME_CREATE_MAGPOINT:
    {
        if(newLineChart == nullptr)
        {
            newLineChart = new Chart_Line(this);
            if(curSelectChart)
                curSelectChart->hideMagSize();
            curSelectChart = newLineChart;
            addLine(newLineChart);
            connect(newLineChart,SIGNAL(sendThisClass(Chart_Base *, int,int)),this,SLOT(setSelecChart(Chart_Base *, int,int)));
            connect(newLineChart,SIGNAL(setTypeChangeSize(ORIENTION)),this,SLOT(setTypeChangeSize(ORIENTION)));
            newLineChart->setXY(newLineFromSelectChart->getMagiPointAbsX(magPointFromIndex),newLineFromSelectChart->getMagiPointAbsY(magPointFromIndex));
            newLineChart->setStartChart(newLineFromSelectChart);
            newLineChart->setStartMagIndex(magPointFromIndex);
            newLineChart->setStartDirect(magPointDirect);
            newLineChart->update();
            newLineChart->show();

            newLineFromSelectChart->addMagiPointStartLine(magPointFromIndex,newLineChart);
        }
        int x = event->pos().rx(),y = event->pos().ry();

        for(auto it = charts.begin();it!=charts.end();it++)
        {

            if((*it)->autoSetMagi(x,y,magPointToIndex))
            {
                newLineToSelectChart = *it;
                break;
            }else{
                newLineToSelectChart = nullptr;
            }
        }

        newLineChart->setWidthHeight(x,y,ORIENTION::ENDPOINT);
        event->ignore();
    }break;
    case MOUSE_EVENT_TYPE::RUNTIME_CHANGE_POS:
    {
        if(curSelectChart->chartType != PaintChartType::LINE)
        {
            curSelectChart->setXY(event->pos().rx() - curSelectChartPos.rx(),event->pos().ry() - curSelectChartPos.ry());
            event->accept();
        }
    }break;
    }
}


void FlowChart::mouseReleaseEvent(QMouseEvent *event)
{
    switch(mouseEventType)
    {
    case MOUSE_EVENT_TYPE::CREATING_CHANGE_SIZE:
    {
        //curPaintChart->updatePath();
        resetPaintChartType();
        if(curSelectChart->chartType == PaintChartType::LINE)
        {
            Chart_Line *cl = reinterpret_cast<Chart_Line *>(curSelectChart);

            if(lineSelectChart)
            {
                lineSelectChart->addMagiPointEndLine(magPointIndex,curSelectChart);
                lineSelectChart->hideMagOnly();
                cl->setEndChart(lineSelectChart);
                cl->setEndMagIndex(magPointIndex);
                cl->setEndDirect(lineSelectChart->getMagiPointDirect(magPointIndex));
                cl->update();
                lineSelectChart = nullptr;
            }
            else{
                cl->resetEndChart();
            }

        }


        curPaintChart = nullptr;
        mouseEventType = MOUSE_EVENT_TYPE::NONE;
        setFileSetSaved(false);
        event->accept();
    }break;
    case MOUSE_EVENT_TYPE::RUNTIME_CHANGE_SIZE:
    {
        if(curSelectChart->chartType == PaintChartType::LINE)
        {
            Chart_Line *cl = reinterpret_cast<Chart_Line *>(curSelectChart);
            if(lineSelectChart)
            {
                if(sizePointDirect == ORIENTION::STARTPOINT)
                {
                    cl->resetStartChart();
                    lineSelectChart->addMagiPointStartLine(magPointIndex,curSelectChart);
                    lineSelectChart->hideMagOnly();
                    cl->setStartChart(lineSelectChart);
                    cl->setStartMagIndex(magPointIndex);
                    cl->setStartDirect(lineSelectChart->getMagiPointDirect(magPointIndex));
                }else if(sizePointDirect == ORIENTION::ENDPOINT)
                {
                    cl->resetEndChart();
                    lineSelectChart->addMagiPointEndLine(magPointIndex,curSelectChart);
                    lineSelectChart->hideMagOnly();
                    cl->setEndChart(lineSelectChart);
                    cl->setEndMagIndex(magPointIndex);
                    cl->setEndDirect(lineSelectChart->getMagiPointDirect(magPointIndex));
                }else{
                    qDebug()<<"Error";
                }

                cl->update();
            }else
            {
                if(sizePointDirect == ORIENTION::STARTPOINT)
                {
                    cl->resetStartChart();
                }else if(sizePointDirect == ORIENTION::ENDPOINT)
                {
                    cl->resetEndChart();
                }else
                {
                    qDebug()<<"Error";
                }
            }
        }
        mouseEventType = MOUSE_EVENT_TYPE::NONE;
        lineSelectChart = nullptr;
        setFileSetSaved(false);
        event->accept();
    }break;
    case MOUSE_EVENT_TYPE::RUNTIME_CREATE_MAGPOINT:
    {
        if(newLineToSelectChart)
        {
            newLineToSelectChart->addMagiPointEndLine(magPointToIndex,newLineChart);
            newLineToSelectChart->hideMagOnly();
            newLineChart->setEndChart(newLineToSelectChart);
            newLineChart->setEndMagIndex(magPointToIndex);
            newLineChart->setEndDirect(newLineToSelectChart->getMagiPointDirect(magPointToIndex));
            newLineChart->update();
        }else
        {
            if(newLineChart)
                newLineChart->resetEndChart();
        }
        mouseEventType = MOUSE_EVENT_TYPE::NONE;
        newLineChart = nullptr;
        newLineFromSelectChart = nullptr;
        newLineToSelectChart = nullptr;
        setFileSetSaved(false);
        event->accept();
    }break;
    case MOUSE_EVENT_TYPE::RUNTIME_CHANGE_POS:
    {
        mouseEventType = MOUSE_EVENT_TYPE::NONE;
        setFileSetSaved(false);
        event->accept();
    }break;
    default:case MOUSE_EVENT_TYPE::NONE:
    {
        event->ignore();
    }break;
    }

}


void FlowChart::keyPressEvent(QKeyEvent *ev)
{
    ev->ignore();
    switch(ev->key())
    {
    case Qt::Key_Escape:
    {
        if(curSelectChart)
        {
            curSelectChart->hideMagSize();
            curSelectChart = nullptr;
        }
    }break;
    case Qt::Key_Delete:
    {
        if(curSelectChart)
        {
            if(curSelectChart->chartType == PaintChartType::LINE)
            {
                if(!delLine(curSelectChart))
                {
                    qDebug()<<"Error";
                }
            }else{
                if(!delChart(curSelectChart))
                {
                    qDebug()<<"Error";
                }
            }
            curSelectChart = nullptr;
        }
    }break;

#if 0
        case Qt::Key_Q:
        {
            qDebug()<<"总个数：charts:"<<charts.size()<<",lines:"<<line.size();
            for(auto it = charts.begin();it != charts.end();it++)
            {
                qDebug()<<"\t磁力点个数："<<(*it)->magPoint.i_point.size();
                for(auto magit = (*it)->magPoint.i_point.begin();magit!=(*it)->magPoint.i_point.end();magit++)
                {
                    qDebug()<<"\t\t线头个数："<<(*magit)->i_lineStart.size()<<"，线尾个数："<<(*magit)->i_lineEnd.size();
                    for(auto magLineStIt = (*magit)->i_lineStart.begin();magLineStIt != (*magit)->i_lineStart.end();magLineStIt++)
                    {
                        qDebug()<<"\t\t\t线头ID："<<(*magLineStIt)->getID();
                    }
                    for(auto magLineEnIt = (*magit)->i_lineEnd.begin();magLineEnIt != (*magit)->i_lineEnd.end();magLineEnIt++)
                    {
                        qDebug()<<"\t\t\t线尾ID："<<(*magLineEnIt)->getID();
                    }
                }
            }
            qDebug();
            for(auto it = line.begin();it != line.end();it++)
            {
                qDebug()<<"\t线头："<<((dynamic_cast<Chart_Line*>(*it)->getStartChart() == nullptr)?"-":"有")<<"，线尾："<<(((dynamic_cast<Chart_Line*>(*it)->getEndChart()) == nullptr)?"-":"有");
            }
        }break;
#endif

    default:{
        ev->ignore();
    }
    }
}



void FlowChart::keyReleaseEvent(QKeyEvent *ev)
{
    ev->ignore();
}


