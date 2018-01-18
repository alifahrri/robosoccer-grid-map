#include "gridmap.h"
#include "mainwindow.h"
#include <random>
#include <chrono>
#include <thread>
#include <atomic>
#include <functional>
#include <QApplication>

struct GridThread
{
    typedef std::function<std::vector<std::pair<double,double>>()> ObstacleCallback;
    GridMap* gridmap;
    ObstacleCallback obs_cb = nullptr;
    std::thread thread;
    std::atomic_bool running;
    std::vector<std::pair<double,double>> obstacles;
    void start();
    void loop();
    void end();
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GridMap gridmap(9.0,6.0,0.1);
    GridThread gridthread;
    gridthread.gridmap = &gridmap;
    gridthread.start();
    MainWindow w(gridmap);
    gridthread.obs_cb = [&]
    {
        return w.obstacles();
    };
    w.show();

    auto ret = a.exec();
    gridthread.end();
    return ret;
}

void GridThread::start()
{
    running = true;
    thread = std::thread(&GridThread::loop,this);
}

void GridThread::loop()
{
    while(running)
    {
        if(obs_cb) obstacles = obs_cb();
        std::vector<GridMap::Point> obs;
        for(const auto& o : obstacles)
            obs.push_back({o.first/100.0,o.second/100.0});
        gridmap->updateObstacles(obs,0.35);
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }
}

void GridThread::end()
{
    running = false;
    thread.join();
}
