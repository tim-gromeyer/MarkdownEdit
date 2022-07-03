#include "mainwindow.h"
#include "common.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QCommandLineParser>


#if defined(Q_OS_WASM) || defined(Q_OS_ANDROID)
#define NOT_SUPPORTET
#else
#include "singleapplication.h"
#endif


int main(int argc, char *argv[])
{
#if defined(Q_OS_WASM) && QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
#error You must use Qt 5.14 or newer
#elif QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
#error You must use Qt 5.10 or newer
#endif

#ifndef NOT_SUPPORTET
    SingleApplication a(argc, argv, true);
#else
    QApplication a(argc, argv);
#endif
    a.setApplicationDisplayName(QStringLiteral("MarkdownEdit"));
    a.setApplicationVersion(APP_VERSION);

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

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription(QCoreApplication::translate(
        "main", "Simple program for editing Markdown files"

        ));
    parser.addPositionalArgument("file", QCoreApplication::translate(
                                             "main", "File to open."));
    parser.process(a);

#ifndef NOT_SUPPORTET
    if(a.isSecondary()) {
        a.sendMessage(QByteArrayLiteral("file://") +
                      parser.positionalArguments().value(0).toLocal8Bit());
        return 0;
    }
#endif

    MainWindow w(parser.positionalArguments().value(0));

#ifndef NOT_SUPPORTET
    QObject::connect(
        &a,
        &SingleApplication::instanceStarted,
        &w,
        &QMainWindow::raise
    );

    QObject::connect(
        &a,
        &SingleApplication::receivedMessage,
        &w,
        &MainWindow::receivedMessage
    );
#endif

    w.show();

    return a.exec();
}
