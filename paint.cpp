#include "paint.h"
#include <QtWidgets>

Canvas::Canvas(QWidget *parent):QWidget(parent){
    setAttribute(Qt::WA_StaticContents);
    Modified=false;
    PaintOrNot=false;
    MyPenWidth=1;
    MyPenColor=Qt::black;
    CurrentShape=ShapeControl.createShape(Tool::pen,MyPenColor,MyPenWidth);
    BruhIndex = 0;
    LoadBruh();
}

Canvas::~Canvas(){
    delete CurrentShape;
}

void Canvas::SetCurrentShape(Tool::Type shapeType){
    delete CurrentShape;
    CurrentShape=ShapeControl.createShape(shapeType,MyPenColor,MyPenWidth);
    if (shapeType==Tool::bruh && !BruhImag.isEmpty()){
        Bruh* Brush = static_cast<Bruh*>(CurrentShape);
        Brush->SetBrushImage(BruhImag[BruhIndex]);
}
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
            if(CurrentShape->GetType()==Tool::pen) {
                Pen* pen=static_cast<Pen*>(CurrentShape);
                pen->addPoint(ev->pos());
            }
            else if (CurrentShape->GetType() == Tool::bruh) {
                DrawBrush(ev->pos());
            }
        }
        PaintOrNot=true;
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *ev){
    if ((ev->buttons() & Qt::LeftButton) && PaintOrNot && CurrentShape) {
        if(CurrentShape->GetType() == Tool::pen){
            DrawLineTo(ev->pos());
            Pen* pen=static_cast<Pen*>(CurrentShape);
            pen->addPoint(ev->pos());
        }
        else if (CurrentShape->GetType() == Tool::bruh) {

            CurrentShape->SetStartPoint(ev->pos());
            DrawBrush(ev->pos());
        }
        else{
            CurrentShape->SetEndPoint(ev->pos());
            update();
        }
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *ev){
    if (ev->button()==Qt::LeftButton && PaintOrNot && CurrentShape) {
        if (CurrentShape->GetType()==Tool::pen) {
            DrawLineTo(ev->pos());
            Pen* pen=static_cast<Pen*>(CurrentShape);
            pen->addPoint(ev->pos());
        }
        else if (CurrentShape->GetType() == Tool::bruh) {
            DrawBrush(ev->pos());
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
    if (PaintOrNot && CurrentShape && CurrentShape->GetType() != Tool::pen && CurrentShape->GetType() != Tool::bruh) {
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
void Canvas::LoadBruh(){
    BruhImag.clear();

    QDir brushesDir("brushes");
    if (!brushesDir.exists()){
        brushesDir.mkpath(".");
        qDebug() << "Created brushes directory:" << brushesDir.absolutePath();
        return;
    }

    QStringList filters;
    filters<<"*.png";
    brushesDir.setNameFilters(filters);

    QStringList imageFiles = brushesDir.entryList(QDir::Files);

    for (const QString &fileName : imageFiles) {
        QString filePath = brushesDir.absoluteFilePath(fileName);
        QImage BrushImage(filePath);
        if (!BrushImage.isNull()) {
            BrushImage = BrushImage.convertToFormat(QImage::Format_ARGB32);
            BruhImag.append(BrushImage);
        }
    }

    if (BruhImag.isEmpty()) {
        return;
    }

}
void Canvas::DrawBrush(const QPoint &point)
{
    if (!CurrentShape || CurrentShape->GetType() != Tool::bruh)
        return;

    QPainter painter(&Image);
    CurrentShape->SetStartPoint(point);
    CurrentShape->draw(painter);
    Modified = true;

    int brushSize = qMax(10, MyPenWidth * 5);
    QRect updateRect(point.x() - brushSize/2, point.y() - brushSize/2,brushSize, brushSize);
    update(updateRect);
}
void Canvas::SetBrushIndex(int i)
{
    if (i >= 0 && i < BruhImag.size()) {
        BruhIndex = i;
        if (CurrentShape && CurrentShape->GetType() == Tool::bruh) {
            Bruh* brush = static_cast<Bruh*>(CurrentShape);
            brush->SetBrushImage(BruhImag[BruhIndex]);
        }
        update();
    }
}
QImage Canvas::GetBrushImage(int i) const
{
    if (i >= 0 && i < BruhImag.size()) {
        return BruhImag[i];
    }
    return QImage();
}
