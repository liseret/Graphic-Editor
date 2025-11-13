#include "mainwindow.h"
#include <QApplication>
#include <QIcon>
#include <QDir>

int main(int argc, char *argv[]){

    QApplication app(argc, argv);
    QString iconPath = ":icons/icon.png";
    if (QFile::exists(iconPath)) {
        app.setWindowIcon(QIcon(iconPath));
    } else {
        // Fallback - попробовать относительный путь
        iconPath = "icons/icon.png";
        if (QFile::exists(iconPath)) {
            app.setWindowIcon(QIcon(iconPath));
        }
    }
    MainWindow window;
    window.show();
    return app.exec();
}
