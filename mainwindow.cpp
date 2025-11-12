#include <QtWidgets>
#include "mainwindow.h"
#include "paint.h"

MainWindow::MainWindow(){
    canvas=new Canvas;
    setCentralWidget(canvas);
    CreateActions();
    CreateMenus();

    setWindowTitle(tr("Graphic editor"));
    resize(500,500);
}

void MainWindow::CreateActions(){
    for (QByteArray format:QImageWriter::supportedImageFormats()){
        QString text=tr("%1...").arg(QString(format).toUpper());
        QAction *action=new QAction(text,this);
        action->setData(format);
        connect(action,SIGNAL(triggered()),this,SLOT(Save()));
        SaveAsActs.append(action);
    }

    ExitAct=new QAction(tr("Exit"),this);
    ExitAct->setShortcuts(QKeySequence::Quit);
    connect(ExitAct,SIGNAL(triggered()),this,SLOT(close()));

    PenColorAct=new QAction(tr("Pen Color..."),this);
    connect(PenColorAct,SIGNAL(triggered()),this,SLOT(PenColor()));

    PenWidthAct=new QAction(tr("Pen Width..."),this);
    connect(PenWidthAct,SIGNAL(triggered()),this,SLOT(PenWidth()));

    ClearScreenAct=new QAction(tr("Clear Screen"),this);
    ClearScreenAct->setShortcut(tr("Ctrl+L"));
    connect(ClearScreenAct, SIGNAL(triggered()),canvas,SLOT(ClearImage()));

    PenToolAct=new QAction(tr("Pen"),this);
    PenToolAct->setCheckable(true);
    PenToolAct->setChecked(true);
    connect(PenToolAct,SIGNAL(triggered()),this,SLOT(SetPenTool()));

    LineToolAct=new QAction(tr("Line"),this);
    LineToolAct->setCheckable(true);
    connect(LineToolAct,SIGNAL(triggered()),this,SLOT(SetLineTool()));

    RectangleToolAct=new QAction(tr("Rectangle"),this);
    RectangleToolAct->setCheckable(true);
    connect(RectangleToolAct,SIGNAL(triggered()),this,SLOT(SetRectangleTool()));

    EllipsToolAct=new QAction(tr("Ellips"),this);
    EllipsToolAct->setCheckable(true);
    connect(EllipsToolAct,SIGNAL(triggered()),this,SLOT(SetEllipsTool()));

    ImageBrushToolAct = new QAction(tr("Image Brush"), this);
    ImageBrushToolAct->setCheckable(true);
    connect(ImageBrushToolAct, SIGNAL(triggered()), this, SLOT(SetImageBrushTool()));
    CreateBrushActions();
}
void MainWindow::CreateMenus(){
    SaveAsMenu=new QMenu(tr("Save As"),this);
    for (QAction *action:SaveAsActs) {
        SaveAsMenu->addAction(action);
    }

    FileMenu=new QMenu(tr("File"),this);
    FileMenu->addMenu(SaveAsMenu);
    FileMenu->addSeparator();

    ToolMenu=new QMenu(tr("Tools"),this);
    ToolMenu->addAction(PenToolAct);
    ToolMenu->addAction(LineToolAct);
    ToolMenu->addAction(RectangleToolAct);
    ToolMenu->addAction(EllipsToolAct);
    ToolMenu->addAction(ImageBrushToolAct);

    OptionMenu=new QMenu(tr("Options"),this);
    OptionMenu->addAction(PenColorAct);
    OptionMenu->addAction(PenWidthAct);
    OptionMenu->addSeparator();
    OptionMenu->addAction(ClearScreenAct);

    menuBar()->addMenu(FileMenu);
    menuBar()->addMenu(ToolMenu);
    menuBar()->addMenu(OptionMenu);
}

void MainWindow::SetPenTool(){
    canvas->SetCurrentShape(Tool::pen);
    PenToolAct->setChecked(true);
    LineToolAct->setChecked(false);
    RectangleToolAct->setChecked(false);
    EllipsToolAct->setChecked(false);
    ImageBrushToolAct->setChecked(false);
}

void MainWindow::SetLineTool(){
    canvas->SetCurrentShape(Tool::line);
    PenToolAct->setChecked(false);
    LineToolAct->setChecked(true);
    RectangleToolAct->setChecked(false);
    EllipsToolAct->setChecked(false);
    ImageBrushToolAct->setChecked(false);
}

