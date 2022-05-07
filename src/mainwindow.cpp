#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"
#include "parser.h"
#include "highlighter.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QTextStream>
#include <QtPrintSupport/QPrinter>
#include <QScreen>
#include <QComboBox>
#include <QScrollBar>
#include <QSettings>
#include <QToolButton>
#include <QSaveFile>

#if QT_CONFIG(printdialog)
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#endif

#include "3rdparty/qmarkdowntextedit/qplaintexteditsearchwidget.h"
#include "3rdparty/qmarkdowntextedit/markdownhighlighter.h"
#include "3rdparty/qtspell/src/QtSpell.hpp"

#if (defined(Q_OS_BLACKBERRY) || defined(Q_OS_ANDROID) || defined(Q_OS_IOS) || defined(Q_OS_WP))
#error This application was developed for desktop only due to enchant
#endif


MainWindow::MainWindow(const QString &file, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    const QPalette &p = palette();
    const QColor &back = p.base().color();
    int r, g, b, a;
    back.getRgb(&r, &g, &b, &a);

    const bool dark = ((r + g + b + a) / 4) < 127;
    if (dark)
        setWindowIcon(QIcon(QStringLiteral(":/Icon_dark.svg")));
    else
        setWindowIcon(QIcon(QStringLiteral(":/Icon.svg")));

    ui->setupUi(this);
    ui->editor->searchWidget()->setDarkMode(dark);

    QComboBox *mode = new QComboBox(ui->Edit);
    mode->addItems({QStringLiteral("Commonmark"), QStringLiteral("GitHub")});
    mode->setCurrentIndex(1);
    _mode = 1;

    checker = new QtSpell::TextEditChecker(this);
    checker->setTextEdit(ui->editor);
    checker->setDecodeLanguageCodes(true);
    checker->setShowCheckSpellingCheckbox(true);
    checker->setUndoRedoEnabled(true);

    htmlHighliter = new Highliter(ui->raw->document());

    toolbutton = new QToolButton(this);
    toolbutton->setMenu(ui->menuRecentlyOpened);
    toolbutton->setIcon(QIcon::fromTheme(QStringLiteral("document-open-recent")));
    toolbutton->setPopupMode(QToolButton::InstantPopup);

    // Settings
    settings = new QSettings("SME", "MarkdownEdit", this);

    loadSettings(file);
    updateOpened();

    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::onFileNew);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onFileOpen);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onFileSave);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::onFileSaveAs);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onHelpAbout);
    connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(mode, &QComboBox::currentTextChanged, this, &MainWindow::changeMode);
    connect(ui->actionExportHtml, &QAction::triggered, this, &MainWindow::exportHtml);
    connect(ui->actionPrint, &QAction::triggered, this, &MainWindow::filePrint);
    connect(ui->actionPrintPreview, &QAction::triggered, this, &MainWindow::filePrintPreview);
    connect(ui->actionCut, &QAction::triggered, this, &MainWindow::cut);
    connect(ui->actionCopy, &QAction::triggered, this, &MainWindow::copy);
    connect(ui->actionPaste, &QAction::triggered, this, &MainWindow::paste);

    connect(ui->editor, &QPlainTextEdit::textChanged,
            this, &MainWindow::onTextChanged, Qt::QueuedConnection);
    connect(ui->actionHighlighting_activated, &QAction::triggered,
            this, &MainWindow::changeHighlighting);
    connect(ui->actionAuto_add_file_path_to_icon_path, &QAction::triggered,
            this, &MainWindow::changeAddtoIconPath);
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
    connect(ui->tabWidget, &QTabWidget::currentChanged,
            this, &MainWindow::onSetText);

    ui->actionSave->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    ui->actionRedo->setEnabled(false);

    ui->File->addAction(ui->actionNew);
    ui->File->addAction(ui->actionOpen);
    ui->File->addAction(ui->actionSave);
    ui->File->addSeparator();
    ui->File->addWidget(toolbutton);
    ui->File->addSeparator();
    ui->File->addAction(ui->actionExportHtml);
    ui->File->addAction(ui->actionPrint);
    ui->File->addAction(ui->actionPrintPreview);
    ui->Edit->addAction(ui->actionUndo);
    ui->Edit->addAction(ui->actionRedo);
    ui->Edit->addAction(ui->actionCut);
    ui->Edit->addAction(ui->actionCopy);
    ui->Edit->addAction(ui->actionPaste);
    ui->Edit->addSeparator();
    ui->Edit->addWidget(mode);

    if (ui->editor->toPlainText().isEmpty()) {
        QFile f(":/default.md", this);
        if (f.open(QFile::ReadOnly | QFile::Text)) {
            ui->editor->setPlainText(f.readAll());
            originalMd = ui->editor->toPlainText();
            originalMdLength = originalMd.length();
            checker->checkSpelling();
            setWindowFilePath(f.fileName());
            setWindowModified(false);
        }
        else {
            onFileNew();
        }
    }
}

