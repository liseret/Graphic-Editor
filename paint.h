#ifndef PAINT_H
#define PAINT_H

#include <qpoint.h>
class Canvas;
class Tool
{
public:
    virtual ~Tool()=default;
    virtual void OnMouseDown(Canvas&, const QPoint&)=0;
    virtual void OnMouseMuve(Canvas&, const QPoint&)=0;
    virtual void OnMouseUp(Canvas&, const QPoint&)=0;
};
class Brush:public Tool{

};
class Shape:public Tool{

};
class Line:public Shape{

};
class Rectangle:public Shape{

};
class Ellips:public Shape{

};
class Esear:public Tool{

};
class EditControl{

};
class Layer{

};
class Stack{

};

void LoadBrushes();
void SaveFile();
#endif // PAINT_H
