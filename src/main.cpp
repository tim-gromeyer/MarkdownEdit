/**
 ** This file is part of the MarkdownEdit project.
 ** Copyright 2022 Tim Gromeyer <sakul8825@gmail.com>.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/


#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QLocale>
#include <QTranslator>

#ifndef NOT_SUPPORTET
#include "singleapplication.h"
#endif

#if defined(Q_OS_WASM) && QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
#error You must use Qt 5.14 or newer // Because of the file dialog
#elif QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
#error You must use Qt 5.10 or newer // Because of QGuiApplication::screenAt
#endif

#define S(str) QStringLiteral(str)


auto main(int argc, char *argv[]) -> int
{
#ifndef NOT_SUPPORTET
    SingleApplication a(argc, argv, true, SingleApplication::Mode::SecondaryNotification);
#else
    QApplication a(argc, argv);
#endif
    QApplication::setApplicationVersion(QStringLiteral(APP_VERSION));

    QTranslator translator, qtTranslator;

    // load translation for Qt
    if (qtTranslator.load(QLocale::system(), S("qtbase"),
                          S("_"), S(":/qtTranslations/"))) {
        QApplication::installTranslator(&qtTranslator);
    }

    // try to load translation for current locale from resource file
    if (translator.load(QLocale::system(), S("MarkdownEdit"),
                        S("_"), S(":/translations"))) {
        QApplication::installTranslator(&translator);
    }

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
    if (a.isSecondary()) {
        a.sendMessage(QByteArrayLiteral("file://") %
                          parser.positionalArguments().join(u' ').toLatin1());
        return 0;
    }
#endif

    MainWindow w(parser.positionalArguments());

#ifndef NOT_SUPPORTET
    QObject::connect(
        &a, &SingleApplication::instanceStarted,
        &w, &MainWindow::toForeground
    );

    QObject::connect(&a, &SingleApplication::receivedMessage,
        &w, &MainWindow::receivedMessage
    );
#endif


#ifdef Q_OS_ANDROID
    w.showFullScreen();
    // Prevent blank window
    a.processEvents();
#else
    w.show();
#endif

    return QApplication::exec();
}
