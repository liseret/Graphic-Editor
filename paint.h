#ifndef PAINT_H
#define PAINT_H

#include <qpoint.h>
#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QRect>
#include <QVector>
#include <QDir>


class Tool
{
protected:
    QPoint StartPoint;
    QPoint EndPoint;
    QColor PenColor;
    int PenWidth;

public:
    enum Type {pen,line,rectangle,ellips,bruh};

    Tool(const QColor &color, int width) : PenColor(color), PenWidth(width) {};
    virtual ~Tool() = default;
    virtual void draw(QPainter &painter) = 0;
    virtual Type GetType() const = 0;

    void SetStartPoint(const QPoint &point){
        StartPoint = point;
    }
    void SetEndPoint(const QPoint &point){
        EndPoint = point;
    }
    void SetPenColor(const QColor &color){
        PenColor = color;
    }
    void SetPenWidth(int width){
        PenWidth = width;
    }
};
class Bruh:public Tool{
private:
    QImage BrImg;
public:
    Bruh(const QColor &color, int width, const QImage &image):Tool(color, width), BrImg(image) {};
    void draw(QPainter &painter) override {
    }
    Type GetType() const override{
        return bruh;
    }
    void SetBrushImage(const QImage &image) {
        BrImg = image;
    }
};


class Pen:public Tool{
private:
    QVector<QPoint> Points;
public:
    Pen(const QColor &color, int width) : Tool(color,width) {};

    void draw(QPainter &painter) override {
        painter.setPen(QPen(PenColor, PenWidth,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        for (int i=1;i<Points.size();++i) {
            painter.drawLine(Points[i-1],Points[i]);
        }
    }
    Type GetType() const override{
        return pen;
    }
    void addPoint(const QPoint &point){
        Points.append(point);
    }
    void clearPoints() {
        Points.clear();
    }

};

class Line:public Tool{
public:
    Line(const QColor &color,int width) : Tool(color,width) {};
    void draw(QPainter &painter) override {
        painter.setPen(QPen(PenColor,PenWidth,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        painter.drawLine(StartPoint,EndPoint);
    }
    Type GetType() const override {
        return line;
    }
};
class Rectangle:public Tool{
public:
    Rectangle(const QColor &color,int width):Tool(color,width) {};

    void draw(QPainter &painter) override {
        painter.setPen(QPen(PenColor,PenWidth,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        QRect rect=QRect(StartPoint, EndPoint).normalized();
        painter.drawRect(rect);
    }

    Type GetType() const override {
        return rectangle;
    }
};
class Ellips:public Tool{
public:
    Ellips(const QColor &color,int width):Tool(color,width) {};
    void draw(QPainter &painter) override{
        painter.setPen(QPen(PenColor,PenWidth,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
        QRect rect=QRect(StartPoint,EndPoint).normalized();
        painter.drawEllipse(rect);
    }
    Type GetType() const override {
        return ellips;
    }
};
class Esear:public Tool{

};
class EditControl{
public:
    Tool* createShape(Tool::Type type,const QColor &color,int width) {
        if (type==Tool::pen){
            return new Pen(color,width);
        }
        else if (type==Tool::line){
            return new Line(color,width);
        }
        else if (type==Tool::rectangle){
            return new Rectangle(color,width);
        }
        else if (type==Tool::ellips){
            return new Ellips(color,width);
        }
        return nullptr;
    }
};

class Canvas: public QWidget{
    Q_OBJECT
protected:
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void paintEvent(QPaintEvent *ev) override;
    void resizeEvent(QResizeEvent *ev) override;

private:
    void DrawLineTo(const QPoint &endPoint);
    void ResizeImage(QImage *image,const QSize &newSize);
    void DrawCurrentShape();
    bool Modified;
    bool PaintOrNot;
    int MyPenWidth;
    QColor MyPenColor;
    QImage Image;
    Tool* CurrentShape;
    QPoint LastPoint;
    EditControl ShapeControl;
public:
    Canvas(QWidget *parent=0);
    ~Canvas();
    bool SaveImage(const QString &fileName,const char *fileFormat);
    void SetPenColor(const QColor &newColor);
    void SetPenWidth(int newWidth);
    void SetCurrentShape(Tool::Type shapeType);

    bool IsModified() const{
        return Modified;
    }
    QColor PenColor() const{
        return MyPenColor;
    }
    int PenWidth() const{
        return MyPenWidth;
    }

public slots:
    void ClearImage();


};
class Layer{

};
class Stack{

};

#endif // PAINT_H
