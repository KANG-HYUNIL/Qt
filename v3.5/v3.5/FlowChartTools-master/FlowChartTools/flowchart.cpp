#include "flowchart.h"
#include "QWheelEvent"


FlowChart::FlowChart(QWidget *parent, Qt::WindowFlags f) : QWidget(parent,f), undoStack(new QUndoStack(this))
{
    //added codes
    QShortcut *copyShortcut = new QShortcut(QKeySequence("Ctrl+C"), this);
    connect(copyShortcut, &QShortcut::activated, this, [this]() {
        qDebug() << "Ctrl + C Pressed via QShortcut";


        if (!curSelecChart) {
            qDebug() << "curSelecChart is nullprt";
            return;
        }



        if (copiedChart)
        {
            bool checkIsPasted = false;

            for(auto it = charts.begin();it != charts.end();it++)
            {

                if (*it == copiedChart)
                {
                    checkIsPasted = true;
                    break;
                }
            }

            for(auto it = line.begin();it != line.end();it++)
            {

                if (*it == copiedChart)
                {
                    checkIsPasted = true;
                    break;
                }
            }

            qDebug() << "checkIsPasted : " + checkIsPasted;

            if (checkIsPasted)
            {
                copiedChart = nullptr;
            }

            else
            {

                if (copiedChart->chartType == PaintChartType::LINE)
                {
                    Chart_Line *tmp = dynamic_cast<Chart_Line*>(copiedChart);
                    tmp->resetEndChart();
                    tmp->resetStartChart();
                    delete tmp;

                }

                else
                {
                    Chart_Base *tmp = copiedChart;
                    copiedChart = nullptr;

                    for(auto magit = tmp->magPoint.i_point.begin();magit!=tmp->magPoint.i_point.end();magit++)
                    {
                        for(auto magLineStIt = (*magit)->i_lineStart.begin();magLineStIt != (*magit)->i_lineStart.end();magLineStIt++)
                        {
                            dynamic_cast<Chart_Line*>(*magLineStIt) ->resetEndChart();
                            if(!delLine(*magLineStIt))
                            {
                                qDebug()<<"Error";
                            }

                            else
                            {
                                qDebug()<<"Succeed.";
                            }
                        }

                        for(auto magLineEnIt = (*magit)->i_lineEnd.begin();magLineEnIt != (*magit)->i_lineEnd.end();magLineEnIt++)
                        {
                            dynamic_cast<Chart_Line*>(*magLineEnIt) ->resetStartChart();
                            if(!delLine(*magLineEnIt))
                            {
                                qDebug()<<"Error";
                            }

                            else
                            {
                                qDebug()<<"Succeed.";
                            }
                        }
                    }
                }

                copiedChart = nullptr;

            }

        }

        // Ctrl + C
        qDebug() << "Ctrl + C Pressed";
        switch(curSelecChart->chartType)
        {
        case PaintChartType::RECT:
        {
            copiedChart = new Chart_Rect(this);
        }break;
        case PaintChartType::DIAMOND:
        {
            copiedChart = new Chart_Diamond(this);
        }break;
        case PaintChartType::ROUNDRECT:
        {
            copiedChart = new Chart_RoundRect(this);
        }break;
        case PaintChartType::ELLIPSE:
        {
            copiedChart = new Chart_Ellipse(this);
        }break;
        case PaintChartType::LINE:
        {
            copiedChart = new Chart_Line(this);
        }break;
        case PaintChartType::TRAPEZOID:
        {
            copiedChart = new Chart_Trapezoid(this);
        }break;
        case PaintChartType::CAPSULE:
        {
            copiedChart = new Chart_Capsule(this);
        }break;
        case PaintChartType::CIRCLE:
        {
            copiedChart = new Chart_Circle(this);
        }break;
        case PaintChartType::DOCUMENT:
        {
            copiedChart = new Chart_Document(this);
        }break;
        case PaintChartType::PARALLELOGRAM:
        {
            copiedChart = new Chart_Parallelogram(this);
        }break;
        case PaintChartType::SUBROUTINE:
        {
            copiedChart = new Chart_Subroutine(this);
        }break;
        case PaintChartType::TEXT:
        {
            copiedChart = new Textbox(this);
        }break;
        default:case PaintChartType::NONE:{
            copiedChart = nullptr;
        }break;

        }

        if(copiedChart)
        {
            //
            copiedChart->copyData(curSelecChart);
            qDebug() << "copyData clear";


            copyMagPointData(copiedChart, curSelecChart);
            qDebug() << "copyMagPointData clear??";


            if (copiedChart->chartType == PaintChartType::LINE)
            {
                Chart_Line* copiedLine = dynamic_cast<Chart_Line*>(copiedChart);
                Chart_Line* selecLine = dynamic_cast<Chart_Line*>(curSelecChart);
                copiedLine->copyLineData(curSelecChart);
                if (selecLine->startChartMag){
                    copiedLine->startChartMag->copyData(selecLine->startChartMag);
                }
                else
                {
                    copiedLine->startChartMag = nullptr;
                }

                if (selecLine->endChartMag)
                {
                    copiedLine->endChartMag->copyData(selecLine->endChartMag);
                }
                else
                {
                    copiedLine->endChartMag = nullptr;
                }

                copiedChart = copiedLine;
            }

            qDebug() << "Deep Copied??";
        }
    });

    QShortcut *pasteShortcut = new QShortcut(QKeySequence("Ctrl+V"), this);
    connect(pasteShortcut, &QShortcut::activated, this, [this]() {
        qDebug() << "Ctrl + V Pressed via QShortcut";

        // Ctrl + V
        qDebug() << "Ctrl + V Pressed";

        if (copiedChart)
        {

            QPointF mousePos = this->mapFromGlobal(QCursor::pos());
            copiedChart->setXY(mousePos.x(), mousePos.y());

            qDebug() << "Mouse Position Check";

            if(copiedChart) copiedChart->hideMagSize();

            qDebug() << "hideMagSize method";

            if(copiedChart->chartType == PaintChartType::LINE)
            {
                connect(copiedChart,SIGNAL(sendThisClass(Chart_Base *, int,int)),this,SLOT(setSelecChart(Chart_Base *, int,int)));
                connect(copiedChart,SIGNAL(setTypeChangeSize(ORIENTION)),this,SLOT(setTypeChangeSize(ORIENTION)));
                //added codes
                connect(copiedChart, SIGNAL(deleteThisChartSignal(Chart_Base*)), this, SLOT(deleteSelectChart(Chart_Base*)));

                qDebug() << "connect line method";

                addLine(copiedChart);

                qDebug() << "dynamic cast";

                Chart_Line *cl = dynamic_cast<Chart_Line*>(copiedChart);
                emit sendLineStyle(cl->paintChartDrawPen,cl->getStartLineHeadType(),cl->getEndLineHeadType());

                qDebug() << "Line connect method";

            }

            else
            {
                connect(copiedChart,SIGNAL(sendThisClass(Chart_Base *, int,int)),this,SLOT(setSelecChart(Chart_Base *, int,int)));
                connect(copiedChart,SIGNAL(setTypeChangeSize(ORIENTION)),this,SLOT(setTypeChangeSize(ORIENTION)));
                connect(copiedChart,SIGNAL(setTypeCreateMagPoint(Chart_Base *,ORIENTION,int)),this,SLOT(setTypeCreateMagPoint(Chart_Base *,ORIENTION,int)));
                //connect(curPaintChart,SIGNAL(hideThisClass()),this,SLOT(resetSeletcChart()));
                //added codes
                connect(copiedChart, SIGNAL(deleteThisChartSignal(Chart_Base*)), this, SLOT(deleteSelectChart(Chart_Base*)));

                qDebug() << "connect chart method";

                addChart(copiedChart);
                emit sendChartStyle(copiedChart->paintChartDrawPen,copiedChart->paintChartFillPen);

                qDebug() << "Chart connect method";
            }

            copiedChart->update();
            copiedChart->show();
            //curSelecChart = copiedChart;
            copiedChart = nullptr;

            qDebug() << "Pasted?";
        }
    });

    initVar();
    setMouseTracking(true);
    update();
}

