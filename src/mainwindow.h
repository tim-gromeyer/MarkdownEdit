#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>


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
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &file, QWidget *parent = nullptr);
    ~MainWindow();

    void openFile(const QString &path);

protected:
    void closeEvent(QCloseEvent *e) override;

private slots:
    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    inline void onFileReload() { openFile(path); };

    void onFileChanged(const QString &);

    void onHelpAbout();
    void onTextChanged();
    void changeMode(const int &);

    void exportHtml();
    void exportPdf();

    void openInWebBrowser();

    void filePrint();
    void filePrintPreview();
    void printPreview(QPrinter *);

    void changeHighlighting(const bool &);
    void changeSpelling(const bool &);
    void pausePreview(const bool &);
    void disablePreview(const bool &);
    void changeAddtoIconPath(const bool &);
    void changeWordWrap(const bool &);

    void onOrientationChanged(const Qt::ScreenOrientation &);

    void cut();
    void copy();
    void paste();

private:
    void loadSettings(const QString &);
    void saveSettings();
    void updateOpened();
    void openRecent();
    void onSetText(const int &);

    void loadIcon(const char* name, QAction* &action);
    void loadIcons();

    MarkdownEditor *currentEditor();
    QList<MarkdownEditor> editorList;

    Ui::MainWindow *ui;

    QString path;
    int _mode;

    QSettings *settings;

    QStringList recentOpened;

    QString html;
    QString spellLang;

    bool dontUpdate;
    bool setPath;
    bool spelling;
    bool highlighting;

    Highliter *htmlHighliter;

    QToolButton *toolbutton;
    QComboBox *widgetBox;

    QWidget *widgetReloadFile = nullptr;
    QHBoxLayout *horizontalLayout = nullptr;
    QLabel *labelReloadFile = nullptr;
    QDialogButtonBox *buttonBox = nullptr;

    QComboBox *mode;

    QMap<QString, QVariant> languagesMap;

    QFileSystemWatcher *watcher;
};
#endif // MAINWINDOW_H
