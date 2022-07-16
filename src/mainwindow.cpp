#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"
#include "markdownparser.h"
#include "highlighter.h"
#include "spellchecker.h"
#include "common.h"
#include "markdowneditor.h"

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
#include <QDebug>
#include <QTimer>
#include <QTemporaryFile>
#include <QDesktopServices>
#include <QFileSystemWatcher>
#include <QShortcut>

#if QT_CONFIG(printdialog)
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#endif

#include "qplaintexteditsearchwidget.h"


MainWindow::MainWindow(const QStringList &file, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    toolbutton = new QToolButton(this);
    toolbutton->setMenu(ui->menuRecentlyOpened);
    toolbutton->setPopupMode(QToolButton::InstantPopup);

    shortcutNew = new QShortcut(this);
    shortcutClose = new QShortcut(this);

    loadIcons();
    setupThings();

    watcher = new QFileSystemWatcher(this);
    connect(watcher, &QFileSystemWatcher::fileChanged,
            this, &MainWindow::onFileChanged);

    connect(ui->tabWidget_2, &QTabWidget::tabCloseRequested,
            this, &MainWindow::closeEditor);

    settings = new QSettings(QLatin1String("SME"),
                             QLatin1String("MarkdownEdit"), this);

    loadSettings(file);
    updateOpened();

    mode = new QComboBox(ui->Edit);
    mode->addItems(QStringList() << QLatin1String("Commonmark") << QLatin1String("GitHub"));
    mode->setCurrentIndex(1);
    _mode = 1;

    widgetBox = new QComboBox(this);
    widgetBox->addItems(QStringList() << tr("Preview") << tr("HTML"));
    widgetBox->setCurrentIndex(0);

    htmlHighliter = new Highliter(ui->raw->document());

    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::onFileNew);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onFileOpen);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onFileSave);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::onFileSaveAs);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onHelpAbout);
    connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->actionExportHtml, &QAction::triggered, this, &MainWindow::exportHtml);
    connect(ui->actionExportPdf, &QAction::triggered, this, &MainWindow::exportPdf);
    connect(ui->actionPrint, &QAction::triggered, this, &MainWindow::filePrint);
    connect(ui->actionPrintPreview, &QAction::triggered, this, &MainWindow::filePrintPreview);
    connect(ui->actionCut, &QAction::triggered, this, &MainWindow::cut);
    connect(ui->actionCopy, &QAction::triggered, this, &MainWindow::copy);
    connect(ui->actionPaste, &QAction::triggered, this, &MainWindow::paste);

    connect(ui->actionHighlighting_activated, &QAction::triggered,
            this, &MainWindow::changeHighlighting);
    connect(ui->actionAuto_add_file_path_to_icon_path, &QAction::triggered,
            this, &MainWindow::changeAddtoIconPath);
    connect(ui->actionDisable_preview, &QAction::triggered,
            this, &MainWindow::disablePreview);
    connect(ui->actionPause_preview, &QAction::triggered,
            this, &MainWindow::pausePreview);
    connect(ui->actionSpell_checking, &QAction::triggered,
            this, &MainWindow::changeSpelling);
    connect(ui->tabWidget, &QStackedWidget::currentChanged,
            this, &MainWindow::setText);
    connect(ui->actionWord_wrap, &QAction::triggered,
            this, &MainWindow::changeWordWrap);
    connect(ui->actionReload, &QAction::triggered,
            this, &MainWindow::onFileReload);
    connect(ui->actionOpen_in_web_browser, &QAction::triggered,
            this, &MainWindow::openInWebBrowser);
    connect(mode, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &MainWindow::changeMode);
    connect(widgetBox, qOverload<int>(&QComboBox::currentIndexChanged),
            ui->tabWidget, &QStackedWidget::setCurrentIndex);
    connect(ui->tabWidget_2, &QTabWidget::currentChanged,
            this, &MainWindow::onEditorChanged);
    connect(ui->actionSelectAll, &QAction::triggered,
            this, &MainWindow::selectAll);
    connect(ui->actionUndo, &QAction::triggered,
            this, &MainWindow::undo);
    connect(ui->actionRedo, &QAction::triggered,
            this, &MainWindow::redo);
    connect(ui->actionMarkdown_Syntax, &QAction::triggered,
            &MarkdownEditor::showMarkdownSyntax);
    connect(ui->tabWidget_2->tabBar(), &QTabBar::tabMoved,
            this, &MainWindow::editorMoved);

    ui->actionSave->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    ui->actionRedo->setEnabled(false);

    ui->File->addAction(ui->actionNew);
    ui->File->addAction(ui->actionOpen);
    ui->File->addAction(ui->actionSave);
    ui->File->addSeparator();
    ui->File->addWidget(toolbutton);
    ui->File->addSeparator();
    ui->File->addAction(ui->actionPrintPreview);
    ui->Edit->addAction(ui->actionUndo);
    ui->Edit->addAction(ui->actionRedo);
    ui->Edit->addAction(ui->actionCut);
    ui->Edit->addAction(ui->actionCopy);
    ui->Edit->addAction(ui->actionPaste);
    ui->toolBarPreview->addWidget(mode);
    ui->toolBarPreview->addSeparator();
    ui->toolBarPreview->addWidget(widgetBox);

