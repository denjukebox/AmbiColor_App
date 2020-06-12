#ifndef SIMPLERECTANGLE_H
#define SIMPLERECTANGLE_H

#include <QWidget>
#include <QtGui>

class SimpleRectangle : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleRectangle();
    ~SimpleRectangle();

    void setColors(int red, int green,int blue);

protected:
    void paintEvent(QPaintEvent *event);

private:
    int R;
    int G;
    int B;

    QRect _rectangle;
    QPainter _painter;
};

#endif // SIMPLERECTANGLE_H
