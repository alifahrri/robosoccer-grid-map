#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "griditem.h"
#include "gridmap.h"
#include "dialog.h"

namespace Ui {
class MainWindow;
}

class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(GridMap& gridmap, QWidget *parent = 0);
    std::vector<std::pair<double,double>> obstacles();
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    GridMap &_gridmap;
    GridItem *_griditem;
    Dialog *_dialog;
    QTimer* _timer;
};

#endif // MAINWINDOW_H