#ifdef NO_SPELLCHECK
    ui->menuExtras->removeAction(ui->actionSpell_checking);
#endif
#ifdef Q_OS_WASM
    ui->menuExport->removeAction(ui->actionExportPdf);
    ui->menuFile->removeAction(ui->actionPrint);
    ui->menuFile->removeAction(ui->actionPrintPreview);

    ui->File->removeAction(ui->actionPrintPreview);
#endif
}

void MainWindow::toForeground()
{
    raise();
    activateWindow();
}

void MainWindow::onFileReload()
{
    if (reloadFile.isEmpty())
        reloadFile = path;

    QFile f(reloadFile);

    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Couldn't open file"),
                             tr("Could not open file %1: %2").arg(
                                 QDir::toNativeSeparators(reloadFile), f.errorString()));
        recentOpened.removeOne(reloadFile);
        updateOpened();
        return;
    }

    for (MarkdownEditor* &editor : editorList) {
        if (editor->getPath() == reloadFile) {
            editor->setText(f.readAll());
            break;
        }
    }

    if (widgetReloadFile)
        widgetReloadFile->hide();

    reloadFile.clear();
}

void MainWindow::setupThings()
{
    QScreen *screen = qApp->primaryScreen();
    onOrientationChanged(screen->primaryOrientation());
    connect(screen, &QScreen::primaryOrientationChanged,
            this, &MainWindow::onOrientationChanged);

    shortcutNew->setKey(QKeySequence::AddTab);
    connect(shortcutNew, &QShortcut::activated,
            this, &MainWindow::onFileNew);

    shortcutClose->setKey(QKeySequence::Close);
    connect(shortcutClose, &QShortcut::activated,
            this, &MainWindow::closeCurrEditor);

    ui->actionNew->setShortcuts(QKeySequence::New);
    ui->actionOpen->setShortcuts(QKeySequence::Open);
    ui->actionSave->setShortcuts(QKeySequence::Save);
    ui->actionSaveAs->setShortcuts(QKeySequence::SaveAs);
    ui->actionReload->setShortcuts(QKeySequence::Refresh);
    ui->actionNew->setShortcuts(QKeySequence::New);
    ui->actionPrint->setShortcuts(QKeySequence::Print);
    ui->actionPrintPreview->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_P));
    ui->actionExit->setShortcuts(QKeySequence::Quit);

    ui->actionUndo->setShortcuts(QKeySequence::Undo);
    ui->actionRedo->setShortcuts(QKeySequence::Redo);
    ui->actionCut->setShortcuts(QKeySequence::Cut);
    ui->actionCopy->setShortcuts(QKeySequence::Copy);
    ui->actionPaste->setShortcuts(QKeySequence::Paste);
    ui->actionSelectAll->setShortcuts(QKeySequence::SelectAll);

    ui->actionSpell_checking->setShortcut(QKeySequence(Qt::ShiftModifier | Qt::Key_F7));

    ui->actionMarkdown_Syntax->setShortcuts(QKeySequence::HelpContents);

    if (ui->actionSaveAs->shortcuts().isEmpty())
        ui->actionSaveAs->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));
}

void MainWindow::closeCurrEditor()
{
    const int i = ui->tabWidget_2->currentIndex();

    if (i != -1)
        closeEditor(i);
}