void FlowChart::initVar()
{
    mouseEventType = MOUSE_EVENT_TYPE::NONE;
    curPaintChart = nullptr;
    curSelecChart = nullptr;
    newLineChart = nullptr;
    newLineFromSelectChart = nullptr;
    newLineToSelectChart = nullptr;
}

void FlowChart::resetFlowChartPanel()
{
    Chart_Base::resetStaticVal();
    curPaintChart = nullptr;
    curPaintChartType = PaintChartType::NONE;
    curSelecChart = nullptr;
    curSelecChartPos = QPoint();
    newLineChart = nullptr;
    newLineToSelectChart = nullptr;
    newLineFromSelectChart = nullptr;
    magPointDirect = ORIENTION::NONE;
    magPointFromIndex = 0;
    magPointToIndex = 0;
    sizePointDirect = ORIENTION::NONE;
    clearChartsLine();
    charts.reserve(0);
    charts.resize(0);
    line.reserve(0);
    line.resize(0);
    mouseEventType = MOUSE_EVENT_TYPE::NONE;
    fileIsOpened = false;
    fileIsSaved = true;
    filePath = "";
    setFileNameShow(fileIsSaved);
}

void FlowChart::setFileSetSaved(bool isSaved)
{
    if(fileIsSaved != isSaved)
    {
        fileIsSaved = isSaved;
        setFileNameShow(fileIsSaved);
    }
}

void FlowChart::setFileNameShow(bool isSaved)
{
    QString tmp;
    if(fileIsOpened)
    {
        tmp = QString("%1%2").arg(filePath).arg((isSaved?"":" *"));
        emit updateWindowTitle(tmp);
    }else
    {
        tmp = QString("%1%2").arg("Untitle").arg((isSaved?"":" *"));
        emit updateWindowTitle(tmp);
    }
}

Chart_Base * FlowChart::getCurPaintChart()
{
    return curPaintChart;
}

void FlowChart::setSelChartLineColor(const QColor &color)
{
    curSelecChart->paintChartDrawPen.setColor(color);
    curSelecChart->update();
}

void FlowChart::setSelChartFillColor(const QColor &color)
{
    curSelecChart->paintChartFillPen.setColor(color);
    curSelecChart->update();
}

void FlowChart::setMousePressedFlag(MOUSE_EVENT_TYPE f)
{
    mouseEventType = f;
}

void FlowChart::setPaintChart()
{
    if(curPaintChart != nullptr)
        delete curPaintChart;

    switch(curPaintChartType)
    {
        case PaintChartType::RECT:
        {
            curPaintChart = new Chart_Rect(this);
        }break;
        case PaintChartType::DIAMOND:
        {
            curPaintChart = new Chart_Diamond(this);
        }break;
        case PaintChartType::ROUNDRECT:
        {
            curPaintChart = new Chart_RoundRect(this);
        }break;
        case PaintChartType::ELLIPSE:
        {
            curPaintChart = new Chart_Ellipse(this);
        }break;
        case PaintChartType::LINE:
        {
            curPaintChart = new Chart_Line(this);
        }break;
        case PaintChartType::TRAPEZOID:
        {
            curPaintChart = new Chart_Trapezoid(this);
        }break;
        case PaintChartType::CAPSULE:
        {
            curPaintChart = new Chart_Capsule(this);
        }break;
        case PaintChartType::CIRCLE:
        {
            curPaintChart = new Chart_Circle(this);
        }break;
        case PaintChartType::DOCUMENT:
        {
            curPaintChart = new Chart_Document(this);
        }break;
        case PaintChartType::PARALLELOGRAM:
        {
            curPaintChart = new Chart_Parallelogram(this);
        }break;
        case PaintChartType::SUBROUTINE:
        {
            curPaintChart = new Chart_Subroutine(this);
        }break;
        case PaintChartType::TEXT:
        {
            curPaintChart = new Textbox(this);
        }break;
        default:case PaintChartType::NONE:{
            curPaintChart = nullptr;
        }break;
    }
    if(curPaintChart)
    {
        mouseEventType = MOUSE_EVENT_TYPE::CREATING_CNANGE_SIZE;
    }
}

