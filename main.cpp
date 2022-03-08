#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
/*
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
#error You must use Qt 5.14 or later
#endif*/
    QApplication a(argc, argv);
    a.setApplicationDisplayName("MarkdownEdit");
    a.setApplicationVersion(APP_VERSION);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "MarkdownEdit_" + QLocale(locale).name().split("_")[0];
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}
