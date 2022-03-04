#include "mainwindow.h"
#include "previewpage.h"
#include "ui_mainwindow.h"
#include "about.h"

#include <QFile>
#include <QFileDialog>
#include <QFontDatabase>
#include <QMessageBox>
#include <QStatusBar>
#include <QTextStream>
#include <QWebChannel>
#include <QFileInfo>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->editor->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->preview->setContextMenuPolicy(Qt::NoContextMenu);

    PreviewPage *page = new PreviewPage(this);
    ui->preview->setPage(page);

    connect(ui->editor, &QPlainTextEdit::textChanged, this,
            [this]() { m_content.setText(ui->editor->toPlainText()); });

    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("content"), &m_content);
    page->setWebChannel(channel);

    ui->preview->setUrl(QUrl("qrc:/index.html"));

    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::onFileNew);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onFileOpen);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onFileSave);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::onFileSaveAs);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onHelpAbout);
    connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);

    connect(ui->editor->document(), &QTextDocument::modificationChanged,
            ui->actionSave, &QAction::setEnabled);
    connect(ui->editor->document(), &QTextDocument::modificationChanged,
            this, &QMainWindow::setWindowModified);

    QFile defaultTextFile(":/default.md");
    defaultTextFile.open(QIODevice::ReadOnly);
    ui->editor->setPlainText(defaultTextFile.readAll());
    setWindowFilePath(QFileInfo(defaultTextFile).fileName());


    ui->toolBar->addActions(QList<QAction*>({ui->actionNew, ui->actionOpen, ui->actionSave}));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, windowTitle(),
                             tr("Could not open file %1: %2").arg(
                                 QDir::toNativeSeparators(path), f.errorString()));
        return;
    }
    m_filePath = path;
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

    m_filePath.clear();
    ui->editor->setPlainText(tr("## New document"));
    ui->editor->document()->setModified(false);
    setWindowFilePath(QFileInfo("new.md").fileName());
}

void MainWindow::onFileOpen()
{
    if (isModified()) {
        int  button = QMessageBox::question(this, windowTitle(),
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
    if (m_filePath.isEmpty()) {
        onFileSaveAs();
        return;
    }

    QFile f(m_filePath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))  {
        QMessageBox::warning(this, windowTitle(),
                             tr("Could not write to file %1: %2").arg(
                                 QDir::toNativeSeparators(m_filePath), f.errorString()));
        return;
    }
    QTextStream str(&f);
    str << ui->editor->toPlainText();

    ui->editor->document()->setModified(false);

    statusBar()->showMessage(tr("Wrote %1").arg(QDir::toNativeSeparators(m_filePath)));
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
    m_filePath = selectedFiles.first();
    if (!m_filePath.endsWith(".md"))
        m_filePath.append(".md");
    setWindowFilePath(QFileInfo(m_filePath).fileName());
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
        if (button != QMessageBox::Yes)
            e->ignore();
    }
}
