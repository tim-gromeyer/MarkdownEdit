#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"
#include "markdownparser.h"
#include "highlighter.h"
#include "spellchecker.h"
#include "common.h"

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
#include <QPushButton>
#include <QDebug>
#include <QCompleter>

#if QT_CONFIG(printdialog)
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#endif

#include "qplaintexteditsearchwidget.h"


MainWindow::MainWindow(const QString &file, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    const QColor back = palette().base().color();
    int r, g, b, a;
    back.getRgb(&r, &g, &b, &a);

    const bool dark = ((r + g + b + a) / 4) < 127;
    if (dark)
        setWindowIcon(QIcon(QStringLiteral(":/Icon_dark.svg")));
    else
        setWindowIcon(QIcon(QStringLiteral(":/Icon.svg")));

    ui->setupUi(this);
    ui->editor->searchWidget()->setDarkMode(dark);

    loadIcons();

    QScreen *screen = qApp->screenAt(pos());
    if (screen->primaryOrientation() == Qt::PortraitOrientation)
        disablePreview(true);
    connect(screen, &QScreen::primaryOrientationChanged,
            this, &MainWindow::onOrientationChanged);

    mode = new QComboBox(ui->Edit);
    mode->addItems({QString::fromLatin1("Commonmark"), QString::fromLatin1("GitHub")});
    mode->setCurrentIndex(1);
    _mode = 1;

    widgetBox = new QComboBox(this);
    widgetBox->addItems({tr("Preview"), tr("HTML")});
    widgetBox->setCurrentIndex(0);

    htmlHighliter = new Highliter(ui->raw->document());

    toolbutton = new QToolButton(this);
    toolbutton->setMenu(ui->menuRecentlyOpened);
    toolbutton->setIcon(ui->menuRecentlyOpened->icon());
    toolbutton->setPopupMode(QToolButton::InstantPopup);

    // Settings
    settings = new QSettings(QStringLiteral("SME"),
                             QStringLiteral("MarkdownEdit"), this);

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

    connect(widgetBox, &QComboBox::currentTextChanged,
            this, &MainWindow::changeWidget);
    connect(ui->editor, &QPlainTextEdit::textChanged,
            this, &MainWindow::onTextChanged);
    connect(ui->actionHighlighting_activated, &QAction::triggered,
            this, &MainWindow::changeHighlighting);
    connect(ui->actionAuto_add_file_path_to_icon_path, &QAction::triggered,
            this, &MainWindow::changeAddtoIconPath);
    connect(ui->editor->document(), &QTextDocument::modificationChanged,
            ui->actionSave, &QAction::setEnabled);
    connect(ui->editor->document(), &QTextDocument::modificationChanged,
            this, &QMainWindow::setWindowModified);
    connect(ui->editor->document(), &QTextDocument::undoAvailable,
            ui->actionUndo, &QAction::setEnabled);
    connect(ui->editor->document(), &QTextDocument::redoAvailable,
            ui->actionRedo, &QAction::setEnabled);
    connect(ui->actionDisable_preview, &QAction::triggered,
            this, &MainWindow::disablePreview);
    connect(ui->actionPause_preview, &QAction::triggered,
            this, &MainWindow::pausePreview);
    connect(ui->actionSpell_checking, &QAction::triggered,
            this, &MainWindow::changeSpelling);
    connect(ui->tabWidget, &QStackedWidget::currentChanged,
            this, &MainWindow::onSetText);
    connect(ui->actionWord_wrap, &QAction::triggered,
            this, &MainWindow::changeWordWrap);
    connect(ui->actionReload, &QAction::triggered,
            this, &MainWindow::onFileReload);

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
    ui->Edit->addSeparator();
    ui->Edit->addWidget(widgetBox);

    if (ui->editor->toPlainText().isEmpty()  && file.isEmpty()) {
        QFile f(QStringLiteral(":/default.md"), this);
        if (f.open(QFile::ReadOnly | QFile::Text)) {
            ui->editor->setText(f.readAll(), QLatin1String(":/defauld.md"));
            setWindowFilePath(f.fileName());
            ui->actionReload->setText(tr("Reload \"%1\"").arg(f.fileName()));
        }
        else {
            onFileNew();
        }
    }

#ifdef NO_SPELLCHECK
    ui->menuExtras->removeAction(ui->actionSpell_checking);
#endif
}

