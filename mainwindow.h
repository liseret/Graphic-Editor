#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>

class Canvas;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void Save();
    void PenColor();
    void PenWidth();
    void SetPenTool();
    void SetLineTool();
    void SetRectangleTool();
    void SetEllipsTool();
    void SelectBrush();
    void SetImageBrushTool();
    void ShowBrushSelectionMenu();



private:
    void CreateActions();
    void CreateMenus();
    bool MaybeSave();
    bool SaveFile(const QByteArray &fileFormat);
    void UpdateBrushesMenu();
    void CreateBrushActions();

    Canvas *canvas;
    QMenu *SaveAsMenu;
    QMenu *FileMenu;
    QMenu *OptionMenu;
    QMenu *ToolMenu;
    QList<QAction *> SaveAsActs;
    QAction *ExitAct;
    QAction *PenColorAct;
    QAction *PenWidthAct;
    QAction *ClearScreenAct;
    QAction *PenToolAct;
    QAction *LineToolAct;
    QAction *RectangleToolAct;
    QAction *EllipsToolAct;
    QAction *ImageBrushToolAct;
    QList<QAction *> BrushActions;
};

#endif