void MainWindow::editorMoved(const int &from, const int &to)
{
    editorList.move(from, to);
}

void MainWindow::closeEditor(const int &index)
{
    overrideEditor = true;
    overrideVal = index;

    path = currentEditor()->getPath();

    if (path == tr("untitled.md"))
        path.clear();

    if (currentEditor()->document()->isModified()) {
        if (!onFileSave())
            return;
    }

    if (!path.isEmpty()) {
        watcher->removePath(path);
        fileList.removeOne(path);
    }

    overrideEditor = false;

    editorList.removeAt(index);
    ui->tabWidget_2->removeTab(index);

    if (editorList.isEmpty()) {
        html.clear();
        setText(ui->tabWidget->currentIndex());
        ui->actionReload->setText(tr("Reload \"%1\"").arg('\0'));
    }
}

void MainWindow::onEditorChanged(const int &index)
{
    MarkdownEditor* &&editor = currentEditor();
    if (!editor) return;

    setWindowTitle(editor->filePath());
    setWindowModified(editor->document()->isModified());
    ui->actionReload->setText(tr("Reload \"%1\"").arg(editor->getFileName()));

    ui->actionRedo->setEnabled(editor->document()->isRedoAvailable());
    ui->actionUndo->setEnabled(editor->document()->isUndoAvailable());

    path = editor->getPath();

    if (path == tr("untitled.md"))
        path.clear();

    onTextChanged();
}

MarkdownEditor *MainWindow::createEditor()
{
    MarkdownEditor *editor = new MarkdownEditor(this);
    editor->changeSpelling(spelling);
    editor->getChecker()->setMarkdownHighlightingEnabled(highlighting);

    if(ui->actionWord_wrap->isChecked())
        editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    else
        editor->setLineWrapMode(QPlainTextEdit::NoWrap);

    editorList.append(editor);

    connect(editor, &QPlainTextEdit::textChanged,
            this, &MainWindow::onTextChanged);
    connect(editor->document(), &QTextDocument::modificationChanged,
            ui->actionSave, &QAction::setEnabled);
    connect(editor->document(), &QTextDocument::modificationChanged,
            this, &QMainWindow::setWindowModified);
    connect(editor->document(), &QTextDocument::undoAvailable,
            ui->actionUndo, &QAction::setEnabled);
    connect(editor->document(), &QTextDocument::redoAvailable,
            ui->actionRedo, &QAction::setEnabled);
    connect(editor, &QMarkdownTextEdit::zoomIn, this,
            [editor]{ static_cast<QPlainTextEdit*>(editor)->zoomOut(-1); });
    connect(editor, &QMarkdownTextEdit::zoomOut, this,
            [editor]{ static_cast<QPlainTextEdit*>(editor)->zoomIn(-1); });

    return editor;
}

void MainWindow::receivedMessage(const qint32 &id, const QByteArray &msg)
{
    QString f = msg;
    f.remove(QLatin1String("file://"));

    if (f.isEmpty())
        onFileNew();
    else
        openFiles(f.split(QChar(QChar::Space)));
}

MarkdownEditor* MainWindow::currentEditor()
{
    if (overrideEditor)
        return editorList.at(overrideVal);
    else {
        if (editorList.isEmpty()) return nullptr;
        else
            return editorList.at(ui->tabWidget_2->currentIndex());
    }
}

void MainWindow::onFileChanged(const QString &f)
{
    if (!widgetReloadFile) {
        QGuiApplication::setOverrideCursor(Qt::WaitCursor);

        widgetReloadFile = new QWidget(this);
        widgetReloadFile->setStyleSheet(QLatin1String("background: orange"));

        horizontalLayout = new QHBoxLayout(widgetReloadFile);

        labelReloadFile = new QLabel(widgetReloadFile);
        labelReloadFile->setStyleSheet(QLatin1String("color: black"));

        horizontalLayout->addWidget(labelReloadFile);

        buttonBox = new QDialogButtonBox(widgetReloadFile);
        buttonBox->setStandardButtons(QDialogButtonBox::No | QDialogButtonBox::Yes);

        connect(buttonBox, &QDialogButtonBox::accepted,
                this, &MainWindow::onFileReload);
        connect(buttonBox, &QDialogButtonBox::rejected,
                widgetReloadFile, &QWidget::hide);

        horizontalLayout->addWidget(buttonBox);

        ui->verticalLayout_2->insertWidget(0, widgetReloadFile);

        QGuiApplication::restoreOverrideCursor();
    }

    labelReloadFile->setText(tr("File <em>%1</em> has been modified.\nWould you like to reload them?").arg(f));
    widgetReloadFile->show();

    reloadFile = f;

    watcher->addPath(f);
}

