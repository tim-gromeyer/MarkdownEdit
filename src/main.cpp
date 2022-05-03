#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>
#include <QCommandLineParser>


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
#error You must use Qt 5.10 or newer
#endif
    QApplication a(argc, argv);
    a.setApplicationDisplayName("MarkdownEdit");
    a.setApplicationVersion(APP_VERSION);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("File(s)", "The file(s) to open.");
    parser.process(a);

    QTranslator translator, qtTranslator;
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    const QString path = QLibraryInfo::path(QLibraryInfo::TranslationsPath);
#else
    const QString path = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
#endif

    QString lang;

    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        lang = QLocale(locale).name().split("_")[0];
        const QString baseName("MarkdownEdit_" + lang);
        const QString qtBaseName("/qtbase_" + lang);
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            if (qtTranslator.load(path + qtBaseName))
                a.installTranslator(&qtTranslator);
        }
    }

    MainWindow w;
    w.setLanguage(qAsConst(lang));

    const QString file(parser.positionalArguments().value(0, QLatin1String()));
    if (!file.isEmpty())
        w.openFile(file);

    w.show();
    return a.exec();
}
