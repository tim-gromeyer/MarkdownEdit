#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QCommandLineParser>


int main(int argc, char *argv[])
{
#if defined(Q_OS_WASM) && QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
#error You must use Qt 5.14 or newer
#elif QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
#error You must use Qt 5.10 or newer
#endif


    QApplication a(argc, argv);
    a.setApplicationDisplayName(QStringLiteral("MarkdownEdit"));
    a.setApplicationVersion(APP_VERSION);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription(QCoreApplication::translate(
        "main", "Simple program for editing Markdown files"

        ));
    parser.addPositionalArgument("file", QCoreApplication::translate(
                                             "main", "File to open."));
    parser.process(a);

    QTranslator translator, qtTranslator;

    // load translation for Qt
    if (qtTranslator.load(QLocale::system(), QStringLiteral("qtbase"),
                          QStringLiteral("_"), QStringLiteral(
                              ":/qtTranslations/")))
        a.installTranslator(&qtTranslator);

    // try to load translation for current locale from resource file
    if (translator.load(QLocale::system(), QStringLiteral("MarkdownEdit"),
                        QStringLiteral("_"), QStringLiteral(":/translations")))
        a.installTranslator(&translator);

    MainWindow w(parser.positionalArguments().value(0));

    w.show();

    return a.exec();
}
