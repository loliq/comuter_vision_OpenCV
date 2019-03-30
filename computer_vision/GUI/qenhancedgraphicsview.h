#ifndef QENHANCEDGRAPHICSVIEW_H
#define QENHANCEDGRAPHICSVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QtMath>
#include <QContextMenuEvent>
#include <QMenu>
#include <QGraphicsItem>
#include <QDebug>
#include <QGraphicsEffect>

class QEnhancedGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit QEnhancedGraphicsView(QWidget *parent = nullptr);
protected:
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

signals:
    void sigMouseMovePoint(QPoint point);
    void sigMouseClicked(QPoint point);
    void sigMouseDoubleClick(QPoint point); //双击事件
    void sigKeyPress(QKeyEvent *event);

public slots:
private:
    QPointF sceneMousePos;
    QPointF sceneMousePosOld;
    QPoint  mousePosOld = QPoint(0, 0);
};

#endif // QENHANCEDGRAPHICSVIEW_H