void MainWindow::loadIcons()
{
    loadIcon(QStringLiteral("application-exit"), ui->actionExit);
    loadIcon(QStringLiteral("document-new"), ui->actionNew);
    loadIcon(QStringLiteral("document-open-recent"), ui->actionOpen_last_document_on_start);
    loadIcon(QStringLiteral("document-open"), ui->actionOpen);
    loadIcon(QStringLiteral("document-print-preview"), ui->actionPrintPreview);
    loadIcon(QStringLiteral("document-print"), ui->actionPrint);
    loadIcon(QStringLiteral("document-save-as"), ui->actionSaveAs);
    loadIcon(QStringLiteral("document-save"), ui->actionSave);
    loadIcon(QStringLiteral("edit-copy"), ui->actionCopy);
    loadIcon(QStringLiteral("edit-cut"), ui->actionCut);
    loadIcon(QStringLiteral("edit-paste"), ui->actionPaste);
    loadIcon(QStringLiteral("edit-redo"), ui->actionRedo);
    loadIcon(QStringLiteral("edit-select-all"), ui->actionSelectAll);
    loadIcon(QStringLiteral("edit-undo"), ui->actionUndo);
    loadIcon(QStringLiteral("edit-copy"), ui->actionCopy);
    loadIcon(QStringLiteral("help-about"), ui->actionAbout);
    loadIcon(QStringLiteral("help-contents"), ui->actionMarkdown_Syntax);
    loadIcon(QStringLiteral("text-wrap"), ui->actionWord_wrap);
    loadIcon(QStringLiteral("tools-check-spelling"), ui->actionSpell_checking);
    loadIcon(QStringLiteral("document-revert"), ui->actionReload);

    ui->menuExport->setIcon(QIcon::fromTheme(QStringLiteral("document-export"),
                                             QIcon(QStringLiteral(":/icons/document-export.svg"))));
    ui->menuRecentlyOpened->setIcon(QIcon::fromTheme(QLatin1String("document-open-recent"),
                                                     QIcon(QStringLiteral(":/icons/document-open-recent.svg"))));
}

void MainWindow::loadIcon(const QString &name, QAction* &a)
{
    a->setIcon(QIcon::fromTheme(name, QIcon(QStringLiteral(
                                                ":/icons/%1.svg").arg(name))));
}

void MainWindow::onOrientationChanged(const Qt::ScreenOrientation &t)
{
    if (t == Qt::PortraitOrientation)
        disablePreview(true);
    else if (t == Qt::LandscapeOrientation)
        disablePreview(false);
}

