#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <functional>

class GraphicsView : public QGraphicsView
{
public:
    typedef std::function<void(bool,int,int)> Callback;
    typedef std::function<void(double,double,double,double)> ReleaseCallback;
    typedef std::function<void(double,double)> DoubleClickCallback;
    typedef std::function<void(int)> KeyCallback;
public:
    GraphicsView(QWidget *parent=nullptr);
    Callback press_callback = nullptr;
    ReleaseCallback release_callback = nullptr;
    DoubleClickCallback double_click_callback = nullptr;
    KeyCallback keyCallback = nullptr;
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
private:
    QPointF press_pos;
};

#endif // GRAPHICSVIEW_H
