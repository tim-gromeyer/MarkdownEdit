#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QPrinter;
class QSettings;
class QTimer;
class QToolButton;
namespace QtSpell { class TextEditChecker; }
namespace QSourceHighlite { class QSourceHighliter; }
class Highliter;
class QPushButton;
class QComboBox;
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

signals:
    void modificationChanged(bool);
    void setText(const QString &text, const int i);;

private slots:
    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    void onHelpAbout();
    void onTextChanged();
    void changeMode(const QString &);
    void exportHtml();

    void filePrint();
    void filePrintPreview();
    void printPreview(QPrinter *);

    void changeHighlighting(const bool &);
    void changeSpelling(const bool &);
    void pausePreview(const bool &);
    void disablePreview(const bool &);
    void changeAddtoIconPath(const bool &);
    void changeWordWrap(const bool &);
    void changeWidget(const QString &);

    void undo();
    void redo();

    void cut();
    void copy();
    void paste();

private:
    bool isModified() const;
    void loadSettings(const QString &);
    void saveSettings();
    void updateOpened();
    void openRecent();
    void onSetText(const int &);

    Ui::MainWindow *ui;

    QString path;
    int _mode;

    QSettings *settings;

    QStringList recentOpened;

    QString originalMd;
    int originalMdLength;
    QString html;

    bool dontUpdate;
    bool setPath;
    bool spelling;
    bool highlighting;
    bool maybeModified;
    bool lastState;

    QtSpell::TextEditChecker *checker;
    Highliter *htmlHighliter;

    QToolButton *toolbutton;
    QComboBox *widgetBox;
};
#endif // MAINWINDOW_H
