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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QPrinter;
class QSettings;
class QToolButton;
class Highliter;
class QComboBox;
class QFileSystemWatcher;
class MarkdownEditor;
class QShortcut;
template <typename Key, typename T> class QHash;
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QStringList &, QWidget *parent = nullptr);
    ~MainWindow();

    void openFile(const QString &, const QString & = QLatin1String());
    void openFiles(const QStringList &);

protected:
    void closeEvent(QCloseEvent *e) override;

public slots:
    void receivedMessage(const qint32, const QByteArray &);

    void toForeground();

    void onHelpSyntax();

private slots:
    void onFileNew();
    void onFileOpen();
    bool onFileSave();
    bool onFileSaveAs();
    void onFileReload();

    void onFileChanged(const QString &);

    void onModificationChanged(const bool);

    void setupThings();
    void setupConnections();

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

private:
    void loadSettings();
    void loadFiles(const QStringList &);
    void saveSettings();
    void updateOpened();
    void openRecent();
    void setText(const int = -1);

    void loadIcon(const QString &name, QAction* a);
    void loadIcons();

    MarkdownEditor *createEditor();
    MarkdownEditor *currentEditor();
    QList<MarkdownEditor*> editorList;
    QStringList fileList;

    QShortcut *shortcutNew;
    QShortcut *shortcutClose;

    Ui::MainWindow *ui;

    QString path;
    int _mode = 1;

    QSettings *settings;

    QStringList recentOpened;

    QString html;

    bool dontUpdate = false;
    bool setPath = false;
    bool spelling = true;
    bool highlighting = true;

    bool overrideEditor = false;
    int overrideVal = 0;

    Highliter *htmlHighliter;

    QToolButton *toolbutton;
    QComboBox *widgetBox;

    QComboBox *mode;

    QFileSystemWatcher *watcher;
};
#endif // MAINWINDOW_H
