#ifndef FIELDITEM_H
#define FIELDITEM_H

#include <QGraphicsItem>
#include <functional>
#include <QMouseEvent>
//#include "robottrajectory.h"

typedef std::function<void(std::vector<std::pair<double,double>>)> Callback;
//typedef std::function<void(std::vector<fukuro::RobotState>)> Callback2;


class FieldItem : public QGraphicsItem
{
public:
  typedef std::function<void(double,double)> MoveCallback;
  typedef std::function<void(double,double)> ClickCallback;
  typedef std::function<void(double,double)> ReleaseCallback;
  MoveCallback move_cb = nullptr;
  ClickCallback click_cb = nullptr;
  ReleaseCallback release_cb = nullptr;
public:
  FieldItem();
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
  QRectF boundingRect() const Q_DECL_OVERRIDE;
  std::pair<double,double> getClickPos(){
    return click_pos;
  }
  std::pair<double,double> getReleasePos(){
    return release_pos;
  }

  // /*
private:
  void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
  // */
  float centerCircleRadius;
  float horizontalLine;
  float verticalLine;
  float arcRadius;
  float arcWidth;
  Callback cb;
  //Callback2 cb2;
  //std::vector<fukuro::RobotState> rs;
  std::vector<std::pair<double,double>> true_pos;
  std::vector<std::pair<double,double>> noisy_pos;
  std::vector<std::pair<double,double>> wp;
  enum { INSERT, RESET, SHIFT_RESET } click_mode;

  //signals:
  //    void mouseClicked(double,double);
  //    void mouseReleased(double,double);

private:
  QPixmap lines;
  std::pair<double,double> click_pos;
  std::pair<double,double> release_pos;
};


#endif //FIELDITEM_H
