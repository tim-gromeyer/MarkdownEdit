#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QTextStream>
#include <QtPrintSupport/QPrinter>
#include <QScreen>
#include <QComboBox>
#include <QScrollBar>
#include <QSettings>
#include <QTimer>
#include <QtSpell-qt5/QtSpell.hpp>
#include <QActionGroup>


#if QT_CONFIG(printdialog)
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#endif

#include "parser.h"
#include "settings.h"
#include "3rdparty/qmarkdowntextedit/qplaintexteditsearchwidget.h"
#include "3rdparty/QSourceHighlite/qsourcehighliter.h"


#if (defined(Q_OS_BLACKBERRY) || defined(Q_OS_ANDROID) || defined(Q_OS_IOS) || defined(Q_OS_WP))
#error This application was developed for desktop only due to web engine module
#endif


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textBrowser->hide();

    QComboBox *mode = new QComboBox(ui->toolBar);
    mode->addItems({"Commonmark", "GitHub"});
    mode->setCurrentIndex(1);
    _mode = 1;

    checker = new QtSpell::TextEditChecker(this);
    checker->setTextEdit(ui->editor);
    checker->setDecodeLanguageCodes(true);
    checker->setShowCheckSpellingCheckbox(true);
    checker->setUndoRedoEnabled(true);

    htmlHighliter = new QSourceHighlite::QSourceHighliter(ui->raw->document());
    htmlHighliter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeXML);
    htmlHighliter->setTheme(QSourceHighlite::QSourceHighliter::Monokai);

    // Settings
    settings = new QSettings("SME", "MarkdownEdit", this);

    loadSettings();
    updateOpened();

    if (ui->editor->toPlainText().isEmpty()) {
        QFile f(":/default.md", this);
        if (f.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream in(&f);
            ui->editor->setPlainText(in.readAll());
            setWindowFilePath(f.fileName());
        }
        else {
            setWindowFilePath(QFileInfo(tr("new.md")).fileName());
        }
    }

    ui->raw->document()->setModified(false);

    /*
    timer = new QTimer(this);
    timer->setInterval(60000);


    connect(timer, &QTimer::timeout, this, &MainWindow::autoSave);

    timer->start();
    */

    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::onFileNew);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onFileOpen);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onFileSave);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::onFileSaveAs);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onHelpAbout);
    connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->editor, &QPlainTextEdit::textChanged, this, &MainWindow::onTextChanged);
    connect(mode, &QComboBox::currentTextChanged, this, &MainWindow::changeMode);
    connect(ui->actionExportHtml, &QAction::triggered, this, [this]{ exportHtml(); });
    connect(ui->actionPrint, &QAction::triggered, this, &MainWindow::filePrint);
    connect(ui->actionPrintPreview, &QAction::triggered, this, &MainWindow::filePrintPreview);

    connect(ui->actionPaste, &QAction::triggered,
            this, [this]{ ui->editor->paste(); changeHighlighting(highlighting); });
    connect(ui->actionHighlighting_activated, &QAction::triggered,
            this, &MainWindow::changeHighlighting);
    connect(ui->actionOptions, &QAction::triggered,
            this, &MainWindow::settingsDialog);
    connect(this, &MainWindow::modificationChanged,
            ui->actionSave, &QAction::setEnabled);
    connect(this, &MainWindow::modificationChanged,
            this, &QMainWindow::setWindowModified);
    connect(checker, &QtSpell::TextEditChecker::undoAvailable,
            ui->actionUndo, &QAction::setEnabled);
    connect(checker, &QtSpell::TextEditChecker::redoAvailable,
            ui->actionRedo, &QAction::setEnabled);
    connect(ui->actionUndo, &QAction::triggered,
            this, &MainWindow::undo);
    connect(ui->actionRedo, &QAction::triggered,
            this, &MainWindow::redo);
    connect(ui->actionDisable_preview, &QAction::triggered,
            this, &MainWindow::disablePreview);
    connect(ui->actionPause_preview, &QAction::triggered,
            this, &MainWindow::pausePreview);
    connect(ui->actionSpell_checking, &QAction::triggered,
            this, &MainWindow::changeSpelling);

    ui->actionSave->setEnabled(isModified());
    ui->actionUndo->setEnabled(ui->editor->document()->isUndoAvailable());
    ui->actionRedo->setEnabled(ui->editor->document()->isRedoAvailable());

    fileActions = new QActionGroup(this);
    fileActions->addAction(ui->actionNew);
    fileActions->addAction(ui->actionOpen);
    fileActions->addAction(ui->actionSave);
    fileActions->addAction(ui->actionSaveAs);
    editActions = new QActionGroup(this);
    editActions->addAction(ui->actionUndo);
    editActions->addAction(ui->actionRedo);
    editActions->addAction(ui->actionCopy);
    editActions->addAction(ui->actionPaste);
    editActions->addAction(ui->actionCut);

    ui->toolBar->addAction(ui->actionNew);
    ui->toolBar->addAction(ui->actionOpen);
    ui->toolBar->addAction(ui->actionSave);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionExportHtml);
    ui->toolBar->addAction(ui->actionPrint);
    ui->toolBar->addAction(ui->actionPrintPreview);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionUndo);
    ui->toolBar->addAction(ui->actionRedo);
    ui->toolBar->addAction(ui->actionCut);
    ui->toolBar->addAction(ui->actionCopy);
    ui->toolBar->addAction(ui->actionPaste);
    ui->toolBar->addSeparator();
    ui->toolBar->addWidget(mode);

    QPalette p = ui->editor->palette();
    QColor back = p.base().color();
    int r;
    int g;
    int b;
    int a;
    back.getRgb(&r, &g, &b, &a);

    const bool dark = ((r + g + b + a) / 4) < 127;
    ui->editor->searchWidget()->setDarkMode(dark);
    if (dark)
        setWindowIcon(QIcon(":/Icon_dark.svg"));
    else
        setWindowIcon(QIcon(":/Icon.svg"));

    // Tests