void MainWindow::changeWidget(const QString &text)
{
    Q_UNUSED(text);

    ui->tabWidget->setCurrentIndex(widgetBox->currentIndex());
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

void MainWindow::changeWordWrap(const bool &c)
{
    if (c)
        ui->editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    else
        ui->editor->setLineWrapMode(QPlainTextEdit::NoWrap);

    ui->actionWord_wrap->setChecked(c);
}

void MainWindow::changeSpelling(const bool &checked)
{
#ifdef NO_SPELLCHECK
    return;
#endif

    ui->editor->getChecker()->setSpellCheckingEnabled(checked);

    ui->actionSpell_checking->setChecked(checked);
    spelling = checked;
}

void MainWindow::disablePreview(const bool &checked)
{
    ui->tabWidget->setVisible(!checked);

    dontUpdate = checked;

    ui->actionPause_preview->setChecked(checked);
    ui->actionPause_preview->setEnabled(!checked);

    ui->actionDisable_preview->setChecked(checked);
}

void MainWindow::pausePreview(const bool &checked)
{
    dontUpdate = checked;
}

void MainWindow::cut()
{
    if (ui->editor->hasFocus())
        ui->editor->cut();
    else if (ui->textBrowser->hasFocus())
        ui->textBrowser->cut();
    else if (ui->raw->hasFocus())
        ui->raw->cut();
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

    ui->editor->getChecker()->setMarkdownHighlightingEnabled(enabled);

    if (enabled)
        htmlHighliter->setDocument(ui->raw->document());
    else
        htmlHighliter->setDocument(nullptr);
    dontUpdate = false;
    highlighting = enabled;
}

void MainWindow::filePrint()
{
#if QT_CONFIG(printdialog)
    QPrinter printer(QPrinter::HighResolution);

    QPrintDialog dlg(&printer, this);
    dlg.setWindowTitle(tr("Print Document"));

    if (dlg.exec() == QDialog::Accepted)
        printPreview(&printer);
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

    statusBar()->showMessage(tr("HTML exported to %1").arg(QDir::toNativeSeparators(file)), 30000);
}

// Indexchanged doesnt work in qt 5.12.8
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
}

