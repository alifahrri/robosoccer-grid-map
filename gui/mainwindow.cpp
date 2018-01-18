#include "mainwindow.h"
#include "fielditem.h"
#include "ui_mainwindow.h"
#include <QTimer>

MainWindow::MainWindow(GridMap &gridmap, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _gridmap(gridmap),
    _griditem(new GridItem(_gridmap)),
    _dialog(new Dialog(this)),
    _timer(new QTimer(this))
{
    ui->setupUi(this);
    auto s = this->size();
    auto w = s.width()-50;
    auto h = s.height()-50;
    ui->graphicsView->setScene(new QGraphicsScene(-w/2,-h/2,w,h,this));
    ui->graphicsView->scene()->addItem(new FieldItem());
    ui->graphicsView->scene()->addItem(_griditem);
    ui->graphicsView->setBackgroundBrush(Qt::green);

    connect(_timer,&QTimer::timeout,[=]
    {
        ui->graphicsView->scene()->update();
    });
    connect(_dialog,&Dialog::updateLFree,[=](double value)
    {
        _gridmap.set_lfree(value);
    });
    connect(_dialog,&Dialog::updateLOccupied,[=](double value)
    {
        _gridmap.set_loccupied(value);
    });
    _dialog->show();
    _timer->start(66);
    setWindowTitle(QString("Occupancy Grid Map"));
}

std::vector<std::pair<double, double> > MainWindow::obstacles()
{
    return _dialog->obstacle();
}

MainWindow::~MainWindow()
{
    delete ui;
}
