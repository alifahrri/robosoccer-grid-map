#ifndef GRIDITEM_H
#define GRIDITEM_H

#include <QGraphicsItem>
#include <functional>

class GridMap;

class GridItem : public QGraphicsItem
{
    typedef std::function<std::vector<uint16_t>(double,double)> MotionTestCallback;
public:
    GridItem(GridMap& grid_map);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
public:
    MotionTestCallback motion_test_cb = nullptr;
    QPointF vel_point;
protected:
    void setTestPoint(const QPointF &tp);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
private:
    GridMap& _grid_map;
    QPointF _test_point;
    bool test;
};

#endif // GRIDITEM_H