void MainWindow::openFile(const QString &newFile)
{
    QFile f(newFile);

    if (!f.open(QIODevice::ReadWrite)) {
        QMessageBox::warning(this, tr("Couldn't open file"),
                             tr("Could not open file %1: %2").arg(
                                 QDir::toNativeSeparators(newFile), f.errorString()));
        recentOpened.removeAll(newFile);
        updateOpened();
        return;
    }
    if (f.size() > 50000) {
        const int out = QMessageBox::warning(this, tr("Large file"),
                                             tr("This is a large file that can potentially cause performance issues."),
                                             QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);

        if (out == QMessageBox::Cancel)
            return;
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    path = newFile;

    if (setPath)
        if (!ui->textBrowser->searchPaths().contains(QFileInfo(newFile).path()))
            ui->textBrowser->setSearchPaths(ui->textBrowser->searchPaths() << QFileInfo(newFile).path());

    ui->editor->setText(f.readAll(), newFile);

    setWindowFilePath(QFileInfo(path).fileName());
    ui->actionReload->setText(tr("Reload \"%1\"").arg(windowFilePath()));
    statusBar()->showMessage(tr("Opened %1").arg(QDir::toNativeSeparators(path)), 30000);

    updateOpened();

    QGuiApplication::restoreOverrideCursor();
}

void MainWindow::onFileNew()
{
    if (ui->editor->document()->isModified()) {
        const int button = QMessageBox::question(this, tr("Save changes?"),
                                           tr("You have unsaved changes. Do you want to create a new document anyway?"));
        if (button != QMessageBox::Yes)
            return;
    }

    path = QLatin1String();
    ui->editor->setText(tr("## New document"));
    setWindowFilePath(QFileInfo(tr("untitled.md")).fileName());
    ui->actionReload->setText(tr("Reload \"%1\"").arg(windowFilePath()));
}

void MainWindow::onFileOpen()
{
#if defined(Q_OS_WASM)
    auto fileContentReady = [this](const QString &newFile, const QByteArray &fileContent) {
        if (!newFile.isEmpty()) {
            QGuiApplication::setOverrideCursor(Qt::WaitCursor);

            path = newFile;

            if (setPath)
                if (!ui->textBrowser->searchPaths().contains(QFileInfo(newFile).path()))
                    ui->textBrowser->setSearchPaths(ui->textBrowser->searchPaths() << QFileInfo(newFile).path());

            ui->editor->setText(fileContent, newFile);

            setWindowFilePath(QFileInfo(path).fileName());
            ui->actionReload->setText(tr("Reload \"%1\"").arg(windowFilePath()));
            statusBar()->showMessage(tr("Opened %1").arg(QDir::toNativeSeparators(path)), 30000);

            updateOpened();

            QMap<QString, QVariant> map = LANGUAGE_MAP();

            if (map.contains(path))
                ui->editor->getChecker()->setLanguage(map[path].toString());
            else
                map[path] = ui->editor->getChecker()->getLanguage();

            QGuiApplication::restoreOverrideCursor();
        }
    };
    QFileDialog::getOpenFileContent("Markdown (*.md *.markdown *.mkd)", fileContentReady);
#else
    if (ui->editor->document()->isModified()) {
        const int button = QMessageBox::question(this, tr("Save changes?"),
                                           tr("You have unsaved changes. Do you want to open a new document anyway?"));
        if (button != QMessageBox::Yes)
            return;
    }

    QFileDialog dialog(this, tr("Open MarkDown File"));
    dialog.setMimeTypeFilters({"text/markdown"});
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    if (dialog.exec() == QDialog::Accepted) {
        const QString file = dialog.selectedFiles().at(0);
        if (file == path) return;
        openFile(file);
    }
#endif
}

void MainWindow::onFileSave()
{
    if (!ui->editor->document()->isModified())
        if (QFile::exists(path))
            return;

    if (path.isEmpty()) {
        onFileSaveAs();
        return;
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

#if defined(Q_OS_WASM)
    QFileDialog::saveFileContent(ui->editor->toPlainText().toLocal8Bit(), path);
#else
    QSaveFile f(path, this);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::warning(this, tr("Warning"),
                             tr("Could not write to file %1: %2").arg(
                                 QDir::toNativeSeparators(path), f.errorString()));
        return;
    }

    QTextStream str(&f);
    str << ui->editor->toPlainText();

    if (!f.commit()) {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::warning(this, tr("Warning"),
                             tr("Could not write to file %1: %2").arg(
                                 QDir::toNativeSeparators(path), f.errorString()));
        return;
    }
#endif

    statusBar()->showMessage(tr("Wrote %1").arg(QDir::toNativeSeparators(path)), 30000);

    updateOpened();

    ui->editor->document()->setModified(false);

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

    if (!(path.endsWith(QLatin1String(".md")) || path.endsWith(QLatin1String(".markdown")) || path.endsWith(QLatin1String(".mkd"))))
        path.append(".md");

    setWindowFilePath(QFileInfo(path).fileName());
    ui->actionReload->setText(tr("Reload \"%1\"").arg(windowFilePath()));

    QMap<QString, QVariant> map = LANGUAGE_MAP();
    map[path] = ui->editor->getChecker()->getLanguage();
    setLanguageMap(map);

    onFileSave();
}

void MainWindow::onHelpAbout()
{
    About dialog(tr("About MarkdownEdit"), this);
    dialog.setAppUrl("https://github.com/software-made-easy/MarkdownEdit");
    dialog.setDescription(tr("MarkdownEdit, as the name suggests, is a simple and easy program to create and edit Markdown files."));

    dialog.addCredit(tr("<p>The conversion from Markdown to HTML is done with the help of the <a href=\"https://github.com/mity/md4c\">md4c</a> library by <em>Martin Mitáš</em>.</p>"));
    dialog.addCredit(tr("<p>The <a href=\"https://github.com/pbek/qmarkdowntextedit\">widget</a> used for writing was created by <em>Patrizio Bekerle</em>.</p>"));

    dialog.exec();
}

void MainWindow::openRecent() {
    // A QAction represents the action of the user clicking
    QAction *action = qobject_cast<QAction *>(sender());
    const QString filename = action->data().toString();

    // Don't open the save file again
    if (filename == path) return;

    if (!QFile::exists(filename)) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("This file could not be found:\n%1.").arg(
                                 QDir::toNativeSeparators(filename)));
        recentOpened.removeAll(filename);
        updateOpened();
        return;
    }

    if (ui->editor->document()->isModified()) {
        const int button = QMessageBox::question(this, tr("Save changes?"),
                                           tr("You have unsaved changes. Do you want to open a new document anyway?"));
        if (button != QMessageBox::Yes)
            return;
    }

    recentOpened.move(recentOpened.indexOf(filename), 0);

    openFile(filename);
    updateOpened();
}

