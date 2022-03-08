#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QTextStream>
#include <QScreen>
#include <QtPrintSupport/QPrintDialog>

// ensure the Q_OS_ makros are defined
#ifndef QSYSTEMDETECTION_H
#include <qsystemdetection.h>
#endif

#include "3dparty/qmarkdowntextedit/qplaintexteditsearchwidget.h"
// #include "3dparty/md4c/src/md4c-html.h"


#if defined(Q_OS_BLACKBERRY) || defined(Q_OS_ANDROID) || defined(Q_OS_IOS) || defined(Q_OS_WP)
#define Q_OS_MOBILE
#else
#define Q_OS_DESKTOP
#endif
#ifdef Q_OS_LINUX
#endif


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::onFileNew);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onFileOpen);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onFileSave);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::onFileSaveAs);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onHelpAbout);
    connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->editor, &QPlainTextEdit::textChanged, this, &MainWindow::onTextChanged);

    connect(ui->editor->document(), &QTextDocument::modificationChanged,
            ui->actionSave, &QAction::setEnabled);
    connect(ui->editor->document(), &QTextDocument::modificationChanged,
            this, &QMainWindow::setWindowModified);
    connect(ui->editor->document(), &QTextDocument::undoAvailable,
            ui->actionUndo, &QAction::setEnabled);
    connect(ui->editor->document(), &QTextDocument::redoAvailable,
            ui->actionRedo, &QAction::setEnabled);

    ui->actionSave->setEnabled(ui->editor->document()->isModified());
    ui->actionUndo->setEnabled(ui->editor->document()->isUndoAvailable());
    ui->actionRedo->setEnabled(ui->editor->document()->isRedoAvailable());

    ui->toolBar->addAction(ui->actionNew);
    ui->toolBar->addAction(ui->actionOpen);
    ui->toolBar->addAction(ui->actionSave);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionUndo);
    ui->toolBar->addAction(ui->actionRedo);
    ui->toolBar->addAction(ui->actionCut);
    ui->toolBar->addAction(ui->actionCopy);
    ui->toolBar->addAction(ui->actionPaste);

    QPalette p = ui->editor->palette();
    QColor back = p.base().color();
    int r;
    int g;
    int b;
    int a;
    back.getRgb(&r, &g, &b, &a);

    bool dark = ((r + g + b + a) / 4) < 127;
    ui->editor->searchWidget()->setDarkMode(dark);
    if (dark)
        setWindowIcon(QIcon(":/Icon_dark.svg"));
    else
        setWindowIcon(QIcon(":/Icon.svg"));

#ifdef Q_OS_MACOS
    // Use dark text on light background on macOS, also in dark mode.
    QPalette pal = textEdit->palette();
    pal.setColor(QPalette::Base, QColor(Qt::white));
    pal.setColor(QPalette::Text, QColor(Qt::black));
    textEdit->setPalette(pal);
#endif
#ifdef Q_OS_ANDROID
    settings = new QSettings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/SME_MarkdownEdit.ini", QSettings::IniFormat);
#else
    settings = new QSettings("SME", "MarkdownEdit", this);
#endif
    loadSettings();

    QFile f(":/default.md", this);
    if (f.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream in(&f);
        ui->editor->setPlainText(in.readAll());
        setWindowFilePath(f.fileName());
    }
    else {
        setWindowFilePath(QFileInfo("new.md").fileName());
    }
}

void MainWindow::onTextChanged()
{
    // TODO: Implement
}

void MainWindow::openFile(const QString &newFile)
{
    QFile f(newFile);
    if (!f.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, windowTitle(),
                             tr("Could not open file %1: %2").arg(
                                 QDir::toNativeSeparators(newFile), f.errorString()));
        return;
    }
    path = newFile;
    ui->editor->setPlainText(f.readAll());
    setWindowFilePath(QFileInfo(path).fileName());
    statusBar()->showMessage(tr("Opened %1").arg(QDir::toNativeSeparators(path)));
    ui->editor->document()->setModified(false);
}

