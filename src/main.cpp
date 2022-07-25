#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QCommandLineParser>


#ifndef NOT_SUPPORTET
#include "singleapplication.h"
#endif

#if defined(Q_OS_WASM) && QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
#error You must use Qt 5.14 or newer
#elif QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
#error You must use Qt 5.10 or newer
#endif


int main(int argc, char *argv[])
{
#ifndef NOT_SUPPORTET
    SingleApplication a(argc, argv, true,
                        SingleApplication::Mode::SecondaryNotification);
#else
    QApplication a(argc, argv);
#endif
    a.setApplicationDisplayName(QStringLiteral("MarkdownEdit"));
    a.setApplicationVersion(APP_VERSION);

    static const QChar underscore[] = {
        QLatin1Char('_')
    };

    QTranslator translator, qtTranslator;

    // load translation for Qt
    if (qtTranslator.load(QLocale::system(), QStringLiteral("qtbase"),
                          QString::fromRawData(underscore, 1),
                          QStringLiteral(
                              ":/qtTranslations/")))
        a.installTranslator(&qtTranslator);

    // try to load translation for current locale from resource file
    if (translator.load(QLocale::system(), QStringLiteral("MarkdownEdit"),
                        QString::fromRawData(underscore, 1),
                        QStringLiteral(":/translations")))
        a.installTranslator(&translator);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription(translator.translate(
        "main", "Simple program for editing Markdown files"

        ));
    parser.addPositionalArgument(translator.translate("main", "Files"),
                                 translator.translate(
                                             "main", "Files to open."));
    parser.process(a);

#ifndef NOT_SUPPORTET
    if(a.sendMessage(QByteArrayLiteral("file://") +
                  parser.positionalArguments().join(' ').toLatin1()))
        return 0;
#endif

    MainWindow w(parser.positionalArguments());

#ifndef NOT_SUPPORTET
    QObject::connect(
        &a,
        &SingleApplication::instanceStarted,
        &w,
        &MainWindow::toForeground
    );

    QObject::connect(
        &a,
        &SingleApplication::receivedMessage,
        &w,
        &MainWindow::receivedMessage
    );
#endif

    w.show();
    a.processEvents(); // Startup looks smoother

    return a.exec();
}
