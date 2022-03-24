#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>
#include <QCommandLineParser>


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
#error You must use Qt 5.4 or newer
#endif
    QApplication a(argc, argv);
    a.setApplicationDisplayName("MarkdownEdit");
    a.setApplicationVersion(APP_VERSION);

    QCommandLineParser parser;
    parser.setApplicationDescription(a.applicationDisplayName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("File", "The file to open.");
    parser.process(a);

    QTranslator translator;
    QTranslator qtTranslator;
    const QString path = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    bool confirm = true;

    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "MarkdownEdit_" + QLocale(locale).name().split("_")[0];
        const QString qtBaseName = "/qtbase_" + QLocale(locale).name().split("_")[0];
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            confirm = false;
        }
        if (qtTranslator.load(path + qtBaseName)) {
            a.installTranslator(&qtTranslator);
            if (!confirm)
                break;
        }
    }

    MainWindow w;

    QString file = parser.positionalArguments().value(0, QString(""));
    if (!file.isEmpty())
        w.openFile(file);

    w.show();
    return a.exec();
}