#ifdef QT_DEBUG
#endif
}

void MainWindow::changeSpelling(bool checked)
{
    if (checked)
        checker->setTextEdit(ui->editor);
    else
        checker->setTextEdit(static_cast<QPlainTextEdit*>(nullptr));
    checker->setSpellingEnabled(checked);

    ui->actionSpell_checking->setChecked(checked);
    spelling = checked;
}

void MainWindow::clearAutoSave()
{
    if (QFile::exists(path + ".autosave"))
        QFile::remove(path + ".autosave");
}

void MainWindow::autoSave()
{
    if (!isModified())
        return;

    QFile f(path + ".autosave");
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&f);
    out << ui->editor->document()->toPlainText();
}

void MainWindow::disablePreview(bool checked)
{
    if (checked)
        ui->tabWidget->hide();
    else
        ui->tabWidget->show();

    dontUpdate = checked;

    ui->actionPause_preview->setChecked(checked);
    ui->actionPause_preview->setEnabled(!checked);
}

void MainWindow::pausePreview(bool checked)
{
    dontUpdate = checked;
}

void MainWindow::undo()
{
    if (!useWebBrowser) {
        dontUpdate = true;
        checker->undo();
        ui->textBrowser->undo();
        dontUpdate = false;
    }
    else {
        checker->undo();

    }
    ui->raw->undo();

}

void MainWindow::redo()
{
    if (!useWebBrowser) {
        dontUpdate = true;
        checker->redo();
        ui->textBrowser->redo();
        dontUpdate = false;
    }
    else {
        checker->redo();

    }
    ui->raw->redo();
}

void MainWindow::settingsDialog()
{
    Settings dialog;
    dialog.setUseWebBrowser(useWebBrowser);
    dialog.setAddPath(setPath);
    if (dialog.exec() != QDialog::Accepted)
        return;

    if (useWebBrowser != dialog.useWebBrowser()) {
        useWebBrowser = !useWebBrowser;
        dialog.useWebBrowser();
        if (useWebBrowser) {
            ui->textBrowser->hide();
            ui->preview->show();
        }
        else {
            ui->preview->hide();
            ui->textBrowser->show();
        }
        onTextChanged();
    }

    if (setPath != dialog.addPath()) {
        setPath = !setPath;

        QStringList searchPaths = ui->textBrowser->searchPaths();
        bool contains = searchPaths.contains(QFileInfo(path).path());
        if (contains && !setPath) {
            searchPaths.removeAll(QFileInfo(path).path());
            ui->textBrowser->setSearchPaths(searchPaths);
            onTextChanged();
        }
        else if (!contains && setPath) {
            searchPaths.append(QFileInfo(path).path());
            ui->textBrowser->setSearchPaths(searchPaths);
            onTextChanged();
        }
    }
}