bool MainWindow::isModified() const
{
    return ui->editor->document()->isModified();
}

void MainWindow::onFileNew()
{
    if (isModified()) {
        int button = QMessageBox::question(this, windowTitle(),
                                           tr("You have unsaved changes. Do you want to create a new document anyway?"));
        if (button != QMessageBox::Yes)
            return;
    }

    path.clear();
    ui->editor->setPlainText(tr("## New document"));
    ui->editor->document()->setModified(false);
    setWindowFilePath(QFileInfo("new.md").fileName());
}

void MainWindow::onFileOpen()
{
    if (isModified()) {
        int button = QMessageBox::question(this, windowTitle(),
                                           tr("You have unsaved changes. Do you want to open a new document anyway?"));
        if (button != QMessageBox::Yes)
            return;
    }

    QFileDialog dialog(this, tr("Open MarkDown File"));
    dialog.setMimeTypeFilters({"text/markdown"});
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    if (dialog.exec() == QDialog::Accepted)
        openFile(dialog.selectedFiles().constFirst());
}

void MainWindow::onFileSave()
{
    if (path.isEmpty()) {
        onFileSaveAs();
        return;
    }

    QFile f(path, this);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))  {
        QMessageBox::warning(this, windowTitle(),
                             tr("Could not write to file %1: %2").arg(
                                 QDir::toNativeSeparators(path), f.errorString()));
        return;
    }
    QTextStream str(&f);
    str << ui->editor->toPlainText();

    ui->editor->document()->setModified(false);

    statusBar()->showMessage(tr("Wrote %1").arg(QDir::toNativeSeparators(path)));
}

void MainWindow::onFileSaveAs()
{
    QFileDialog dialog(this, tr("Save MarkDown File"));
    dialog.setMimeTypeFilters({"text/markdown"});
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("md");
    if (dialog.exec() != QDialog::Accepted)
        return;

    QStringList selectedFiles = dialog.selectedFiles();
    path = selectedFiles.first();
    if (!path.endsWith(".md"))
        path.append(".md");
    setWindowFilePath(QFileInfo(path).fileName());
    onFileSave();
}

void MainWindow::onHelpAbout()
{
    About dialog(tr("About MarkdownEdit"), this);
    dialog.setAppUrl("https://github.com/software-made-easy/MarkdownEdit");
    dialog.addCredit(tr("<p>The conversion from Markdown to HTML is done with the help of the <a href=\"https://github.com/chjj/marked\">marked JavaScript library</a> by <em>Christopher Jeffrey</em>.</p>"));
    dialog.addCredit(tr("<p>The <a href=\"https://bitbucket.org/kevinburke/markdowncss/src/master/\">style sheet</a> was created by <em>Kevin Burke</em>.</p>"));
    dialog.exec();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (isModified()) {
        int button = QMessageBox::question(this, windowTitle(),
                                           tr("You have unsaved changes. Do you want to exit anyway?"));
        if (button == QMessageBox::No)
            e->ignore();
        else {
            saveSettings();
            e->accept();
        }
    }
}

void MainWindow::loadSettings() {
#ifdef Q_OS_DESKTOP
    const QByteArray geo = settings->value("geometry", QByteArray({})).toByteArray();
    if (geo.isEmpty()) {
        const QRect availableGeometry = qApp->screenAt(pos())->availableGeometry();
        resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    }
    else {
        restoreGeometry(geo);
    }
#endif
    restoreState(settings->value("state", QByteArray({})).toByteArray());
}

void MainWindow::saveSettings() {
#ifdef Q_OS_DESKTOP
    settings->setValue("geometry", saveGeometry());
#endif
    settings->setValue("state", saveState());
    settings->sync();
}

MainWindow::~MainWindow()
{
    delete ui;
}
