#include "simplerectangle.h"

SimpleRectangle::SimpleRectangle() : QWidget(),
    _rectangle(0,0,50,50),
    _painter(this)
{

    R = 0;
    G = 0;
    B = 0;
}

void SimpleRectangle::paintEvent(QPaintEvent *)
{
    _painter.begin(this);
    _painter.setBrush(QBrush(QColor(R,G,B,255)));
    _painter.drawRect(_rectangle);
    _painter.end();
}

void SimpleRectangle::setColors(int red, int green,int blue){
    R = red;
    G = green;
    B = blue;
}

SimpleRectangle::~SimpleRectangle()
{

}

