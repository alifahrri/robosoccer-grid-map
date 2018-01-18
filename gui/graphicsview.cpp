#include "graphicsview.h"
#include <QMouseEvent>
#include <QKeyEvent>

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{

}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    if(press_callback)
    {
        press_pos = mapToScene(event->pos());
        bool en = (event->button()==Qt::LeftButton);
        auto x = press_pos.x();
        auto y = press_pos.y();
        press_callback(en,x,y);
    }
    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if(release_callback)
    {
        auto p = mapToScene(event->pos());
        release_callback(press_pos.x(),press_pos.y(),p.x(),p.y());
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void GraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(double_click_callback)
    {
        auto p = mapToScene(event->pos());
        double_click_callback(p.x(),p.y());
    }
    QGraphicsView::mouseDoubleClickEvent(event);
}

void GraphicsView::keyPressEvent(QKeyEvent *event)
{
    if((event->key() == Qt::Key_Plus) || (event->key()==Qt::Key_Equal))
        this->scale(1.1,1.1);
    else if((event->key() == Qt::Key_Minus ) || (event->key() == Qt::Key_Underscore))
        this->scale(0.9,0.9);
    QGraphicsView::keyPressEvent(event);
}