void MainWindow::loadIcons()
{
    loadIcon("application-exit", ui->actionExit);
    loadIcon("document-new", ui->actionNew);
    loadIcon("document-open-recent", ui->actionOpen_last_document_on_start);
    loadIcon("document-open", ui->actionOpen);
    loadIcon("document-print-preview", ui->actionPrintPreview);
    loadIcon("document-print", ui->actionPrint);
    loadIcon("document-save-as", ui->actionSaveAs);
    loadIcon("document-save", ui->actionSave);
    loadIcon("edit-copy", ui->actionCopy);
    loadIcon("edit-cut", ui->actionCut);
    loadIcon("edit-paste", ui->actionPaste);
    loadIcon("edit-redo", ui->actionRedo);
    loadIcon("edit-select-all", ui->actionSelectAll);
    loadIcon("edit-undo", ui->actionUndo);
    loadIcon("edit-copy", ui->actionCopy);
    loadIcon("help-about", ui->actionAbout);
    loadIcon("help-contents", ui->actionMarkdown_Syntax);
    loadIcon("text-wrap", ui->actionWord_wrap);
    loadIcon("tools-check-spelling", ui->actionSpell_checking);
    loadIcon("document-revert", ui->actionReload);

    ui->actionExportHtml->setIcon(QIcon::fromTheme(QLatin1String("text-html"),
                                             QIcon(QLatin1String(":/icons/text-html_16.png"))));
    ui->actionExportPdf->setIcon(QIcon::fromTheme(QLatin1String("application-pdf"),
                                                   QIcon(QLatin1String(":/icons/application-pdf_16.png"))));

    ui->menuExport->setIcon(QIcon::fromTheme(QLatin1String("document-export"),
                                             QIcon(QLatin1String(":/icons/document-export.svg"))));
    ui->menuRecentlyOpened->setIcon(QIcon::fromTheme(QLatin1String("document-open-recent"),
                                                     QIcon(QLatin1String(":/icons/document-open-recent.svg"))));

    toolbutton->setIcon(ui->menuRecentlyOpened->icon());

    if (isDarkMode())
        setWindowIcon(QIcon(QLatin1String(":/Icon_dark.svg")));
    else
        setWindowIcon(QIcon(QLatin1String(":/Icon.svg")));
}

void MainWindow::loadIcon(const char* &&name, QAction* &a)
{
    a->setIcon(QIcon::fromTheme(QString::fromUtf8(name), QIcon(QString::fromLatin1(
                                                ":/icons/%1.svg").arg(QString::fromUtf8(name)))));
}

void MainWindow::onOrientationChanged(const Qt::ScreenOrientation &t)
{
    if (t == Qt::PortraitOrientation) {
        disablePreview(true);
    }
    else if (t == Qt::LandscapeOrientation) {
        disablePreview(false);
    }
}

void MainWindow::setText(const int &index)
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
    for (MarkdownEditor* &editor : editorList) {
        if (c)
            editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        else
            editor->setLineWrapMode(QPlainTextEdit::NoWrap);
    }

    ui->actionWord_wrap->setChecked(c);
}

void MainWindow::changeSpelling(const bool &checked)
{
#if defined(NO_SPELLCHECK)
    return;
#endif

    for (MarkdownEditor* &editor : editorList) {
        editor->changeSpelling(checked);
    }

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
    if (ui->textBrowser->hasFocus())
        ui->textBrowser->cut();
    else if (ui->raw->hasFocus())
        ui->raw->cut();
    else
        if (currentEditor())
            currentEditor()->cut();
}

void MainWindow::copy()
{
    if (ui->textBrowser->hasFocus())
        ui->textBrowser->copy();
    else if (ui->raw->hasFocus())
        ui->raw->copy();
    else
        if (currentEditor())
            currentEditor()->copy();
}