void FlowChart::setSelecChart(Chart_Base * cb, int x, int y)
{

    emit disableStyle();
    //this->grabKeyboard();
    if(curSelecChart != nullptr) curSelecChart->hideMagSize();
    curSelecChart = cb;
    curSelecChart->showMagSize();
    curSelecChartPos = QPoint(x,y);
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

bool FlowChart::openChartFile()
{
    undoStack->clear();
    if(!fileIsSaved)
    {
        QMessageBox tmp(QMessageBox::Warning,tr("警告！"),tr("修改未保存"),QMessageBox::NoButton,this->parentWidget());
        QPushButton *saveclose = tmp.addButton(tr("保存并关闭"),QMessageBox::ActionRole);
        QPushButton *nosaveclose = tmp.addButton(tr("不关闭"),QMessageBox::AcceptRole);
        QPushButton *cancel = tmp.addButton(tr("取消"),QMessageBox::RejectRole);
        tmp.exec();
        if(tmp.clickedButton() == saveclose)
        {
            if(saveChartFile())
            {
                resetFlowChartPanel();
            }else
                return false;
            //qDebug()<<"Save and Quit";
        }else if(tmp.clickedButton() == nosaveclose)
        {
            //qDebug()<<"Not Save and Quit";
            resetFlowChartPanel();
        }else if(tmp.clickedButton() == cancel)
        {
            return true;
            //qDebug()<<"cancel ";
        }
    }
    resetFlowChartPanel();
    QString tmpFilePath = QFileDialog::getOpenFileName(this,tr("打开文件"),"F:",tr("FCT文件(*.fct)"));
    if(tmpFilePath == "") return false;

    if(loadFile(tmpFilePath) == false)
    {
        QMessageBox::critical(this->parentWidget(),tr("错误！"),tr("打开文件失败！\n文件损坏或类型不正确"),QMessageBox::Ok);
        return false;
    }else
    {
        fileIsOpened = true;
        this->filePath = tmpFilePath;
        setFileNameShow(fileIsSaved);
        qDebug()<<"Load File Success"<<fileIsSaved;
    }
    qDebug()<<"charts size:"<<charts.size()<<" line size::"<<line.size();
    return true;
}

bool FlowChart::saveChartFile()
{
    if(fileIsSaved)
    {
        return true;
    }else
    {
        if(fileIsOpened)
        {
            if(saveFile(filePath))
            {
                filePath = "";
            }else
            {
                QMessageBox::critical(this->parentWidget(),tr("错误！"),tr("保存文件失败！\n文件已占用或者访问权限不足"),QMessageBox::Ok);
                return false;
            }
        }else{
            QString tmpFilePath = QFileDialog::getSaveFileName(this,tr("保存文件"),"F:",tr("FCT文件(*.fct)"));
            if(tmpFilePath == "") return false;
            if(saveFile(tmpFilePath))
            {
                filePath = tmpFilePath;
                fileIsOpened = true;
            }else
            {
                QMessageBox::critical(this->parentWidget(),tr("错误！"),tr("保存文件失败！\n文件已占用或者访问权限不足"),QMessageBox::Ok);
                return false;
            }
        }
        setFileSetSaved(true);
    }
    return true;
}
bool FlowChart::newChartFile()
{
    undoStack->clear();
    if(!fileIsSaved)
    {
        QMessageBox tmp(QMessageBox::Warning,tr("警告！"),tr("修改未保存"),QMessageBox::NoButton,this->parentWidget());
        QPushButton *saveclose = tmp.addButton(tr("保存并关闭"),QMessageBox::ActionRole);
        QPushButton *nosaveclose = tmp.addButton(tr("关闭"),QMessageBox::AcceptRole);
        QPushButton *cancel = tmp.addButton(tr("取消"),QMessageBox::RejectRole);
        tmp.exec();
        if(tmp.clickedButton() == saveclose)
        {
            if(saveChartFile())
            {
                resetFlowChartPanel();
            }else{
                return false;
            }
            //qDebug()<<"Save and Quit";

        }else if(tmp.clickedButton() == nosaveclose)
        {
            //qDebug()<<"Not Save and Quit";
            resetFlowChartPanel();
        }else if(tmp.clickedButton() == cancel)
        {
            //qDebug()<<"cancel ";
        }
    }else{
        resetFlowChartPanel();
    }
    return true;
}

//void FlowChart::resetSeletcChart()
//{
//    if(curSelecChart != nullptr) curSelecChart->hideMagSize();
//    curSelecChart = nullptr;
//}

Chart_Base * FlowChart::getCurSeleChart()
{
    return curSelecChart;
}


void FlowChart::addChart(Chart_Base *cb)
{
    charts.push_back(cb);
}
bool FlowChart::delChart(Chart_Base *&cb)
{
    for(auto it = charts.begin();it != charts.end();it++)
    {
        if(*it == cb)
        {
            Chart_Base *tmp = cb;
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
            delete tmp;

            return true;
        }
    }
    return false;
}
void FlowChart::addLine(Chart_Base *cb)
{
    line.push_back(cb);
}
bool FlowChart::delLine(Chart_Base *&cb)
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

//added codes
void FlowChart::deleteSelectChart(Chart_Base *cb)
{
    if (cb == curSelecChart)
    {
        if (curSelecChart->chartType == PaintChartType::LINE)
        {
            if(!delLine(curSelecChart))
            {
                qDebug() << "Error";
            }
        }

        else
        {
            if(!delChart(curSelecChart))
            {
                qDebug() << "Error";
            }
        }

    }
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
    curSelecChart = nullptr;
    mouseEventType = MOUSE_EVENT_TYPE::NONE;
    emit disableStyle();
}

bool FlowChart::saveFile(QString filePath)
{
    QFile file(filePath);
    file.open(QIODevice::WriteOnly);
    QDataStream fout(&file);

    Chart_Base::saveStaticValue(fout);
    unsigned long long i;
    i = charts.size();
    fout.writeRawData(reinterpret_cast<const char*>(&i),sizeof(unsigned long long));
    for(auto it = charts.begin(); it!=charts.end(); it++)
    {
        fout<<*(*it);
    }
    i = line.size();
    fout.writeRawData(reinterpret_cast<const char*>(&i),sizeof(unsigned long long));
    for(auto it = line.begin(); it!=line.end(); it++)
    {
        fout<<*(*it)<<*(reinterpret_cast<const Chart_Line*>(*it));
    }
    file.close();
    return true;
}

bool FlowChart::loadFile(QString filePath)
{
    QFile file(filePath);
    file.open(QIODevice::ReadOnly);
    QDataStream fin(&file);
    std::map<int,Chart_Base*> chartMap;

    Chart_Base::loadStaticValue(fin);
    unsigned long long cnt;
    fin.readRawData(reinterpret_cast<char*>(&cnt),sizeof(unsigned long long));
    qDebug()<<"图形个数："<<cnt;
    for(unsigned long long i = 0;i<cnt;i++)
    {
        PaintChartType tmp;
        Chart_Base *cb;
        fin.readRawData(reinterpret_cast<char*>(&tmp),sizeof(PaintChartType));
        switch(tmp)
        {

            case PaintChartType::RECT:
            {
                cb = new Chart_Rect(this);
            }break;
            case PaintChartType::DIAMOND:
            {
                cb = new Chart_Diamond(this);
            }break;
            case PaintChartType::ROUNDRECT:
            {
                cb = new Chart_RoundRect(this);
            }break;
            case PaintChartType::ELLIPSE:
            {
                cb = new Chart_Ellipse(this);
            }break;
            case PaintChartType::TRAPEZOID:
            {
                cb = new Chart_Trapezoid(this);
            }break;
            case PaintChartType::CAPSULE:
            {
                cb = new Chart_Capsule(this);
            }break;
            case PaintChartType::CIRCLE:
            {
                cb = new Chart_Circle(this);
            }break;
            case PaintChartType::DOCUMENT:
            {
                cb = new Chart_Document(this);
            }break;
            case PaintChartType::PARALLELOGRAM:
            {
                cb = new Chart_Parallelogram(this);
            }break;
            case PaintChartType::SUBROUTINE:
            {
                cb = new Chart_Subroutine(this);
            }break;
            case PaintChartType::TEXT:
            {
                cb = new Textbox(this);
            }break;
            default:case PaintChartType::NONE:{
                cb = nullptr;
            }break;
        }
        cb->chartType = tmp;
        fin>>(*cb);
        connect(cb,SIGNAL(sendThisClass(Chart_Base *, int,int)),this,SLOT(setSelecChart(Chart_Base *, int,int)));
        connect(cb,SIGNAL(setTypeChangeSize(ORIENTION)),this,SLOT(setTypeChangeSize(ORIENTION)));
        connect(cb,SIGNAL(setTypeCreateMagPoint(Chart_Base *,ORIENTION,int)),this,SLOT(setTypeCreateMagPoint(Chart_Base *,ORIENTION,int)));
        //added codes
        connect(cb, SIGNAL(deleteThisChartSignal(Chart_Base*)), this, SLOT(deleteSelectChart(Chart_Base*)));

        addChart(cb);
        cb->applyWidthHeight();
        cb->update();
        cb->show();
        chartMap[cb->getID()] = cb;
    }
    fin.readRawData(reinterpret_cast<char*>(&cnt),sizeof(unsigned long long));
    qDebug()<<"连线个数："<<cnt;
    for(unsigned long long i = 0;i<cnt;i++)
    {
        PaintChartType tmp;
        Chart_Base *cb;
        Chart_Line *cl;
        int id;
        fin.readRawData(reinterpret_cast<char*>(&tmp),sizeof(PaintChartType));
        cb = new Chart_Line(this);
        addLine(cb);
        if(nullptr == (cl = dynamic_cast<Chart_Line*>(cb))) qDebug()<<"error";
        fin>>(*cb)>>(*cl);
        try{
            fin.readRawData(reinterpret_cast<char*>(&id),sizeof(int));
            if(id>=0)
            {
                Chart_Base *cbs = chartMap.at(id);
                cbs->addMagiPointStartLine(cl->getStartMagIndex(),cl);
                cl->setStartChart(cbs);
            }
        }catch(std::out_of_range &oor){
            qDebug()<<oor.what()<<"Not Found Start chart.";
        }
        try{
            fin.readRawData(reinterpret_cast<char*>(&id),sizeof(int));
            if(id>=0)
            {
                Chart_Base *cbe = chartMap.at(id);
                cbe->addMagiPointEndLine(cl->getEndMagIndex(),cl);
                cl->setEndChart(cbe);
            }
        }catch(std::out_of_range &oor){
            qDebug()<<oor.what()<<"Not Found End chart.";
        }
        connect(cl,SIGNAL(sendThisClass(Chart_Base *, int,int)),this,SLOT(setSelecChart(Chart_Base *, int,int)));
        connect(cl,SIGNAL(setTypeChangeSize(ORIENTION)),this,SLOT(setTypeChangeSize(ORIENTION)));

        //added codes
        connect(cl, SIGNAL(deleteThisChartSignal(Chart_Base*)), this, SLOT(deleteSelectChart(Chart_Base*)));


        cl->applyWidthHeight();
        cl->update();
        cl->show();
    }
    hideMagSizeAll();
    return true;
}


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
        case MOUSE_EVENT_TYPE::CREATING_CNANGE_SIZE:{
            curChartPos=event->pos();
            int x = event->pos().rx();
            int y = event->pos().ry();
            //curPaintChart->setObjectName(QStringLiteral("tttt"));
            if(curSelecChart) curSelecChart->hideMagSize();
            curSelecChart = curPaintChart;
            curPaintChart->setXY(x,y);
            //curPaintChart->setAttribute(Qt::WA_StyledBackground,true);
            //curPaintChart->setStyleSheet(QStringLiteral("background-color: rgb(100, 100, 100);"));
            if(curPaintChart->chartType == PaintChartType::LINE)
            {
                connect(curPaintChart,SIGNAL(sendThisClass(Chart_Base *, int,int)),this,SLOT(setSelecChart(Chart_Base *, int,int)));
                connect(curPaintChart,SIGNAL(setTypeChangeSize(ORIENTION)),this,SLOT(setTypeChangeSize(ORIENTION)));
                //added codes
                connect(curPaintChart, SIGNAL(deleteThisChartSignal(Chart_Base*)), this, SLOT(deleteSelectChart(Chart_Base*)));

                addLine(curPaintChart);
                Chart_Line *cl = dynamic_cast<Chart_Line*>(curSelecChart);
                emit sendLineStyle(cl->paintChartDrawPen,cl->getStartLineHeadType(),cl->getEndLineHeadType());
            }else
            {
                connect(curPaintChart,SIGNAL(sendThisClass(Chart_Base *, int,int)),this,SLOT(setSelecChart(Chart_Base *, int,int)));
                connect(curPaintChart,SIGNAL(setTypeChangeSize(ORIENTION)),this,SLOT(setTypeChangeSize(ORIENTION)));
                connect(curPaintChart,SIGNAL(setTypeCreateMagPoint(Chart_Base *,ORIENTION,int)),this,SLOT(setTypeCreateMagPoint(Chart_Base *,ORIENTION,int)));
                //connect(curPaintChart,SIGNAL(hideThisClass()),this,SLOT(resetSeletcChart()));

                //added codes
                connect(curPaintChart, SIGNAL(deleteThisChartSignal(Chart_Base*)), this, SLOT(deleteSelectChart(Chart_Base*)));


                addChart(curPaintChart);
                emit sendChartStyle(curSelecChart->paintChartDrawPen,curSelecChart->paintChartFillPen);
            }
            curPaintChart->update();
            curPaintChart->show();
            //mouseEventType = MOUSE_EVENT_TYPE::CREATING_CNANGE_SIZE;
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
    //if(event->isAccepted()) return;
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
            if(curSelecChart == nullptr)
            {
                setCursor(QCursor(Qt::ArrowCursor));
                hideMagSizeAll();
            }
        }break;
        case MOUSE_EVENT_TYPE::CREATING_CNANGE_SIZE:
        {
            int x = event->pos().rx(),y = event->pos().ry();
            if(curSelecChart)
            {
                if(curSelecChart->chartType == PaintChartType::LINE)
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
                    //if(!lineSelectChart) reinterpret_cast<Chart_Line *>(curSelecChart)->resetEndChart();
                }
                curPaintChart->setWidthHeight(x,y);
            }
            event->accept();
        }break;
        case MOUSE_EVENT_TYPE::RUNTIME_CHANGE_SIZE:
        {
            int x = event->pos().rx(),y = event->pos().ry();

            if(curSelecChart->chartType == PaintChartType::LINE)
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
                //if(!lineSelectChart) reinterpret_cast<Chart_Line *>(curSelecChart)->resetEndChart();
            }
            curSelecChart->setWidthHeight(x,y,sizePointDirect);
            event->accept();
        }break;
        case MOUSE_EVENT_TYPE::RUNTIME_CREATE_MAGPOINT:
        {
            if(newLineChart == nullptr)
            {
                newLineChart = new Chart_Line(this);
                if(curSelecChart)
                    curSelecChart->hideMagSize();
                curSelecChart = newLineChart;
                addLine(newLineChart);
                //newLineChart->setAttribute(Qt::WA_StyledBackground,true);
                //newLineChart->setStyleSheet(QStringLiteral("background-color: rgb(100, 100, 100);"));
                connect(newLineChart,SIGNAL(sendThisClass(Chart_Base *, int,int)),this,SLOT(setSelecChart(Chart_Base *, int,int)));
                connect(newLineChart,SIGNAL(setTypeChangeSize(ORIENTION)),this,SLOT(setTypeChangeSize(ORIENTION)));

                //added codes
                connect(newLineChart, SIGNAL(deleteThisChartSignal(Chart_Base*)), this, SLOT(deleteSelectChart(Chart_Base*)));


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
                //if(*it == newLineFromSelectChart) continue;
                if((*it)->autoSetMagi(x,y,magPointToIndex))
                {
                    newLineToSelectChart = *it;
                    break;
                }else{
                    newLineToSelectChart = nullptr;
                }
            }
            //if(!newLineToSelectChart) newLineChart->resetEndChart();
            newLineChart->setWidthHeight(x,y,ORIENTION::ENDPOINT);
            event->ignore();
        }break;
        case MOUSE_EVENT_TYPE::RUNTIME_CHANGE_POS:
        {
            if(curSelecChart->chartType != PaintChartType::LINE)
            {
                curSelecChart->setXY(event->pos().rx() - curSelecChartPos.rx(),event->pos().ry() - curSelecChartPos.ry());
                event->accept();
            }
        }break;
    }
}

