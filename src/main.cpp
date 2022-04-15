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
    parser.setApplicationDescription(a.applicationDisplayName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("File(s)", "The file(s) to open.");
    parser.process(a);

    QTranslator translator;
    QTranslator qtTranslator;
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    const QString path = QLibraryInfo::path(QLibraryInfo::TranslationsPath);
#else
    const QString path = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
#endif

    bool confirm = true;
    QString lang;

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
            if (!confirm) {
                lang = QLocale(locale).name().split("_")[0];
                break;
            }
        }
    }

    MainWindow w;
    w.setLanguage(lang);

    QString file = parser.positionalArguments().value(0, QString(""));
    if (!file.isEmpty())
        w.openFile(file);

    w.show();
    return a.exec();
}
