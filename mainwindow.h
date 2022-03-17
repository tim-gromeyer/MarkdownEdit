#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>


#include "parser.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QPrinter;
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
    void exportHtml();

    void filePrint();
    void filePrintPreview();
    void printPreview(QPrinter *);

private:
    bool isModified() const;
    void loadSettings();
    void saveSettings();

    Ui::MainWindow *ui;

    QString path;
    Parser::Mode _mode;

    QSettings *settings;
};
#endif // MAINWINDOW_H