void MainWindow::updateOpened() {
    for (QAction* &a : ui->menuRecentlyOpened->actions()) {
        disconnect(a, &QAction::triggered, this, &MainWindow::openRecent);
        a->deleteLater();
        delete a;
    }

    ui->menuRecentlyOpened->clear();

    if (recentOpened.contains(path) && recentOpened.at(0) != path)
        recentOpened.move(recentOpened.indexOf(path), 0);
    else if (!path.isEmpty() && !recentOpened.contains(path))
        recentOpened.insert(0, path);

    if (recentOpened.size() > RECENT_OPENED_LIST_LENGTH)
        recentOpened.takeLast();

    for (int i = 0; i < recentOpened.size(); i++) {
        const QString document = recentOpened.at(i);
        QAction *action = new QAction(QStringLiteral("&%1 | %2").arg(QString::number(i + 1),
                                                                     document), this);
        connect(action, &QAction::triggered, this, &MainWindow::openRecent);

        action->setData(document);
        ui->menuRecentlyOpened->addAction(action);
    }
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (ui->editor->document()->isModified()) {
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
    const QByteArray geo = settings->value(QStringLiteral("geometry"),
                                           QByteArrayLiteral("")).toByteArray();
    if (geo.isEmpty()) {
        const QRect availableGeometry = QGuiApplication::screenAt(pos())->availableGeometry();
        resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    }
    else {
        restoreGeometry(geo);
    }

    restoreState(settings->value(QStringLiteral("state"), QByteArrayLiteral("")).toByteArray());

    highlighting = settings->value(QStringLiteral("highlighting"), true).toBool();
    ui->actionHighlighting_activated->setChecked(highlighting);
    changeHighlighting(highlighting);

    setPath = settings->value(QStringLiteral("setPath"), true).toBool();
    changeAddtoIconPath(setPath);

    recentOpened = settings->value(QStringLiteral("recent"), QStringList()).toStringList();
    if (!recentOpened.isEmpty()) {
        if (recentOpened.first().isEmpty()) {
            recentOpened.takeFirst();
        }
    }

    const bool lineWrap = settings->value(QStringLiteral("lineWrap"), false).toBool();
    changeWordWrap(lineWrap);

    setLanguageMap(settings->value(QStringLiteral("languagesMap"),
                                   QMap<QString, QVariant>()).toMap());

    if (f.isEmpty()) {
        const bool openLast = settings->value(QStringLiteral("openLast"), true).toBool();
        if (openLast) {
            ui->actionOpen_last_document_on_start->setChecked(openLast);

            const QString last(settings->value(QStringLiteral("last"),
                                               QLatin1String()).toString());
            if (!last.isEmpty())
                openFile(last);
        }
    }
    else
        openFile(f);

    spelling = settings->value(QStringLiteral("spelling"), true).toBool();
    changeSpelling(spelling);

    onTextChanged();
}

void MainWindow::saveSettings() {
    settings->setValue("geometry", saveGeometry());
    settings->setValue("state", saveState());
    settings->setValue("highlighting", highlighting);
    settings->setValue("recent", recentOpened);
    settings->setValue("openLast", ui->actionOpen_last_document_on_start->isChecked());
    settings->setValue("last", path);
    settings->setValue("setPath", setPath);
    settings->setValue("spelling", spelling);
    settings->setValue("lineWrap", ui->actionWord_wrap->isChecked());
    settings->setValue("languagesMap", LANGUAGE_MAP());
    settings->sync();
}

MainWindow::~MainWindow()
{
    delete ui;
}
