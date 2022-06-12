#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>
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
    parser.addPositionalArgument(QStringLiteral("File(s)"), QStringLiteral("The file(s) to open."));
    parser.process(a);

    QTranslator translator, qtTranslator;
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    const QString path = QLibraryInfo::path(QLibraryInfo::TranslationsPath);
#else
    const QString path = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
#endif

    const QLocale &lang = QLocale::system();

    // load translation for Qt
    if (qtTranslator.load(lang, QStringLiteral("qt"),
                          QStringLiteral("_"), path))
        a.installTranslator(&qtTranslator);

    // try to load translation for current locale from resource file
    if (translator.load(lang, QStringLiteral("MarkdownEdit"),
                        QStringLiteral("_"), QStringLiteral(":/i18n")))
        a.installTranslator(&translator);

    MainWindow w(parser.positionalArguments().value(0, QLatin1String()));

    w.show();

    return a.exec();
}
