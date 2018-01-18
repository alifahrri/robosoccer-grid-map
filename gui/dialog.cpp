#include "dialog.h"
#include "ui_dialog.h"
#include "fielditem.h"
#include <random>
#include <QTimer>
#include <QResizeEvent>
#include <QGraphicsItem>

#define OBSTACLE_RADIUS 70

Dialog::Dialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Dialog),
  timer(new QTimer())
{
  ui->setupUi(this);
  auto s = this->size();
  auto w = s.width()-50;
  auto h = s.height()-50;
  ui->graphicsView->setScene(new QGraphicsScene(-w/2,-h/2,w,h,this));
  ui->graphicsView->setRenderHint(QPainter::Antialiasing);
  ui->graphicsView->setBackgroundBrush(Qt::green);
  field = new FieldItem();
  ui->graphicsView->scene()->addItem(field);
  ui->graphicsView->scale(0.5,0.5);
  connect(ui->clrbtn,&QPushButton::clicked,[=]
  {
    this->clearObstacle();
  });
  connect(ui->addbtn,&QPushButton::clicked,[=]
  {
    auto x = ui->xsbox->value();
    auto y = ui->ysbox->value();
    addObstacle(x,y);
  });
  connect(this->timer,&QTimer::timeout,[=]
  {
    for(size_t i=0; i<noise_obs.size(); i++) {
      this->noise_list.at(i)->setPos(noise_obs.at(i).first, noise_obs.at(i).second);
    }
    this->ui->graphicsView->scene()->update();
  });
  this->field->move_cb = [&](double,double) {
    auto click = this->field->getClickPos();
    auto release = this->field->getReleasePos();
    moveObstacle(release.first,release.second,release.first,release.second);
  };
  //  connect(this,&Dialog::updateNoisyObs,[=](int i, double x, double y)
  //  {
  //     this->noise_list.at(i)->setPos(x,y);
  //  });
  connect(ui->lfreesbox,SIGNAL(valueChanged(double)),this,SIGNAL(updateLFree(double)));
  connect(ui->loccupiedsbox,SIGNAL(valueChanged(double)),this,SIGNAL(updateLOccupied(double)));

  thd._dialog = this;
  //  thd.update_noise_fn = [&](int i, double x, double y)
  //  {
  //    emit updateNoisyObs(i,x,y);
  //  };

  thd._obs_true = &this->obs_true;
  thd._obs_target = &this->obs_target;
  thd._obs_noise = &this->noise_obs;
  thd.start();
  timer->start(100); // update gui every 100 ms
  setWindowTitle("True Position");
}

std::vector<std::pair<double, double> > Dialog::obstacle()
{
  /*
    std::vector<std::pair<double,double>> ret;
    for(const auto& o : obs_list)
    {
        auto x = o->pos().x();
        auto y = o->pos().y();
        ret.push_back(std::make_pair(x,y));
    }
    return ret;
    */
  return noise_obs;
}

Dialog::~Dialog()
{
  thd.end();
  delete ui;
}

void Dialog::addObstacle(double x, double y)
{
  auto px = x-OBSTACLE_RADIUS/2.0;
  auto py = y-OBSTACLE_RADIUS/2.0;
  auto obs = ui->graphicsView->scene()->addEllipse(px,py,OBSTACLE_RADIUS,OBSTACLE_RADIUS,QPen(Qt::black),QBrush(Qt::black));
  auto noisy_obs = ui->graphicsView->scene()->addEllipse(px,py,OBSTACLE_RADIUS,OBSTACLE_RADIUS,QPen(Qt::gray),QBrush(Qt::gray));
  obs_list.push_back(obs);
  noise_list.push_back(noisy_obs);
  auto p = std::make_pair(px+OBSTACLE_RADIUS/2.0,py+OBSTACLE_RADIUS/2.0);
  noise_obs.push_back(p);
  obs_true.push_back(p);
}

void Dialog::moveObstacle(double x, double y, double xf, double yf)
{
  QGraphicsItem* near = nullptr;
  std::pair<double,double>* near_pair = nullptr;
  for(size_t i=0; i<obs_list.size(); i++)
  {
    auto o = obs_list.at(i);
    auto dis = std::hypot((x)-(o->pos().x()),
                          (y)-(o->pos().y()));
    if(dis<OBSTACLE_RADIUS){
      if(!near){
        near = o;
        near_pair = &(obs_true.at(i));
      }
      else
      {
        auto d = std::hypot((x)-(near->pos().x()),
                            (y)-(near->pos().y()));
        if(d<dis){
          near = o;
          near_pair = &(obs_true.at(i));
        }
      }
    }
  }
  if(near)
  {
    near->setPos(xf,yf);
    near_pair->first = xf;
    near_pair->second = yf;
  }
}

void Dialog::clearObstacle()
{
  for(auto o : obs_list)
  {
    ui->graphicsView->scene()->removeItem(o);
  }
  for(auto o : noise_list) {
    ui->graphicsView->scene()->removeItem(o);
  }
  obs_list.clear();
  noise_list.clear();
  noise_obs.clear();
  obs_true.clear();
}

void Dialog::resizeEvent(QResizeEvent *event)
{
  auto s = event->size();
  auto w = s.width() - 50;
  auto h = s.height() - 50;
  this->ui->graphicsView->setSceneRect(-w/2,-h/2,w,h);
}

void Dialog::Thread::start()
{
  if((!_dialog) || (!_obs_true) || (!_obs_target))
    return;
  running = true;
  thread = std::thread(&Thread::loop,this);
}

void Dialog::Thread::loop()
{
  std::normal_distribution<double> noise(0.0,10);
  std::random_device rd;
  std::mt19937 engine(rd());
  while(running)
  {
    for(int i=0; i<_obs_true->size(); i++)
    {
      auto ex = noise(engine);
      auto ey = noise(engine);
      auto& o = _obs_true->at(i);
      auto x = o.first+ex;
      auto y = o.second+ey;
      _obs_noise->at(i) = std::make_pair(x,y);
      if(update_noise_fn)
        update_noise_fn(i,_obs_noise->at(i).first,_obs_noise->at(i).second);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(66));
  }
}

void Dialog::Thread::end()
{
  running = false;
  thread.join();
}