void MainWindow::SetRectangleTool(){
    canvas->SetCurrentShape(Tool::rectangle);
    PenToolAct->setChecked(false);
    LineToolAct->setChecked(false);
    RectangleToolAct->setChecked(true);
    EllipsToolAct->setChecked(false);
    ImageBrushToolAct->setChecked(false);
}

void MainWindow::SetEllipsTool(){
    canvas->SetCurrentShape(Tool::ellips);
    PenToolAct->setChecked(false);
    LineToolAct->setChecked(false);
    RectangleToolAct->setChecked(false);
    EllipsToolAct->setChecked(true);
    ImageBrushToolAct->setChecked(false);
}
void MainWindow::SetImageBrushTool()
{
    canvas->SetCurrentShape(Tool::bruh);
    PenToolAct->setChecked(false);
    LineToolAct->setChecked(false);
    RectangleToolAct->setChecked(false);
    EllipsToolAct->setChecked(false);
    ImageBrushToolAct->setChecked(true);
    ShowBrushSelectionMenu();
}

void MainWindow::closeEvent(QCloseEvent *ev){
    if(MaybeSave()){
        ev->accept();
    }
    else{
        ev->ignore();
    }
}

void MainWindow::Save(){
    QAction *action=qobject_cast<QAction *>(sender());
    QByteArray FileFormat=action->data().toByteArray();
    SaveFile(FileFormat);
}

void MainWindow::PenColor(){
    QColor NewColor=QColorDialog::getColor(canvas->PenColor());
    if (NewColor.isValid())
        canvas->SetPenColor(NewColor);
}

void MainWindow::PenWidth(){
    bool ok;
    int a=QInputDialog::getInt(this,tr("Select width"),tr("Select pen width (from 1 to 100) :"),canvas->PenWidth(),1,100,1,&ok);
    if(ok)
        canvas->SetPenWidth(a);
}

bool MainWindow::MaybeSave(){
    if (canvas->IsModified()){
        auto a = QMessageBox::question(this,tr("Save Changes"),tr("The image has been modified.\nDo you want to save your changes?"),tr("Yep"),tr("Nope"),tr("Cancel"),0,2);
        if (a==0){
            return SaveFile("png");
        }
        else if(a==2) {
            return false;
        }
    }
    return true;
}

bool MainWindow::SaveFile(const QByteArray &fileFormat){
    QString Name = QDir::currentPath() + "/untitle." + fileFormat;
    QString FileName = QFileDialog::getSaveFileName(this, tr("Save As"),Name,tr("%1 Files (*.%2);;All Files (*)").arg(QString::fromLatin1(fileFormat.toUpper())).arg(QString::fromLatin1(fileFormat)));
    if(FileName.isEmpty()){
        return false;
    }
    else{
        return canvas->SaveImage(FileName, fileFormat.constData());
    }
}
void MainWindow::UpdateBrushesMenu()
{
    int currentIndex = canvas->GetCurrentBrushIndex();
    for (int i = 0; i < BrushActions.size(); ++i) {
        BrushActions[i]->setChecked(i == currentIndex);
    }
}
void MainWindow::SelectBrush()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        int brushIndex = action->data().toInt();
        canvas->SetBrushIndex(brushIndex);
        UpdateBrushesMenu();
    }
}


void MainWindow::CreateBrushActions(){
    for (QAction *a : BrushActions) {
        delete a;
    }
    BrushActions.clear();
    int all = canvas->GetBrushCount();
    int thisbr = canvas->GetCurrentBrushIndex();
    for (int i=0;i<all;++i){
        QString Name=tr("Brush %1").arg(i + 1);
        QAction *BrAct=new QAction(Name, this);
        BrAct->setData(i);
        BrAct->setCheckable(true);
        BrAct->setChecked(i==thisbr);
        connect(BrAct,SIGNAL(triggered()),this,SLOT(SelectBrush()));
        BrushActions.append(BrAct);
    }
}


void MainWindow::ShowBrushSelectionMenu(){
    QMenu BrMenu(this);
    int all=canvas->GetBrushCount();
    if (all==0){
        return;

    }
    else{
        if (BrushActions.isEmpty()) {
            CreateBrushActions();
        }
        else{
            UpdateBrushesMenu();
        }
        for (QAction *a : BrushActions) {
            BrMenu.addAction(a);
        }
    }
    BrMenu.exec(QCursor::pos());
}
