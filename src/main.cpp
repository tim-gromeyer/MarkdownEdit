/**
 ** This file is part of the MarkdownEdit project.
 ** Copyright 2022 - 2023 Tim Gromeyer <sakul8826@gmail.com>.
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
#include "spellchecker.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QLocale>
#include <QTranslator>

#include <thread>

#if SINGLEAPP
#include "singleapplication.h"
#endif

#if defined(Q_OS_WASM) && QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
#error You must use Qt 5.14 or newer // Because of the file dialog
#elif QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
#error You must use Qt 5.12 or newer // Because of QTextBlockFormat::headingLevel()
#endif

// Helper function for loading translator
bool loadTranslator(QTranslator &translator, const QString &name)
{
    if (translator.load(QLocale::system(),
                        name,
                        QStringLiteral("_"),
                        QStringLiteral(":/translations"))) {
        QApplication::installTranslator(&translator);
        return true;
    }
    return false;
}

auto main(int argc, char *argv[]) -> int
{
    std::thread t(&SpellChecker::populateLangMap);
    Q_INIT_RESOURCE(media);

#if SINGLEAPP
    SingleApplication a(argc, argv, true, SingleApplication::Mode::SecondaryNotification);
#else
    QApplication a(argc, argv);
#endif
    QApplication::setApplicationVersion(QStringLiteral(APP_VERSION));
    QApplication::setApplicationName(QStringLiteral("MarkdownEdit"));

    QTranslator translator, qtTranslator;

    // Load translation for Qt
    if (loadTranslator(qtTranslator, QStringLiteral("qtbase"))) {
        qInfo("Qt translations loaded successfully.");
    }

    // Load translation for the application
    if (loadTranslator(translator, QStringLiteral("MarkdownEdit"))) {
        qInfo("Application translations loaded successfully.");
    }

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription(
        translator.translate("main", "Simple program for editing Markdown files"));
    parser.addPositionalArgument(translator.translate("main", "Files"),
                                 translator.translate("main", "Files to open."));
    parser.process(a);

#if SINGLEAPP
    if (a.isSecondary()) {
        a.sendMessage(QByteArrayLiteral("file://")
                      % parser.positionalArguments().join(u' ').toLatin1());
        t.join();
        return 0;
    }
#endif
    t.join();

    MainWindow w(parser.positionalArguments());

#if SINGLEAPP
    QObject::connect(&a, &SingleApplication::instanceStarted, &w, &MainWindow::toForeground);

    QObject::connect(&a, &SingleApplication::receivedMessage, &w, &MainWindow::receivedMessage);
#endif

#ifdef Q_OS_ANDROID
    w.showFullScreen();

    // Prevent blank window
    QApplication::processEvents();
#else
    w.show();
#endif

    return QApplication::exec();
}