void MainWindow::paste()
{
    if (ui->textBrowser->hasFocus())
        ui->textBrowser->paste();
    else if (ui->raw->hasFocus())
        ui->raw->paste();
    else
        if (currentEditor())
            currentEditor()->paste();
}

void MainWindow::selectAll()
{
    if (ui->textBrowser->hasFocus())
        ui->textBrowser->selectAll();
    else if (ui->raw->hasFocus())
        ui->raw->selectAll();
    else
        if (currentEditor())
            currentEditor()->selectAll();
}

void MainWindow::undo()
{
    if (ui->raw->hasFocus())
        ui->raw->undo();
    else
        if (currentEditor())
            currentEditor()->undo();
}

void MainWindow::redo()
{
    if (ui->raw->hasFocus())
        ui->raw->redo();
    else
        if (currentEditor())
            currentEditor()->redo();
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

    for (MarkdownEditor* &editor : editorList) {
        editor->getChecker()->setMarkdownHighlightingEnabled(enabled);
    }

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
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    QPrinter printer(QPrinter::HighResolution);

    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested,
            this, &MainWindow::printPreview);

    QGuiApplication::restoreOverrideCursor();

    preview.exec();
#endif
}

void MainWindow::printPreview(QPrinter *printer)
{
#ifdef QT_NO_PRINTER
    Q_UNUSED(printer);
#else
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    ui->textBrowser->setHtml(html);
    ui->textBrowser->print(printer);

    QGuiApplication::restoreOverrideCursor();
#endif
}

void MainWindow::exportPdf()
{
    QFileDialog dialog(this, tr("Export Pdf"));
    dialog.setMimeTypeFilters({"application/pdf"});
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("pdf");
    if (dialog.exec() != QDialog::Accepted)
        return;

    const QString file = dialog.selectedFiles().at(0);

    if (file.isEmpty()) return;

    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(file);
    printer.setResolution(QPrinter::HighResolution);

    printPreview(&printer);

    statusBar()->show();
    statusBar()->showMessage(tr("Pdf exported to %1").arg(
                                 QDir::toNativeSeparators(file)), 10000);
    QTimer::singleShot(10000, statusBar(), &QStatusBar::hide);
}

void MainWindow::openInWebBrowser()
{
    QTemporaryFile f(this);

    const QString name = f.fileTemplate() + QLatin1String(".html");

    f.setFileTemplate(name);
    f.setAutoRemove(false);

    if (!f.open()) {
        qWarning() << "Could not create temporyry file: " << f.errorString();

        QMessageBox::warning(this, tr("Warning"),
                             tr("Could not create temporary file: %1").arg(
                                 f.errorString()));

        return;
    }

    QTextStream out(&f);
    out << html;

    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(f.fileName())))
        f.remove();
    else
        QTimer::singleShot(2000, this, [name]{
            QFile::remove(name);
        });
}

void MainWindow::exportHtml()
{
#if defined(Q_OS_WASM)
    QFileDialog::saveFileContent(html.toUtf8(), QLatin1String("Exported HTML.html"));
#else
    QFileDialog dialog(this, tr("Export HTML"));
    dialog.setMimeTypeFilters({"text/html"});
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("html");
    if (dialog.exec() != QDialog::Accepted)
        return;

    const QString file = dialog.selectedFiles().at(0);

    if (file.isEmpty()) return;

    QFile f(file, this);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))  {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Could not write to file %1: %2").arg(
                                 QDir::toNativeSeparators(path), f.errorString()));
        return;
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    QTextStream str(&f);
    str << html;

    statusBar()->show();
    statusBar()->showMessage(tr("HTML exported to %1").arg(QDir::toNativeSeparators(file)), 10000);
    QTimer::singleShot(10000, statusBar(), &QStatusBar::hide);

    QGuiApplication::restoreOverrideCursor();
#endif
}

void MainWindow::changeMode(const int &i)
{
    _mode = i;
    onTextChanged();
}

void MainWindow::onTextChanged()
{
    if (dontUpdate || !currentEditor())
        return;

    html = Parser::toHtml(currentEditor()->document()->toPlainText(), _mode);

    setText(ui->tabWidget->currentIndex());
}