void MainWindow::onSetText(const int &index)
{
    if (index == 0) {
        const int v = ui->textBrowser->verticalScrollBar()->value();
        ui->textBrowser->setHtml(html);
        ui->textBrowser->verticalScrollBar()->setValue(v);
    }
    else if (index == 1) {
        const int v = ui->raw->verticalScrollBar()->value();
        ui->raw->setPlainText(html);
        ui->raw->verticalScrollBar()->setValue(v);
    }
}

void MainWindow::changeSpelling(const bool &checked)
{
    checker->setSpellingEnabled(checked);

    ui->actionSpell_checking->setChecked(checked);
    spelling = checked;
}

void MainWindow::disablePreview(const bool &checked)
{
    if (checked)
        ui->tabWidget->hide();
    else
        ui->tabWidget->show();

    dontUpdate = checked;

    ui->actionPause_preview->setChecked(checked);
    ui->actionPause_preview->setEnabled(!checked);
}

void MainWindow::pausePreview(const bool &checked)
{
    dontUpdate = checked;
}

void MainWindow::undo()
{
    dontUpdate = true;
    checker->undo();
    ui->textBrowser->undo();
    dontUpdate = false;
    ui->raw->undo();
}

void MainWindow::redo()
{
    dontUpdate = true;
    checker->redo();
    ui->textBrowser->redo();
    dontUpdate = false;
    ui->raw->redo();
}

void MainWindow::cut()
{
    if (ui->editor->hasFocus())
        ui->editor->cut();
    else if (ui->textBrowser->hasFocus())
        ui->textBrowser->copy();
    else if (ui->raw->hasFocus())
        ui->raw->copy();
}

void MainWindow::copy()
{
    if (ui->editor->hasFocus())
        ui->editor->copy();
    else if (ui->textBrowser->hasFocus())
        ui->textBrowser->copy();
    else if (ui->raw->hasFocus())
        ui->raw->copy();
}

void MainWindow::paste()
{
    if (ui->editor->hasFocus()) {
        ui->editor->paste();
        ui->editor->highlighter()->rehighlight();
    }
    else if (ui->textBrowser->hasFocus())
        ui->textBrowser->paste();
    else if (ui->raw->hasFocus())
        ui->raw->paste();
}

void MainWindow::changeAddtoIconPath(const bool &c)
{
    setPath = c;

    QStringList searchPaths = ui->textBrowser->searchPaths();
    const bool contains = searchPaths.contains(QFileInfo(path).path());

    if (c && !contains) {
        ui->textBrowser->setSearchPaths(searchPaths << QFileInfo(path).path());
        onTextChanged();
    }
    else if (!c && contains) {
        searchPaths.removeAll(QFileInfo(path).path());
        ui->textBrowser->setSearchPaths(searchPaths);
        onTextChanged();
    }

    ui->actionAuto_add_file_path_to_icon_path->setChecked(c);
}

void MainWindow::changeHighlighting(const bool &enabled)
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
    ui->textBrowser->print(printer);
#endif
}

