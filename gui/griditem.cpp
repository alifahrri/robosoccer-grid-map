#include "griditem.h"
#include "gridmap.h"
#include <cmath>
#include <QDebug>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>

GridItem::GridItem(GridMap &grid_map)
    : _grid_map(grid_map),
      test(false),
      vel_point(QPointF(0.0,0.0))
{
    setAcceptHoverEvents(true);
}

void GridItem::setTestPoint(const QPointF &tp)
{
    _test_point = tp;
}

void GridItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    test = true;
    setTestPoint(event->pos());
    update();
}

void GridItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    setTestPoint(event->pos());
    update();
}

void GridItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    test = false;
    update();
}

void GridItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    vel_point = event->pos();
    qDebug() << "griditem mouse press" << vel_point;
}

void GridItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto map = _grid_map.getMap();
    auto cell_size = _grid_map.cellSize()*100.0;
    auto rect = QRectF(-cell_size/2,-cell_size/2,cell_size,cell_size);
    QVector<QRectF> draw_rect;
    painter->setPen(QPen(Qt::black,1));
    if(motion_test_cb)
    {
        auto motion_index = motion_test_cb(_test_point.x()/100.0,_test_point.y()/100.0);
        for(auto i : motion_index)
            std::get<2>(map.at(i)) = 125.0;
    }
    for(auto& m : map)
    {
        auto x = std::get<0>(m)*100.0;
        auto y = std::get<1>(m)*100.0;
        auto logodds = std::get<2>(m);
        auto v = (1.0-(1.0/(1.0+std::exp(logodds))));
        auto r = rect.translated(x,y);
        auto color = (1.0-v)*255;
        color = std::min(255.0,std::max(0.0,color));
        painter->setBrush(QBrush(QColor(color,color,color,255*(255-color)/255)));
        painter->drawRect(r);
        auto font = painter->font();
        font.setPixelSize(2);
        painter->setFont(font);
        painter->drawText(r,Qt::AlignVCenter | Qt::AlignHCenter, QString::number(v));
    }
    if(test)
    {
        auto test_cell = _grid_map.getCell(_test_point.x()/100.0,_test_point.y()/100.0);
        auto neighbor = _grid_map.getNeighbor(test_cell);
        painter->setBrush(Qt::blue);
        for(auto& n : neighbor)
        {
            auto x = std::get<0>(n)*100.0;
            auto y = std::get<1>(n)*100.0;
            auto v = std::get<2>(n);
            auto r = rect.translated(x,y);
            painter->drawRect(r);
        }
        auto c_x = std::get<0>(test_cell)*100.0;
        auto c_y = std::get<1>(test_cell)*100.0;
        painter->setBrush(Qt::red);
        painter->drawRect(rect.translated(c_x,c_y));
    }
////    draw circle
//    auto px = vel_point.x()/100.0;
//    auto py = vel_point.y()/100.0;
//    auto circle = _grid_map.circle(px,py,0.5);
//    for(auto& c : circle)
//    {
//        auto x = c.first*100.0;
//        auto y = c.second*100.0;
//        painter->setBrush(QBrush(Qt::green));
//        painter->drawRect(rect.translated(x,y));
//    }
    painter->setBrush(Qt::gray);
    painter->drawEllipse(-3,-3,6,6);
}

QRectF GridItem::boundingRect() const
{
    if(scene())
        return scene()->sceneRect();
    else
        return QRectF(-100,-100,200,200);
}
