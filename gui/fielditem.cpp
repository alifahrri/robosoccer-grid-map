#include "fielditem.h"
//#include "robottrajectory.h"

#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <iostream>

#define PIXEL_WIDTH 900
#define PIXEL_HEIGHT 600

FieldItem::FieldItem()
{
  QString pixmap_path = ":/lines.png";
  lines = QPixmap(pixmap_path).scaledToHeight(PIXEL_HEIGHT);
}

void FieldItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  painter->setBrush(QColor(0,182,0));
  painter->setPen(Qt::white);
  painter->drawRect(-PIXEL_WIDTH/2,-PIXEL_HEIGHT/2,PIXEL_WIDTH,PIXEL_HEIGHT);
  painter->drawPixmap(-PIXEL_WIDTH/2,-PIXEL_HEIGHT/2,lines);
}

void FieldItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  qDebug() << "mouse pressed : " << event->pos().x()/100.0 << "," << event->pos().y()/100.0 << '\n';
  click_pos.first = event->pos().x();
  click_pos.second = event->pos().y();
  if(click_cb) click_cb(click_pos.first, click_pos.second);
}

void FieldItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  qDebug() << "mouse moved : " << event->pos().x()/100.0 << "," << event->pos().y()/100.0 << '\n';
  release_pos.first = event->pos().x();
  release_pos.second = event->pos().y();
  if(move_cb) move_cb(release_pos.first,release_pos.second);
}

void FieldItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

  qDebug() << "mouse released : " << event->pos().x()/100.0 << "," << event->pos().y()/100.0 << '\n';
  release_pos.first = event->pos().x();
  release_pos.second = event->pos().y();
  if(release_cb) release_cb(release_pos.first,release_pos.second);
}

QRectF FieldItem::boundingRect() const
{
  return QRectF(-PIXEL_WIDTH/2,-PIXEL_HEIGHT/2,PIXEL_WIDTH,PIXEL_HEIGHT);
}