void MainWindow::exportHtml()
{
    QFileDialog dialog(this, tr("Export HTML"));
    dialog.setMimeTypeFilters({"text/html"});
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("html");
    if (dialog.exec() != QDialog::Accepted)
        return;

    const QString file = dialog.selectedFiles().at(0);

    QFile f(file, this);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))  {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Could not write to file %1: %2").arg(
                                 QDir::toNativeSeparators(path), f.errorString()));
        return;
    }

    QTextStream str(&f);
    str << html;

    statusBar()->showMessage(tr("HTML exported to %1").arg(QDir::toNativeSeparators(file)), 60000);
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

    html = Parser::Parse(ui->editor->document()->toPlainText(), _mode);

    onSetText(ui->tabWidget->currentIndex());

    maybeModified = true;

    const bool state = isModified();
    if (state != lastState)
        emit modificationChanged(state);
}

void MainWindow::openFile(const QString &newFile)
{
    // Don't open the save file again
    if (newFile == path)
        return;

    QFile f(newFile);

    if (f.size() > 50000) {
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

    ui->editor->setPlainText(f.readAll());
    originalMd = ui->editor->toPlainText();
    originalMdLength = originalMd.length();

    maybeModified = false;

    setWindowFilePath(QFileInfo(path).fileName());
    statusBar()->showMessage(tr("Opened %1").arg(QDir::toNativeSeparators(path)), 60000);

    checker->checkSpelling();

    updateOpened();

    emit modificationChanged(false);

    QGuiApplication::restoreOverrideCursor();
}

bool MainWindow::isModified() const
{
    if (!maybeModified)
        return false;

    return ui->editor->toPlainText() != originalMd;
}

void MainWindow::onFileNew()
{
    if (isModified()) {
        const int button = QMessageBox::question(this, windowTitle(),
                                           tr("You have unsaved changes. Do you want to create a new document anyway?"));
        if (button != QMessageBox::Yes)
            return;
    }
    checker->clearUndoRedo();

    path = "";
    ui->editor->setPlainText(tr("## New document"));
    setWindowFilePath(QFileInfo(tr("untitled.md")).fileName());
    originalMd = ui->editor->toPlainText();
    originalMdLength = originalMd.length();

    maybeModified = false;

    emit modificationChanged(false);
}

void MainWindow::onFileOpen()
{
    if (isModified()) {
        int button = QMessageBox::question(this, "",
                                           tr("You have unsaved changes. Do you want to open a new document anyway?"));
        if (button != QMessageBox::Yes)
            return;
    }

    QFileDialog dialog(this, tr("Open MarkDown File"));
    dialog.setMimeTypeFilters({"text/markdown"});
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    if (dialog.exec() == QDialog::Accepted)
        openFile(dialog.selectedFiles().at(0));
}

void MainWindow::onFileSave()
{
    if (!isModified())
        return;

    if (path.isEmpty()) {
        onFileSaveAs();
        return;
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    QSaveFile f(path, this);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))  {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::warning(this, windowTitle(),
                             tr("Could not write to file %1: %2").arg(
                                 QDir::toNativeSeparators(path), f.errorString()));
        return;
    }


    QTextStream str(&f);
    str << ui->editor->toPlainText();

    if (!f.commit()) {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::warning(this, windowTitle(),
                             tr("Could not write to file %1: %2").arg(
                                 QDir::toNativeSeparators(path), f.errorString()));
        return;
    }

    statusBar()->showMessage(tr("Wrote %1").arg(QDir::toNativeSeparators(path)), 60000);

    updateOpened();

    originalMd = ui->editor->toPlainText();
    originalMdLength = originalMd.length();

    maybeModified = false;

    emit modificationChanged(false);

    QGuiApplication::restoreOverrideCursor();
}

void MainWindow::onFileSaveAs()
{
    QFileDialog dialog(this, tr("Save MarkDown File"));
    dialog.setMimeTypeFilters({"text/markdown"});
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("md");
    if (dialog.exec() != QDialog::Accepted)
        return;

    path = dialog.selectedFiles().at(0);

    if (!path.endsWith(".md"))
        path.append(".md");

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
    dialog.addCredit(tr("<p>Spell checking is done with the <a href=\"https://github.com/software-made-easy/qtspell\">QtSpell</a> library based on the <a href=\"https://github.com/manisandro/qtspell\">QtSpell</a> library by <em>Sandro Mani</em>.</p>"));

    dialog.exec();
}

