#include "document.h"
#include "mainwindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationDisplayName("Markdown Editor");
    a.setApplicationVersion(APP_VERSION);

    MainWindow window;
    window.show();

    return a.exec();
}
