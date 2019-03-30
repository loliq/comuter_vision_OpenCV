#include "qenhancedgraphicsview.h"
#include    <QPoint>

QEnhancedGraphicsView::QEnhancedGraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{

}

void QEnhancedGraphicsView::wheelEvent(QWheelEvent *event)
{
    if (event->orientation() == Qt::Vertical)
    {
        if(event->pos().x() != mousePosOld.x() || event->pos().y() != mousePosOld.y())
        {
             sceneMousePosOld = this->mapToScene(event->pos());
             mousePosOld = event->pos();
        }

        double angleDeltaY = event->angleDelta().y();
        double zoomFactor = qPow(1.0015, angleDeltaY);
        scale(zoomFactor, zoomFactor);
        //if(angleDeltaY > 0)
        {

            sceneMousePos = this->mapToScene(event->pos());
            this->translate(sceneMousePos.x()- sceneMousePosOld.x(),
                            sceneMousePos.y()- sceneMousePosOld.y());


        }
        this->viewport()->update();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}
void QEnhancedGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QPoint point = event->pos();
    emit sigMouseMovePoint(point);
    QGraphicsView::mouseMoveEvent(event);
}

void QEnhancedGraphicsView::mousePressEvent(QMouseEvent *event)
{
   //鼠标左键按下事件
    if(event->button() == Qt::LeftButton)
    {
        QPoint point = event->pos();
        emit sigMouseClicked(point);
    }
    QGraphicsView::mousePressEvent(event);
}


void QEnhancedGraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{ //鼠标双击事件
    if (event->button()==Qt::LeftButton)
    {
//        QPoint  point;
        QPoint point=event->pos(); //QGraphicsView的坐标
        emit sigMouseDoubleClick(point);//释放信号
    }
    QGraphicsView::mouseDoubleClickEvent(event);
}

void QEnhancedGraphicsView::keyPressEvent(QKeyEvent *event)
{ //按键事件
    emit sigKeyPress(event);
    QGraphicsView::keyPressEvent(event);
}