void MainWindow::openRecent() {
    // A QAction represents the action of the user clicking
    QAction *action = qobject_cast<QAction *>(sender());
    const QString filename = action->data().toString();

    // Don't open the save file again
    if (filename == path)
        return;

    if (!QFile::exists(filename)) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("This file could not be found:\n%1.").arg(
                                 QDir::toNativeSeparators(filename)));
        recentOpened.removeAll(filename);
        updateOpened();
        return;
    }

    if (isModified()) {
        const int button = QMessageBox::question(this, tr("Save changes?"),
                                           tr("You have unsaved changes. Do you want to open a new document anyway?"));
        if (button != QMessageBox::Yes)
            return;
    }

    const int index = recentOpened.indexOf(filename);
    if (index == -1)
        recentOpened.removeAll(filename);

    else
        recentOpened.move(index, 0);

    openFile(filename);
    updateOpened();
}

void MainWindow::updateOpened() {
    foreach (QAction *a, ui->menuRecentlyOpened->actions()) {
        disconnect(a, &QAction::triggered, this, &MainWindow::openRecent);
        a->deleteLater();
    }

    ui->menuRecentlyOpened->clear();

    if (!path.isEmpty() && recentOpened.contains(path))
        recentOpened.insert(0, path);

    if (recentOpened.size() > 7)
        recentOpened.takeLast();

    for (int i = 0; i < recentOpened.size(); i++) {
        const QString document(recentOpened.at(i).toLatin1());
        const QString title("&" + QString::number(i + 1) + " | " + document);
        QAction *action = new QAction(title, this);
        connect(action, &QAction::triggered, this, &MainWindow::openRecent);

        action->setData(document);
        ui->menuRecentlyOpened->addAction(action);
    }
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (isModified()) {
        const int button = QMessageBox::question(this, tr("Save changes?"),
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

void MainWindow::loadSettings(const QString &f) {
    const QByteArray geo = settings->value("geometry", QByteArray({})).toByteArray();
    if (geo.isEmpty()) {
        const QRect availableGeometry = QGuiApplication::screenAt(pos())->availableGeometry();
        resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    }
    else {
        restoreGeometry(geo);
    }

    restoreState(settings->value("state", QByteArray({})).toByteArray());

    highlighting = settings->value("highlighting", true).toBool();
    ui->actionHighlighting_activated->setChecked(highlighting);
    changeHighlighting(highlighting);

    setPath = settings->value("setPath", true).toBool();
    changeAddtoIconPath(setPath);

    recentOpened = settings->value("recent", QStringList()).toStringList();
    if (!recentOpened.isEmpty()) {
        if (recentOpened.first().isEmpty()) {
            recentOpened.takeFirst();
        }
    }

    if (f.isEmpty()) {
        const bool openLast = settings->value("openLast", true).toBool();
        if (openLast) {
            ui->actionOpen_last_document_on_start->setChecked(openLast);

            const QLatin1String last(settings->value("last", QLatin1String()).toByteArray());
            if (!last.isEmpty())
                openFile(last);
        }
    }
    else
        openFile(f);

    spelling = settings->value("spelling", true).toBool();
    changeSpelling(spelling);
    QString spellLang(settings->value("spellLang", QString()).toString());
    if (spellLang.isEmpty())
        spellLang = QLocale::system().name();
    checker->setLanguage(spellLang);

    onTextChanged();
}

void MainWindow::saveSettings() {
    settings->setValue("geometry", saveGeometry());
    settings->setValue("state", saveState());
    settings->setValue("highlighting", ui->actionHighlighting_activated->isChecked());
    settings->setValue("recent", recentOpened);
    settings->setValue("openLast", ui->actionOpen_last_document_on_start->isChecked());
    settings->setValue("last", path);
    settings->setValue("setPath", setPath);
    settings->setValue("spelling", checker->getSpellingEnabled());
    settings->setValue("spellLang", checker->getLanguage());
    settings->sync();
}

MainWindow::~MainWindow()
{
    delete ui;
}