void MainWindow::openFile(const QString &newFile)
{
    if (fileList.contains(newFile)) {
        for (int i = 0; editorList.length(); i++) {
            if (editorList.at(i)->getPath() == newFile) {
                ui->tabWidget_2->setCurrentIndex(i);
                return;
            }
        }
    }

    QFile f(newFile);

    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Couldn't open file"),
                             tr("Could not open file %1: %2").arg(
                                 QDir::toNativeSeparators(newFile), f.errorString()));
        recentOpened.removeOne(newFile);
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
    watcher->addPath(path);

    if (setPath)
        ui->textBrowser->setSearchPaths(QStringList() << QFileInfo(newFile).path());

    MarkdownEditor* &&editor = createEditor();
    editor->setFile(newFile);
    ui->tabWidget_2->insertTab(editorList.length() -1,
                               editor, editor->getFileName());
    ui->tabWidget_2->setCurrentIndex(editorList.length() -1);
    ui->tabWidget_2->tabBar()->setTabToolTip(editorList.length() -1, newFile);

    QCoreApplication::processEvents();

    editor->setText(f.readAll(), newFile);

    setWindowTitle(editor->filePath());
    ui->actionReload->setText(tr("Reload \"%1\"").arg(editor->getFileName()));

    fileList.append(newFile);

    statusBar()->show();
    statusBar()->showMessage(tr("Opened %1").arg(QDir::toNativeSeparators(path)), 10000);
    QTimer::singleShot(10000, statusBar(), &QStatusBar::hide);

    updateOpened();

    QGuiApplication::restoreOverrideCursor();
}

void MainWindow::onFileNew()
{
    path = QLatin1String();
    const QString file = tr("untitled.md");

    MarkdownEditor* &&editor = createEditor();
    editor->setFile(file);

    ui->tabWidget_2->insertTab(editorList.length() -1,
                               editor, editor->getFileName());
    ui->tabWidget_2->setCurrentIndex(editorList.length() -1);

    if (!editor->setLanguage(QLatin1String("en-US")))
            editor->setLanguage();
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

            MarkdownEditor* &&editor = createEditor();
            editor->setFile(newFile);
            ui->tabWidget_2->insertTab(editorList.length() -1,
                                       editor, editor->getFileName());
            ui->tabWidget_2->setCurrentIndex(editorList.length() -1);
            editor->setText(fileContent, newFile);

            setWindowTitle(editor->filePath());
            ui->actionReload->setText(tr("Reload \"%1\"").arg(editor->getFileName()));

            fileList.append(newFile);

            statusBar()->show();
            statusBar()->showMessage(tr("Opened %1").arg(QDir::toNativeSeparators(path)), 30000);
            QTimer::singleShot(10000, statusBar(), &QStatusBar::hide);

            updateOpened();

            QGuiApplication::restoreOverrideCursor();
        }
    };
    QFileDialog::getOpenFileContent("Markdown (*.md *.markdown *.mkd)", fileContentReady);
#else
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

bool MainWindow::onFileSave()
{
    if (!currentEditor()->document()->isModified())
        if (QFile::exists(path))
            return true;

    if (path.isEmpty() || path == QLatin1String(":/default.md")) {
        return onFileSaveAs();
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

#if defined(Q_OS_WASM)
    QFileDialog::saveFileContent(currentEditor()->toPlainText().toUtf8(), path);
#else

    watcher->removePath(path);

    QSaveFile f(path, this);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::warning(this, tr("Warning"),
                             tr("Could not write to file %1: %2").arg(
                                 QDir::toNativeSeparators(path), f.errorString()));
        return false;
    }

    QTextStream str(&f);
    str << currentEditor()->toPlainText();

    if (!f.commit()) {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::warning(this, tr("Warning"),
                             tr("Could not write to file %1: %2").arg(
                                 QDir::toNativeSeparators(path), f.errorString()));
        return false;
    }

    watcher->addPath(path);
#endif

    statusBar()->show();
    statusBar()->showMessage(tr("Wrote %1").arg(QDir::toNativeSeparators(path)), 30000);
    QTimer::singleShot(10000, statusBar(), &QStatusBar::hide);

    updateOpened();

    currentEditor()->document()->setModified(false);

    QGuiApplication::restoreOverrideCursor();

    return true;
}

