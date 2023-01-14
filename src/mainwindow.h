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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore/qcontainerfwd.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
};
class QPrinter;
class QSettings;
class QToolButton;
class Highliter;
class QComboBox;
class QFileSystemWatcher;
class MarkdownEditor;
class QShortcut;
class QWidgetAction;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QStringList &, QWidget *parent = nullptr);
    ~MainWindow() override;

    void openFile(const QString &, const QString & = QLatin1String());
    void openFiles(const QStringList &);

protected:
    void closeEvent(QCloseEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;

public Q_SLOTS:
    void receivedMessage(const quint32, const QByteArray &);

    void toForeground();

    void onHelpSyntax();

    inline void onOpenFile(const QString &file) { openFile(file); };

private Q_SLOTS:
    void onFileNew();
    void onFileOpen();
    bool onFileSave();
    bool onFileSaveAs();
    void onFileReload();

    void onImportHTML();

    void loadIcons();

    void onFileChanged(const QString &);

    void onModificationChanged(const bool);

    void loadSettings();

    void setupThings();
    void setupConnections();

    void setupToolbar();

    void onEditorChanged(const int);
    void closeEditor(const int);
    void editorMoved(const int, const int);
    void closeCurrEditor();

    void onHelpAbout();

    void onTextChanged();
    void changeMode(const int);

    void exportHtml();
    void exportPdf();

    void openInWebBrowser();

    void filePrint();
    void filePrintPreview();
    void printPreview(QPrinter *);

    void changeHighlighting(const bool);
    void changeSpelling(const bool);
    void pausePreview(const bool);
    void disablePreview(const bool);
    void changeAddtoIconPath(const bool);
    void changeWordWrap(const bool);

    void onOrientationChanged(const Qt::ScreenOrientation);

    void cut();
    void copy();
    void paste();
    void selectAll();

    void undo();
    void redo();

    void insertTable();
    void insertTableOfContents();
    /*
    void insertLink();
    void insertImage();
    */

    void bold();
    void italic();
    void underline();
    void strikethrough();

    void insertLongText(const QString &, const bool = false);
    void inserText(QString what, const bool h = false);

    void onUrlClicked(const QString &);

    // Android specific
    void androidPreview(const bool);

private:
    void loadFiles(const QStringList &);
    void saveSettings();
    void updateOpened();
    void openRecent();
    void setText(const int);

    static void loadIcon(const QString &name, QAction *a);
    static void loadIcon(const QString &name, QMenu *m);

    auto createEditor() -> MarkdownEditor *;
    auto currentEditor() -> MarkdownEditor *;
    QList<MarkdownEditor *> editorList;
    QStringList fileList;

    QShortcut *shortcutNew;
    QShortcut *shortcutClose;

    Ui::MainWindow *ui;

    QString path;
    short _mode = 1; // 0 = Commonmark, 1 = GitHub

    QSettings *settings;

    QStringList recentOpened;

    QString html;

    bool dontUpdate = false;
    bool setPath = false;
    bool spelling = true;
    bool highlighting = true;
    bool preview = true;

    Qt::ScreenOrientation orientation = Qt::LandscapeOrientation;

    bool overrideEditor = false;
    short overrideVal = 0;

    bool bigFile = false;

    Highliter *htmlHighliter = nullptr;

    QToolButton *toolbutton = nullptr;
    QComboBox *widgetBox = nullptr;
    QAction *actionWidgetBox = nullptr;
    QAction *actionPreview = nullptr;

    QComboBox *mode = nullptr;
    QTimer *timer = nullptr;

    QFileSystemWatcher *watcher = nullptr;
};
#endif // MAINWINDOW_H