void MainWindow::changeHighlighting(bool enabled)
{
    dontUpdate = true;
    ui->editor->setHighlightingEnabled(enabled);
    if (enabled)
        htmlHighliter->setDocument(ui->raw->document());
    else
        htmlHighliter->setDocument(static_cast<QTextDocument*>(nullptr));
    dontUpdate = false;
    highlighting = enabled;
}

void MainWindow::filePrint()
{
#if QT_CONFIG(printdialog)
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);

    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted)
        printPreview(&printer);
    delete dlg;
#endif
}

void MainWindow::filePrintPreview()
{
#if QT_CONFIG(printpreviewdialog)
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, this, &MainWindow::printPreview);
    preview.exec();
#endif
}

void MainWindow::printPreview(QPrinter *printer)
{
#ifdef QT_NO_PRINTER
    Q_UNUSED(printer);
#else
    if (useWebBrowser) {
        QString html = Parser::Parse(ui->editor->document()->toPlainText(), Parser::MD2HTML, _mode);
        ui->textBrowser->setHtml(html);
    }
    ui->textBrowser->print(printer);
#endif
}

void MainWindow::exportHtml(QString file)
{
    if (file.isEmpty()) {
        QFileDialog dialog(this, tr("Export HTML"));
        dialog.setMimeTypeFilters({"text/html"});
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setDefaultSuffix("html");
        if (dialog.exec() != QDialog::Accepted)
            return;

        QStringList selectedFiles = dialog.selectedFiles();
        file = selectedFiles.first();
        if (!file.endsWith(".html"))
            file.append(".html");
    }

    QFile f(file, this);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))  {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Could not write to file %1: %2").arg(
                                 QDir::toNativeSeparators(path), f.errorString()));
        return;
    }
    QTextStream str(&f);

    str << Parser::Parse(ui->editor->toPlainText(), Parser::MD2HTML, _mode);

    statusBar()->showMessage(tr("HTML exported to %1").arg(QDir::toNativeSeparators(file)), 60000);

    if (!file.isEmpty()) {
        setWindowModified(false);
    }
}

void MainWindow::changeMode(const QString &text)
{
    _mode = text == "GitHub" ? 1
                             : 0;
    onTextChanged();
}

void MainWindow::onTextChanged()
{
    if (dontUpdate)
        return;

    QString html = Parser::Parse(ui->editor->document()->toPlainText(), Parser::MD2HTML, _mode);

    if (useWebBrowser)
        ui->preview->setHtml(html);
    else {
        const int value = ui->textBrowser->verticalScrollBar()->value();
        ui->textBrowser->setHtml(html);
        ui->textBrowser->verticalScrollBar()->setValue(value);
    }

    const int i = ui->raw->verticalScrollBar()->value();
    ui->raw->setPlainText(html);
    if (highlighting)
        htmlHighliter->rehighlight();
    ui->raw->verticalScrollBar()->setValue(i);

    emit modificationChanged(isModified());
}