bool MainWindow::onFileSaveAs()
{
    QFileDialog dialog(this, tr("Save MarkDown File"));
    dialog.setMimeTypeFilters({"text/markdown"});
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("md");
    if (dialog.exec() == QDialog::Rejected)
        return false;

    const QString file = dialog.selectedFiles().at(0);
    const QString fileName = QFileInfo(file).fileName();

    if (file.isEmpty())
        return false;
    else
        path = file;

    if (!(path.endsWith(QLatin1String(".md")) || path.endsWith(QLatin1String(".markdown")) || path.endsWith(QLatin1String(".mkd"))))
        path.append(QLatin1String(".md"));

    watcher->addPath(file);

    if (fileList.contains(file))
        openFile(file);

    setMapAttribute(path, currentEditor()->getChecker()->getLanguage());

    ui->tabWidget_2->tabBar()->setTabText(editorList.length() -1, fileName);
    ui->tabWidget_2->tabBar()->setTabToolTip(editorList.length() -1, file);

    currentEditor()->setFile(file);
    setWindowTitle(currentEditor()->filePath());

    return onFileSave();
}

void MainWindow::onHelpAbout()
{
    About dialog(tr("About MarkdownEdit"), this);
    dialog.setAppUrl("https://software-made-easy.github.io/MarkdownEdit/");
    dialog.setDescription(tr("MarkdownEdit, as the name suggests, is a simple and easy program to create and edit Markdown files."));

    dialog.addCredit(tr("<p>The conversion from Markdown to HTML is done with the help of the <a href=\"https://github.com/mity/md4c\">md4c</a> library by <em>Martin Mitáš</em>.</p>"));
    dialog.addCredit(tr("<p>The <a href=\"https://github.com/pbek/qmarkdowntextedit\">widget</a> used for writing was created by <em>Patrizio Bekerle</em>.</p>"));

    dialog.exec();
}

void MainWindow::openRecent() {
    // A QAction represents the action of the user clicking
    const QString filename = qobject_cast<QAction *>(sender())->data().toString();

    if (!QFile::exists(filename)) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("This file could not be found:\n%1.").arg(
                                 QDir::toNativeSeparators(filename)));
        recentOpened.removeOne(filename);
        updateOpened();
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

    recentOpened.removeDuplicates();

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
    for(MarkdownEditor* &editor : editorList) {
        if (editor->document()->isModified()) {
            const int button = QMessageBox::question(this, tr("Save changes?"),
                                               tr("The file <em>%1</em> has been changed.\n"
                                                  "Do you want to leave anyway?").arg(editor->getFileName()));
            if (button == QMessageBox::No) {
                e->ignore();
                return;
            }
        }
    }

    saveSettings();
    e->accept();
    QMainWindow::closeEvent(e);
}

void MainWindow::loadSettings(const QStringList &files) {
    const QByteArray geo = settings->value(QLatin1String("geometry"),
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

    restoreState(settings->value(QLatin1String("state"), QByteArrayLiteral("")).toByteArray());

    highlighting = settings->value(QLatin1String("highlighting"), true).toBool();
    ui->actionHighlighting_activated->setChecked(highlighting);
    changeHighlighting(highlighting);

    setPath = settings->value(QLatin1String("setPath"), true).toBool();
    changeAddtoIconPath(setPath);

    recentOpened = settings->value(QLatin1String("recent"), QStringList()).toStringList();
    if (!recentOpened.isEmpty()) {
        if (recentOpened.first().isEmpty()) {
            recentOpened.takeFirst();
        }
    }

    const bool lineWrap = settings->value(QLatin1String("lineWrap"), false).toBool();
    changeWordWrap(lineWrap);

    setLanguageMap(settings->value(QLatin1String("languagesMap"),
                                   QMap<QString, QVariant>()).toMap());

    spelling = settings->value(QLatin1String("spelling"), true).toBool();
    changeSpelling(spelling);

    if (files.isEmpty()) {
        const bool openLast = settings->value(QLatin1String("openLast"), true).toBool();
        if (openLast) {
            ui->actionOpen_last_document_on_start->setChecked(openLast);

            const QString last = settings->value(QLatin1String("last"),
                                               QLatin1String()).toString();
            if (!last.isEmpty())
                openFile(last);
            else
                onFileNew();
        }
    }
    else
        openFiles(files);

    if (editorList.length() > 0)
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
    settings->setValue("languagesMap", getLanguageMap());
    settings->sync();
}

MainWindow::~MainWindow()
{
    delete ui;
}
