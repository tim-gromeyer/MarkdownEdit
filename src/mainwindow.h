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
class QHBoxLayout;
class QLabel;
class QDialogButtonBox;
class QShortcut;
template <typename Key, typename T> class QHash;
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QStringList &, QWidget *parent = nullptr);
    ~MainWindow();

    void openFile(const QString &);
    void openFiles(const QStringList &files);

protected:
    void closeEvent(QCloseEvent *e) override;

public slots:
    void receivedMessage(const qint32 &, const QByteArray &);

    void toForeground();

private slots:
    void onFileNew();
    void onFileOpen();
    bool onFileSave();
    bool onFileSaveAs();
    void onFileReload();

    void onFileChanged(const QString &);

    void setupThings();

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

    void onOrientationChanged(const Qt::ScreenOrientation &);

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
    void setText(const int);

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
