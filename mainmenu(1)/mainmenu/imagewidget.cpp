#include "imagewidget.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QGraphicsSceneDragDropEvent>
#include <QDrag>
#include <math.h>

ImageWidget::ImageWidget(QPixmap *pixmap)
{
    m_pix = *pixmap;
    setAcceptDrops(true);//If enabled is true, this item will accept hover events; otherwise, it will ignore them. By default, items do not accept hover events.
    m_scaleValue = 0;
    m_scaleDefault = 0;
    m_isMove = false;
}

ImageWidget::ImageWidget(QPixmap pixmap)
{
    m_pix = pixmap;
    setAcceptDrops(true);//If enabled is true, this item will accept hover events; otherwise, it will ignore them. By default, items do not accept hover events.
    m_scaleValue = 0;
    m_scaleDefault = 0;
    m_isMove = false;
}
QRectF ImageWidget::boundingRect() const
{
    return QRectF(-m_pix.width() / 2, -m_pix.height() / 2,
                  m_pix.width(), m_pix.height());
}

void ImageWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                        QWidget *)
{
    painter->drawPixmap(-m_pix.width() / 2, -m_pix.height() / 2, m_pix);

}

void ImageWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button()== Qt::LeftButton)
    {
        m_startPos = event->pos();//鼠标左击时，获取当前鼠标在图片中的坐标，
        //qDebug()<<m_startPos;
        m_isMove = true;//标记鼠标左键被按下
    }
    else if(event->button() == Qt::RightButton)
    {
        ResetItemPos();
    }

}

void ImageWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_isMove)
    {

        //qDebug()<<"mouseMoveEvent"<<event->pos()<<m_startPos<<m_scaleValue;
        QPointF point = (event->pos() - m_startPos)*m_scaleValue;
        //qDebug()<<"point"<<point;
        moveBy(point.x(), point.y());
    }
}

void ImageWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    m_isMove = false;//标记鼠标左键已经抬起
}


void  ImageWidget::wheelEvent(QGraphicsSceneWheelEvent *event)//鼠标滚轮事件
{
    if((event->delta() > 0)&&(m_scaleValue >= 50))//最大放大到原始图像的50倍
    {
        return;
    }
    else if((event->delta() < 0)&&(m_scaleValue <= m_scaleDefault))//图像缩小到自适应大小之后就不继续缩小
    {
        ResetItemPos();//重置图片大小和位置，使之自适应控件窗口大小
    }
    else
    {
        qreal qrealOriginScale = m_scaleValue;
        if(event->delta() > 0)//鼠标滚轮向前滚动
        {
            m_scaleValue*=1.1;//每次放大10%
        }
        else
        {
            m_scaleValue*=0.9;//每次缩小10%
        }
        setScale(m_scaleValue);
        if(event->delta() > 0)
        {
            moveBy(-event->pos().x()*qrealOriginScale*0.1, -event->pos().y()*qrealOriginScale*0.1);//使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
        }
        else
        {
            moveBy(event->pos().x()*qrealOriginScale*0.1, event->pos().y()*qrealOriginScale*0.1);//使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
        }
    }
}

void ImageWidget::setQGraphicsViewWH(int nwidth, int nheight)//将主界面的控件QGraphicsView的width和height传进本类中，并根据图像的长宽和控件的长宽的比例来使图片缩放到适合控件的大小
{
    int nImgWidth = m_pix.width();
    int nImgHeight = m_pix.height();
    qreal temp1 = nwidth*1.0/nImgWidth;
    qreal temp2 = nheight*1.0/nImgHeight;
    //qDebug()<<nImgWidth<<nImgHeight<<temp1<<temp2<<endl;
    if(temp1>temp2)
    {
        m_scaleDefault = temp2;
    }
    else
    {
        m_scaleDefault = temp1;
    }
    setScale(m_scaleDefault);
    m_scaleValue = m_scaleDefault;

}

void ImageWidget::ResetItemPos()//重置图片位置
{
    m_scaleValue = m_scaleDefault;//缩放比例回到一开始的自适应比例
    setScale(m_scaleDefault);//缩放到一开始的自适应大小
    setPos(0,0);
}

qreal ImageWidget::getScaleValue() const
{
    return m_scaleValue;
}
void ImageWidget::setRectPos(QRect *rect1, QRect *rect2)
{
    // 根据矩形位置进行联动
    //固定缩放比例
    //根据传入坐标计算缩放后位置

    QPointF point(rect1->left()-m_pix.width()/2,rect2->top()-m_pix.height()/2);

    //moveBy(point.x()/10, point.y()/10);

    QGraphicsSceneMouseEvent *event = new QGraphicsSceneMouseEvent();
    event->setPos(point);
    qDebug()<<"setRectPos"<<event->pos();
    m_isMove = true;
    int k = 1;
    if(point.y()>0)
        k = 1;
    else
        k = -1;
    m_startPos = QPointF(event->pos().x(),(m_pix.height()/2)*k);
    this->mouseMoveEvent(event);

}

QPixmap ImageWidget::getmap(){
    return m_pix;
}
