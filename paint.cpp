#include "paint.h"
#include <QMouseEvent>

Canvas::Canvas(QWidget *parent):QWidget(parent){
    setAttribute(Qt::WA_StaticContents);
    Modified=false;
    PaintOrNot=false;
    MyPenWidth=1;
    MyPenColor=Qt::black;
    CurrentShape=ShapeControl.createShape(Tool::pen,MyPenColor,MyPenWidth);
}

Canvas::~Canvas(){
    delete CurrentShape;
}

void Canvas::SetCurrentShape(Tool::Type shapeType){
    delete CurrentShape;
    CurrentShape=ShapeControl.createShape(shapeType,MyPenColor,MyPenWidth);
}

bool Canvas::SaveImage(const QString &fileName,const char *fileFormat){
    QImage visibleImage=Image;
    ResizeImage(&visibleImage, size());

    if(visibleImage.save(fileName, fileFormat)){
        Modified = false;
        return true;
    }
    else{
        return false;
    }
}

void Canvas::SetPenColor(const QColor &newColor){
    MyPenColor=newColor;
    if(CurrentShape){
        CurrentShape->SetPenColor(newColor);
    }
}

void Canvas::SetPenWidth(int newWidth){
    MyPenWidth=newWidth;
    if (CurrentShape){
        CurrentShape->SetPenWidth(newWidth);
    }
}

void Canvas::ClearImage(){
    Image.fill(qRgb(255, 255, 255));
    Modified=true;
    update();
}

void Canvas::mousePressEvent(QMouseEvent *ev){
    if (ev->button()==Qt::LeftButton) {
        LastPoint=ev->pos();
        if(CurrentShape){
            CurrentShape->SetStartPoint(ev->pos());
            if(CurrentShape->getType()==Tool::pen) {
                Pen* pen=static_cast<Pen*>(CurrentShape);
                pen->addPoint(ev->pos());
            }
        }
        PaintOrNot=true;
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *ev){
    if ((ev->buttons() & Qt::LeftButton) && PaintOrNot && CurrentShape) {
        if(CurrentShape->getType() == Tool::pen){
            DrawLineTo(ev->pos());
            Pen* pen=static_cast<Pen*>(CurrentShape);
            pen->addPoint(ev->pos());
        }
        else{
            CurrentShape->SetEndPoint(ev->pos());
            update();
        }
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *ev){
    if (ev->button()==Qt::LeftButton && PaintOrNot && CurrentShape) {
        if (CurrentShape->getType()==Tool::pen) {
            DrawLineTo(ev->pos());
            Pen* pen=static_cast<Pen*>(CurrentShape);
            pen->addPoint(ev->pos());
        }
        else{
            CurrentShape->SetEndPoint(ev->pos());
            DrawCurrentShape();
        }
        PaintOrNot = false;
    }
}

void Canvas::paintEvent(QPaintEvent *ev){
    QPainter painter(this);
    QRect dirtyRect=ev->rect();
    painter.drawImage(dirtyRect,Image,dirtyRect);
    if (PaintOrNot && CurrentShape && CurrentShape->getType()!=Tool::pen) {
        CurrentShape->draw(painter);
    }
}

void Canvas::DrawCurrentShape(){
    QPainter painter(&Image);
    if (CurrentShape){
        CurrentShape->draw(painter);
        Modified=true;
        update();
    }
}

void Canvas::resizeEvent(QResizeEvent *ev){
    if ((width()> Image.width()) ||( height()> Image.height())) {
        int newWidth = qMax(width()+128,Image.width());
        int newHeight = qMax(height()+128,Image.height());
        ResizeImage(&Image, QSize(newWidth,newHeight));
        update();
    }
    QWidget::resizeEvent(ev);
}

void Canvas::DrawLineTo(const QPoint &endPoint){
    QPainter painter(&Image);
    painter.setPen(QPen(MyPenColor,MyPenWidth,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
    painter.drawLine(LastPoint,endPoint);
    Modified=true;
    int rad = (MyPenWidth/2)+2;
    update(QRect(LastPoint, endPoint).normalized().adjusted(-rad,-rad,+rad,+rad));
    LastPoint=endPoint;
}

void Canvas::ResizeImage(QImage *image,const QSize &newSize){
    if (image->size()==newSize)
        return;
    QImage newImage(newSize,QImage::Format_RGB32);
    newImage.fill(qRgb(255,255,255));
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0),*image);
    *image=newImage;
}
