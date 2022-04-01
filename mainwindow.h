#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QPrinter;
class QSettings;
class QTimer;
namespace QtSpell { class TextEditChecker; }
class QTextDocument;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void openFile(const QString &path);

    void setLanguage(const QString lang) { language = lang; };

protected:
    void closeEvent(QCloseEvent *e) override;

private slots:
    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    void onHelpAbout();
    void onTextChanged();
    void changeMode(const QString &);
    void exportHtml(QString file = "");

    void filePrint();
    void filePrintPreview();
    void printPreview(QPrinter *);

    void changeHighlighting(bool enabled);
    void changeSpelling(bool checked);
    void pausePreview(bool checked);
    void disablePreview(bool checked);

    void settingsDialog();

    void undo();
    void redo();

    void autoSave();
    void clearAutoSave();

private:
    bool isModified() const;
    void loadSettings();
    void saveSettings();
    void updateOpened();
    void openRecent();

    Ui::MainWindow *ui;

    QString path;
    int _mode;

    QTimer *timer;

    QSettings *settings;

    QStringList recentOpened;

    QString language;

    bool dontUpdate = false;
    bool useWebBrowser = false;
    bool setPath = true;
    bool html = false;
    bool spelling = true;
    bool highlighting = false;

    QtSpell::TextEditChecker *checker;
};
#endif // MAINWINDOW_H
