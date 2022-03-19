#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


#include "parser.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QPrinter;
class QSettings;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void openFile(const QString &path);

protected:
    void closeEvent(QCloseEvent *e) override;

private slots:
    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    void onHelpAbout();
    void onTextChanged();
    void changeMode(const QString &text);
    void exportHtml(QString file = "");

    void filePrint();
    void filePrintPreview();
    void printPreview(QPrinter *);

    void changeHighlighting(bool enabled);

    void loadIcons(bool dark = false);

    void updateOpened();
    void openRecent();

private:
    bool isModified() const;
    void loadSettings();
    void saveSettings();

    Ui::MainWindow *ui;

    QString path;
    Parser::Dialect _mode;

    QSettings *settings;

    QStringList recentOpened;
};
#endif // MAINWINDOW_H