void FlowChart::mouseReleaseEvent(QMouseEvent *event)
{
    switch(mouseEventType)
    {
        case MOUSE_EVENT_TYPE::CREATING_CNANGE_SIZE:
        {
            //curPaintChart->updatePath();
            resetPaintChartType();
            if(curSelecChart->chartType == PaintChartType::LINE)
            {
                Chart_Line *cl = reinterpret_cast<Chart_Line *>(curSelecChart);

                if(lineSelectChart)
                {
                    lineSelectChart->addMagiPointEndLine(magPointIndex,curSelecChart);
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
            QUndoCommand* command = new AddChartCommand(this, curSelecChart);
            undoStack->push(command);
            oldx = event->pos().rx();
            oldy = event->pos().ry();
        }break;
        case MOUSE_EVENT_TYPE::RUNTIME_CHANGE_SIZE:
        {
            if(curSelecChart->chartType == PaintChartType::LINE)
            {
                Chart_Line *cl = reinterpret_cast<Chart_Line *>(curSelecChart);
                if(lineSelectChart)
                {
                    if(sizePointDirect == ORIENTION::STARTPOINT)
                    {
                        cl->resetStartChart();
                        lineSelectChart->addMagiPointStartLine(magPointIndex,curSelecChart);
                        lineSelectChart->hideMagOnly();
                        cl->setStartChart(lineSelectChart);
                        cl->setStartMagIndex(magPointIndex);
                        cl->setStartDirect(lineSelectChart->getMagiPointDirect(magPointIndex));
                    }else if(sizePointDirect == ORIENTION::ENDPOINT)
                    {
                        cl->resetEndChart();
                        lineSelectChart->addMagiPointEndLine(magPointIndex,curSelecChart);
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
            //else{
                QUndoCommand* command = new ChangeChartSizeCommand(curSelecChart,oldx,oldy,event->pos().rx(),event->pos().ry(),sizePointDirect);
                undoStack->push(command);
            //}
            mouseEventType = MOUSE_EVENT_TYPE::NONE;
            lineSelectChart = nullptr;
            setFileSetSaved(false);
            event->accept();
            oldx = event->pos().rx();
            oldy = event->pos().ry();
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
            QPoint oldpos = curChartPos;
            QPoint newpos = event->pos()-curSelecChartPos;
            QUndoCommand* command = new MoveChartCommand(curSelecChart,oldpos,newpos);
            undoStack->push(command);
            curChartPos=event->pos()-curSelecChartPos;
        }break;
        default:case MOUSE_EVENT_TYPE::NONE:
        {
            event->ignore();
        }break;
    }

}

//added codes
void FlowChart::copyMagPointData(Chart_Base *copiedChart, Chart_Base *curSelecChart)
{
    for (size_t i = 0; i < curSelecChart->magPoint.i_point.size(); ++i)
    {
        if (curSelecChart->magPoint.i_point[i])
        {

            for (size_t j = 0; j < curSelecChart->magPoint.i_point[i]->i_lineStart.size(); ++j)
            {

                if (curSelecChart->magPoint.i_point[i]->i_lineStart[j])
                {
                    Chart_Base* newBase = nullptr;

                    switch(curSelecChart->magPoint.i_point[i]->i_lineStart[j]->chartType)
                    {
                    case PaintChartType::RECT:
                    {
                        newBase = new Chart_Rect(this);
                    }break;
                    case PaintChartType::DIAMOND:
                    {
                        newBase = new Chart_Diamond(this);
                    }break;
                    case PaintChartType::ROUNDRECT:
                    {
                        newBase = new Chart_RoundRect(this);
                    }break;
                    case PaintChartType::ELLIPSE:
                    {
                        newBase = new Chart_Ellipse(this);
                    }break;
                    case PaintChartType::LINE:
                    {
                        newBase = new Chart_Line(this);
                    }break;
                    case PaintChartType::TRAPEZOID:
                    {
                        newBase = new Chart_Trapezoid(this);
                    }break;
                    case PaintChartType::CAPSULE:
                    {
                        newBase = new Chart_Capsule(this);
                    }break;
                    case PaintChartType::CIRCLE:
                    {
                        newBase = new Chart_Circle(this);
                    }break;
                    case PaintChartType::DOCUMENT:
                    {
                        newBase = new Chart_Document(this);
                    }break;
                    case PaintChartType::PARALLELOGRAM:
                    {
                        newBase = new Chart_Parallelogram(this);
                    }break;
                    case PaintChartType::SUBROUTINE:
                    {
                        newBase = new Chart_Subroutine(this);
                    }break;
                    case PaintChartType::TEXT:
                    {
                        newBase = new Textbox(this);
                    }break;

                    default:case PaintChartType::NONE:{
                        newBase = nullptr;
                    }break;
                    }

                    if (newBase)
                    {
                        newBase->copyData(curSelecChart->magPoint.i_point[i]->i_lineStart[j]);
                        copyMagPointData(newBase, curSelecChart->magPoint.i_point[i]->i_lineStart[j]);
                        if (newBase->chartType == PaintChartType::LINE)
                        {
                            Chart_Line* copiedLine = dynamic_cast<Chart_Line*>(newBase);
                            Chart_Line* selecLine = dynamic_cast<Chart_Line*>(curSelecChart);
                            copiedLine->copyLineData(curSelecChart);
                            if (selecLine->startChartMag){
                                copiedLine->startChartMag->copyData(selecLine->startChartMag);
                            }
                            else
                            {
                                copiedLine->startChartMag = nullptr;
                            }

                            if (selecLine->endChartMag)
                            {
                                copiedLine->endChartMag->copyData(selecLine->endChartMag);
                            }
                            else
                            {
                                copiedLine->endChartMag = nullptr;
                            }

                            newBase = copiedLine;
                        }
                    }

                    copiedChart->magPoint.i_point[i]->i_lineStart[j] = newBase;

                }

                else
                {
                    copiedChart->magPoint.i_point[i]->i_lineStart[j] = nullptr;
                }

            }



            for (size_t j = 0; j < curSelecChart->magPoint.i_point[i]->i_lineEnd.size(); ++j)
            {
                if (curSelecChart->magPoint.i_point[i]->i_lineEnd[j])
                {
                    Chart_Base* newBase = nullptr;

                    switch(curSelecChart->magPoint.i_point[i]->i_lineEnd[j]->chartType)
                    {
                    case PaintChartType::RECT:
                    {
                        newBase = new Chart_Rect(this);
                    }break;
                    case PaintChartType::DIAMOND:
                    {
                        newBase = new Chart_Diamond(this);
                    }break;
                    case PaintChartType::ROUNDRECT:
                    {
                        newBase = new Chart_RoundRect(this);
                    }break;
                    case PaintChartType::ELLIPSE:
                    {
                        newBase = new Chart_Ellipse(this);
                    }break;
                    case PaintChartType::LINE:
                    {
                        newBase = new Chart_Line(this);
                    }break;
                    case PaintChartType::TRAPEZOID:
                    {
                        newBase = new Chart_Trapezoid(this);
                    }break;
                    case PaintChartType::CAPSULE:
                    {
                        newBase = new Chart_Capsule(this);
                    }break;
                    case PaintChartType::CIRCLE:
                    {
                        newBase = new Chart_Circle(this);
                    }break;
                    case PaintChartType::DOCUMENT:
                    {
                        newBase = new Chart_Document(this);
                    }break;
                    case PaintChartType::PARALLELOGRAM:
                    {
                        newBase = new Chart_Parallelogram(this);
                    }break;
                    case PaintChartType::SUBROUTINE:
                    {
                        newBase = new Chart_Subroutine(this);
                    }break;
                    case PaintChartType::TEXT:
                    {
                        newBase = new Textbox(this);
                    }break;

                    default:case PaintChartType::NONE:{
                        newBase = nullptr;
                    }break;
                    }

                    if (newBase)
                    {
                        newBase->copyData(curSelecChart->magPoint.i_point[i]->i_lineEnd[j]);
                        copyMagPointData(newBase, curSelecChart->magPoint.i_point[i]->i_lineEnd[j]);

                        if (newBase->chartType == PaintChartType::LINE)
                        {
                            Chart_Line* copiedLine = dynamic_cast<Chart_Line*>(newBase);
                            Chart_Line* selecLine = dynamic_cast<Chart_Line*>(curSelecChart);
                            copiedLine->copyLineData(curSelecChart);
                            if (selecLine->startChartMag){
                                copiedLine->startChartMag->copyData(selecLine->startChartMag);
                            }
                            else
                            {
                                copiedLine->startChartMag = nullptr;
                            }

                            if (selecLine->endChartMag)
                            {
                                copiedLine->endChartMag->copyData(selecLine->endChartMag);
                            }
                            else
                            {
                                copiedLine->endChartMag = nullptr;
                            }

                            newBase = copiedLine;
                        }

                    }

                    copiedChart->magPoint.i_point[i]->i_lineEnd[j] = newBase;
                }

                else
                {
                    copiedChart->magPoint.i_point[i]->i_lineEnd[j] = nullptr;
                }

            }

        }

    }
}



//added method for text search and replace, highlight
void FlowChart::textSearch(QString searchText)
{
    if (searchText == ""){
        return;
    }

    offHighlight();
    textSearchResult.clear();

    for (Chart_Base* chart: charts) {
        if (chart->chartText.textType) {
            if (chart->chartText.textType2->toPlainText().contains(searchText)) {
                textSearchResult.push_back(chart);
                qDebug() << chart->chartText.textType2->toPlainText();
            }


            // if (chart->chartText.textType1->text().contains(searchText)) {
            //     textSearchResult.push_back(chart);
            //     qDebug() << chart->chartText.textType1->text();
            // }
        }

        else {
            if (chart->chartText.textType2->toPlainText().contains(searchText)) {
                textSearchResult.push_back(chart);
                qDebug() << chart->chartText.textType2->toPlainText();
            }
        }
    }

    for (Chart_Base* l : line) {
        if (l->chartText.textType) {

            if (l->chartText.textType2->toPlainText().contains(searchText)) {
                textSearchResult.push_back(l);
                qDebug() << l->chartText.textType2->toPlainText();
            }
            // if (l->chartText.textType1->text().contains(searchText)) {
            //     textSearchResult.push_back(l);
            //     qDebug() << l->chartText.textType1->text();
            // }
        }

        else {
            if (l->chartText.textType2->toPlainText().contains(searchText)) {
                textSearchResult.push_back(l);
                qDebug() << l->chartText.textType2->toPlainText();
            }
        }
    }

    onHighlight(searchText);

}


void FlowChart::onHighlight(QString searchText)
{
    for (Chart_Base* result : textSearchResult)
    {
        //이쪽 코드가 문제다
        if (result->chartText.textType) {

            QTextCursor cursor(result->chartText.textType2->document());

            cursor.beginEditBlock();
            QTextCharFormat format;
            format.setFontWeight(QFont::Bold);
            format.setFontUnderline(true);
            //format.setForeground(Qt::red);

            while (!cursor.isNull() && !cursor.atEnd()) {
                cursor = result->chartText.textType2->document()->find(searchText, cursor);

                if (!cursor.isNull()) {
                    cursor.setCharFormat(format);
                }
            }

            cursor.endEditBlock();

            qDebug() << "Text on";
            qDebug() << result->chartText.textType2->toPlainText();



            // QString fullText = result->chartText.textType1->text();
            // //QString highlightText;

            // QFont font = result->chartText.textType1->font();
            // font.setBold(true);
            // font.setUnderline(true);
            // result->chartText.textType1->setFont(font);

            // QPalette palette = result->chartText.textType1->palette();
            // palette.setColor(QPalette::WindowText, Qt::red);
            // result->chartText.textType1->setPalette(palette);



            //qDebug() << "label on";
            // qDebug() << fullText;
            //qDebug() << highlightText;


        }

        else {
            QTextCursor cursor(result->chartText.textType2->document());

            cursor.beginEditBlock();
            QTextCharFormat format;
            format.setFontWeight(QFont::Bold);
            format.setFontUnderline(true);
            //format.setForeground(Qt::red);

            while (!cursor.isNull() && !cursor.atEnd()) {
                cursor = result->chartText.textType2->document()->find(searchText, cursor);

                if (!cursor.isNull()) {
                    cursor.setCharFormat(format);
                }
            }

            cursor.endEditBlock();

            qDebug() << "Text on";
            qDebug() << result->chartText.textType2->toPlainText();

            // QTextCursor cursor(result->chartText.textType2->document()->rootFrame());
            // QTextCharFormat format;

            // format.setFontWeight(QFont::Bold);
            // format.setFontUnderline(true);
            // format.setForeground(Qt::red);

            // cursor.select(QTextCursor::Document);
            // cursor.mergeCharFormat(format);
            // result->chartText.textType2->setTextCursor(cursor);

        }
    }
}


void FlowChart::offHighlight()
{
    for (Chart_Base* result : textSearchResult)
    {
        if (result->chartText.textType) {

            QTextCursor cursor(result->chartText.textType2->document()->rootFrame());
            QTextCharFormat format;

            format.setFontWeight(QFont::Normal);
            format.setFontUnderline(false);
            //format.setForeground(Qt::black);

            cursor.select(QTextCursor::Document);
            cursor.setCharFormat(format);
            result->chartText.textType2->setTextCursor(cursor);


            // QFont font = result->chartText.textType1->font();
            // font.setBold(false);
            // font.setUnderline(false);
            // result->chartText.textType1->setFont(font);

            // QPalette palette = result->chartText.textType1->palette();
            // palette.setColor(QPalette::WindowText, Qt::black);
            // result->chartText.textType1->setPalette(palette);

        }

        else {
            QTextCursor cursor(result->chartText.textType2->document()->rootFrame());
            QTextCharFormat format;

            format.setFontWeight(QFont::Normal);
            format.setFontUnderline(false);
            //format.setForeground(Qt::black);

            cursor.select(QTextCursor::Document);
            cursor.setCharFormat(format);
            result->chartText.textType2->setTextCursor(cursor);
        }
    }
}

void FlowChart::replaceText(QString searchText, QString replaceText)
{
    textSearch(searchText);

    offHighlight();

    for (Chart_Base* result : textSearchResult)
    {
        if (result->chartText.textType) {
            QString curText = result->chartText.textType2->toPlainText();
            QString updateText = curText.replace(searchText, replaceText);
            result->chartText.textType2->setPlainText(updateText);

            // QString curText = result->chartText.textType1->text();
            // QString updateText = curText.replace(searchText, replaceText);
            // result->chartText.textType1->setText(updateText);
        }

        else {
            QString curText = result->chartText.textType2->toPlainText();
            QString updateText = curText.replace(searchText, replaceText);
            result->chartText.textType2->setPlainText(updateText);
        }
    }

    textSearch(searchText);

}

void FlowChart::insertRect(){

        if(rectX==0){
            rectX=600;
            rectY=350;
        }
        else{
            rectX = rectX+20;
            rectY = rectY+20;
        }
        QPoint globalPoint(rectX,rectY);
        QPoint point = this->mapFromGlobal(globalPoint);
        curSelecChart = curPaintChart;
        //curPaintChart->widgetEnd.rx() = x+20;
        //curPaintChart->widgetEnd.ry() = y+20;
        curPaintChart->setXY(point.rx(),point.ry());
        connect(curPaintChart,SIGNAL(sendThisClass(Chart_Base *, int,int)),this,SLOT(setSelecChart(Chart_Base *, int,int)));
        connect(curPaintChart,SIGNAL(setTypeChangeSize(ORIENTION)),this,SLOT(setTypeChangeSize(ORIENTION)));
        connect(curPaintChart,SIGNAL(setTypeCreateMagPoint(Chart_Base *,ORIENTION,int)),this,SLOT(setTypeCreateMagPoint(Chart_Base *,ORIENTION,int)));
        addChart(curPaintChart);
        emit sendChartStyle(curSelecChart->paintChartDrawPen,curSelecChart->paintChartFillPen);
        curPaintChart->setWidthHeight(rectX+20,rectY+20,ORIENTION::SOUTHEAST);
        //curPaintChart->setWidthHeight(20,20,ORIENTION::SOUTHEAST);
        //curPaintChart->applyWidthHeight();
        curPaintChart->update();
        curPaintChart->show();
        resetPaintChartType();
        curPaintChart = nullptr;
        mouseEventType = MOUSE_EVENT_TYPE::NONE;
        setFileSetSaved(false);

}

void FlowChart::insertText(){
        if(rectX==0){
            rectX=600;
            rectY=350;
        }
        else{
            rectX = rectX+20;
            rectY = rectY+20;
        }
        QPoint globalPoint(rectX,rectY);
        QPoint point = this->mapFromGlobal(globalPoint);
        curSelecChart = curPaintChart;
        //curPaintChart->widgetEnd.rx() = x+20;
        //curPaintChart->widgetEnd.ry() = y+20;
        curPaintChart->setXY(point.rx(),point.ry());
        connect(curPaintChart,SIGNAL(sendThisClass(Chart_Base *, int,int)),this,SLOT(setSelecChart(Chart_Base *, int,int)));
        connect(curPaintChart,SIGNAL(setTypeChangeSize(ORIENTION)),this,SLOT(setTypeChangeSize(ORIENTION)));
        connect(curPaintChart,SIGNAL(setTypeCreateMagPoint(Chart_Base *,ORIENTION,int)),this,SLOT(setTypeCreateMagPoint(Chart_Base *,ORIENTION,int)));
        addChart(curPaintChart);
        emit sendChartStyle(curSelecChart->paintChartDrawPen,curSelecChart->paintChartFillPen);
        curPaintChart->setWidthHeight(rectX+20,rectY+20,ORIENTION::SOUTHEAST);
        //curPaintChart->setWidthHeight(20,20,ORIENTION::SOUTHEAST);
        //curPaintChart->applyWidthHeight();
        curPaintChart->update();
        curPaintChart->show();
        resetPaintChartType();
        curPaintChart = nullptr;
        mouseEventType = MOUSE_EVENT_TYPE::NONE;
        setFileSetSaved(false);

}


