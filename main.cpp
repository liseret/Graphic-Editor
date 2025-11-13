#include "mainwindow.h"
#include <QApplication>
#include <QIcon>
#include <QDir>

int main(int argc, char *argv[]){

    QApplication app(argc, argv);
    QIcon icon(":/icons/app.png");
    app.setWindowIcon(icon);
    MainWindow window;
    window.show();
    return app.exec();
}