void MainWindow::openFile(const QString &newFile)
{
    /*
    if (QFile::exists(newFile + ".autosave")) {
        QMessageBox dlg;
        dlg.setWindowTitle(tr(""));
        dlg.setText(tr(""));
        dlg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        dlg.setDefaultButton(QMessageBox::Yes);

        // todo: Finish
        if (dlg.exec() == QMessageBox::Yes) {
            QFile f(newFile + ".autosave");
            if (!f.rename(newFile)) {}
        }
    }
    */
    QFile f(newFile);

    if (f.size() > 10000) {
        int out = QMessageBox::warning(this, tr("Large file"),
                                       tr("This is a large file that can potentially cause performance issues."),
                                       QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);

        if (out == QMessageBox::Cancel)
            return;
    }
    if (!f.open(QIODevice::ReadWrite)) {
        QMessageBox::warning(this, tr("Couldn't open file"),
                             tr("Could not open file %1: %2").arg(
                                 QDir::toNativeSeparators(newFile), f.errorString()));
        recentOpened.removeAll(newFile);
        updateOpened();
        return;
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    path = newFile;

    // moved here, because the pictures used to be above the text
    if (setPath) {
        if (!ui->textBrowser->searchPaths().contains(QFileInfo(newFile).path()))
            ui->textBrowser->setSearchPaths(ui->textBrowser->searchPaths() << QFileInfo(newFile).path());
    }

    checker->clearUndoRedo();

    if (!newFile.endsWith(".md")) {
        QString md = Parser::Parse(f.readAll(), Parser::HTML2MD);
        ui->editor->setPlainText(md);
        html = true;
        originalMd = md;
        originalMdLength = md.length();
    }
    else {
        ui->editor->setPlainText(f.readAll());
        html = false;
        originalMd = ui->editor->toPlainText();
        originalMdLength = originalMd.length();
    }

    setWindowFilePath(QFileInfo(path).fileName());
    statusBar()->showMessage(tr("Opened %1").arg(QDir::toNativeSeparators(path)), 60000);
    // ui->editor->document()->setModified(false);

    // It's already done in qtspell
    // checker->setSpellingEnabled(spelling);

    updateOpened();
    clearAutoSave();

    QGuiApplication::restoreOverrideCursor();
}

bool MainWindow::isModified() const
{
    QString text = ui->editor->toPlainText();
    if (text.length() != originalMdLength)
        return true;
    else
        return text != originalMd;
}

void MainWindow::onFileNew()
{
    if (isModified()) {
        int button = QMessageBox::question(this, windowTitle(),
                                           tr("You have unsaved changes. Do you want to create a new document anyway?"));
        if (button != QMessageBox::Yes)
            return;
    }
    clearAutoSave();
    checker->clearUndoRedo();

    path = "";
    ui->editor->setPlainText(tr("## New document"));
    ui->editor->document()->setModified(false);
    setWindowFilePath(QFileInfo("new.md").fileName());
    originalMd = ui->editor->toPlainText();
    originalMdLength = originalMd.length();

    html = false;
}

void MainWindow::onFileOpen()
{
    if (isModified()) {
        int button = QMessageBox::question(this, windowTitle(),
                                           tr("You have unsaved changes. Do you want to open a new document anyway?"));
        if (button != QMessageBox::Yes)
            return;
    }

    QStringList mimes;
    mimes << "text/markdown";
#ifdef QT_DEBUG
    mimes << "text/html";
#endif

    QFileDialog dialog(this, tr("Open MarkDown File"));
    dialog.setMimeTypeFilters(mimes);
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

    if (html) {
        exportHtml(path);
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

    clearAutoSave();

    ui->editor->document()->setModified(false);

    statusBar()->showMessage(tr("Wrote %1").arg(QDir::toNativeSeparators(path)), 60000);

    updateOpened();

    originalMd = ui->editor->toPlainText();
    originalMdLength = originalMd.length();

    emit modificationChanged(false);
}

void MainWindow::onFileSaveAs()
{
    QStringList mimes = {"text/markdown"};
#ifdef QT_DEBUG
    mimes << "text/html";
#endif

    QFileDialog dialog(this, tr("Save MarkDown File"));
    dialog.setMimeTypeFilters(mimes);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("md");
    if (dialog.exec() != QDialog::Accepted)
        return;

    QStringList selectedFiles = dialog.selectedFiles();
    path = selectedFiles.first();

    if (dialog.selectedMimeTypeFilter() ==  "text/html")
        html = true;

    if (!path.endsWith(".md") && !html)
        path.append(".md");
    if (!path.endsWith(".html") && html)
        path.append(".html");
    setWindowFilePath(QFileInfo(path).fileName());
    onFileSave();
}

void MainWindow::onHelpAbout()
{
    About dialog(tr("About MarkdownEdit"), this);
    dialog.setAppUrl("https://github.com/software-made-easy/MarkdownEdit");
    dialog.setDescription(tr("MarkdownEdit, as the name suggests, is a simple and easy program to create and edit Markdown files."));

    dialog.addCredit(tr("<p>The conversion from Markdown to HTML is done with the help of the <a href=\"https://github.com/mity/md4c\">md4c</a> library by <em>Martin Mitáš</em>.</p>"));
    dialog.addCredit(tr("<p>The <a href=\"https://github.com/pbek/qmarkdowntextedit\">widget</a> used for writing was created by <em>Patrizio Bekerle</em>.</p>"));
    dialog.addCredit(tr("<p>Spell checking is done using the <a href=\"https://github.com/manisandro/qtspell\">QtSpell</a> library by <em>Sandro Mani</em>.</p>"));
    dialog.addCredit(tr("<p>The HTML syntax is highlighted using <em>Waqar Ahmed</em>'s <a href=\"https://github.com/Waqar144/QSourceHighlite\">QSourceHighlite</a> library.</p>"));

    dialog.exec();
}

void MainWindow::openRecent() {
    // A QAction represents the action of the user clicking
    QAction *action = qobject_cast<QAction *>(sender());
    QString filename = action->data().toString();

    if (!QFile::exists(filename)) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("This file could not be found:\n%1.").arg(
                                 QDir::toNativeSeparators(filename)));
        recentOpened.removeAll(filename);
        return;
    }

    recentOpened.move(recentOpened.indexOf(filename), 0);

    openFile(filename);
    updateOpened();
}

