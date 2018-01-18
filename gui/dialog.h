#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <atomic>
#include <thread>
#include <functional>

namespace Ui {
class Dialog;
}

class QGraphicsItem;
class QTimer;
class FieldItem;

class Dialog : public QDialog
{
    Q_OBJECT

    struct Thread
    {
        Dialog* _dialog = nullptr;
        std::thread thread;
        std::atomic_bool running;
        // std::vector<QGraphicsItem *> *_obs_true;
        // std::vector<QGraphicsItem *> *_obs_noisy;
        std::vector<std::pair<double,double>> *_obs_true;
        std::vector<std::pair<double,double>> *_obs_noise;
        std::vector<std::pair<double,double>> *_obs_target;
        std::function<void(int,double,double)> update_noise_fn = nullptr;
        void start();
        void loop();
        void end();
    };

public:
    explicit Dialog(QWidget *parent = 0);
    std::vector<std::pair<double,double>> obstacle();
    ~Dialog();

private:
    void addObstacle(double x, double y);
    void moveObstacle(double x, double y, double xf, double yf);
    void clearObstacle();
    void resizeEvent(QResizeEvent *event);

private:
    Ui::Dialog *ui;
    std::vector<QGraphicsItem*> obs_list;
    std::vector<QGraphicsItem*> noise_list;
    std::vector<std::pair<double,double>> obs_target;
    std::vector<std::pair<double,double>> noise_obs;
    std::vector<std::pair<double,double>> obs_true;
    QTimer *timer;
    FieldItem *field;
    Thread thd;

signals:
    void updateNoisyObs(int,double,double);
    void updateLFree(double);
    void updateLOccupied(double);
};

#endif // DIALOG_H