void MainWindow::updateOpened() {
    ui->menuRecentlyOpened->clear();

    if (!path.isEmpty() && recentOpened.indexOf(path) == -1) {
        recentOpened.insert(0, path);
    }

    if (recentOpened.size() > 7) {
        recentOpened.takeLast();
    }

    for (int i = 0; i < recentOpened.size(); i++) {
        QString document = recentOpened.at(i);
        QString title("&" + QString::number(recentOpened.indexOf(document) + 1) + " | " + document);
        QAction *action = new QAction(title, this);
        connect(action, &QAction::triggered, this, &MainWindow::openRecent);

        action->setData(document);
        ui->menuRecentlyOpened->addAction(action);
    }
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
    else {
        saveSettings();
        e->accept();
    }
}

void MainWindow::loadSettings() {
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

    restoreState(settings->value("state", QByteArray({})).toByteArray());

    highlighting = settings->value("highlighting", QString::number(true)).toBool();
    ui->actionHighlighting_activated->setChecked(highlighting);
    changeHighlighting(highlighting);

    setPath = settings->value("setPath", QString::number(true)).toBool();

    recentOpened = settings->value("recent", QStringList()).toStringList();
    if (!recentOpened.isEmpty()) {
        if (recentOpened.first().isEmpty()) {
            recentOpened.takeFirst();
        }
    }

    const bool openLast = settings->value("openLast", QString::number(true)).toBool();
    if (openLast) {
        ui->actionOpen_last_document_on_start->setChecked(openLast);

        QString last = settings->value("last", QString()).toString();
        if (!last.isEmpty())
            openFile(last);
    }

    useWebBrowser = settings->value("useWebBrowser", QString::number(false)).toBool();
    if (useWebBrowser) {
        ui->textBrowser->hide();
        ui->preview->show();
    }
    else {
        ui->preview->hide();
        ui->textBrowser->show();
    }

    onTextChanged();
    spelling = settings->value("spelling", QString::number(true)).toBool();
    changeSpelling(spelling);
    const QString spellLang = settings->value("spellLang", QString()).toString();
    if (!spellLang.isEmpty())
        checker->setLanguage(spellLang);
}

void MainWindow::saveSettings() {
    settings->setValue("geometry", saveGeometry());
    settings->setValue("state", saveState());
    settings->setValue("highlighting", QString::number(ui->actionHighlighting_activated->isChecked()));
    settings->setValue("recent", recentOpened);
    settings->setValue("openLast", QString::number(ui->actionOpen_last_document_on_start->isChecked()));
    settings->setValue("last", path);
    settings->setValue("useWebBrowser", useWebBrowser);
    settings->setValue("setPath", QString::number(setPath));
    settings->setValue("spelling", QString::number(checker->getSpellingEnabled()));
    settings->setValue("spellLang", checker->getLanguage());
    settings->sync();
}

MainWindow::~MainWindow()
{
    // timer->stop();
    delete ui;
}
