/**
 ** This file is part of the MarkdownEdit project.
 ** Copyright 2022 Tim Gromeyer <sakul8825@gmail.com>.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/


// imports
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "markdownparser.h"
#include "highlighter.h"
#include "spellchecker.h"
#include "settings.h"
#include "markdowneditor.h"

// qt imports
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
#include <QLabel>
#include <QDialogButtonBox>
#include <QHBoxLayout>

#ifdef QT_PRINTSUPPORT_LIB
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#endif


MainWindow::MainWindow(const QStringList &files, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->toolBarTools->hide();

    setupThings(); // Setup things

    // // settings was set up in setupThings()
    loadSettings(); // Load settings

    setupConnections(); // Connect everything

    ui->File->addAction(ui->actionNew);
    ui->File->addAction(ui->actionOpen);
    ui->File->addAction(ui->actionSave);
#ifndef Q_OS_WASM
    ui->File->addSeparator();
    ui->File->addWidget(toolbutton);
#endif
    ui->Edit->addAction(ui->actionUndo);
    ui->Edit->addAction(ui->actionRedo);
#ifndef Q_OS_ANDROID // Save space
    ui->Edit->addSeparator();
    ui->Edit->addAction(ui->actionCut);
    ui->Edit->addAction(ui->actionCopy);
    ui->Edit->addAction(ui->actionPaste);
    ui->toolBarPreview->addWidget(mode);
    ui->toolBarPreview->addSeparator();
#endif
    ui->toolBarPreview->addWidget(widgetBox);

#ifdef NO_SPELLCHECK
    ui->menuExtras->removeAction(ui->actionSpell_checking);
#endif
#ifdef Q_OS_WASM
    ui->menuExport->removeAction(ui->actionExportPdf);
    ui->menuFile->removeAction(ui->actionPrint);
    ui->menuFile->removeAction(ui->actionPrintPreview);

    // Doesnt work on wasm
    ui->menuFile->removeAction(ui->menuRecentlyOpened->menuAction());
    ui->menuRecentlyOpened->setDisabled(true);
    toolbutton->deleteLater();

    ui->menuExtras->removeAction(ui->actionOpen_last_document_on_start);

    ui->menu_View->removeAction(ui->actionOpen_in_web_browser);
    ui->menu_View->removeAction(ui->actionAuto_add_file_path_to_icon_path);
#endif
#ifdef Q_OS_ANDROID
    mode->deleteLater();
    setStatusBar(nullptr);
    menuBar()->hide();
    ui->File->setIconSize(QSize(32, 32));
    ui->Edit->setIconSize(QSize(32, 32));
    ui->toolBarPreview->setIconSize(QSize(32, 32));
    ui->File->setMovable(false);
    ui->Edit->setMovable(false);
    ui->toolBarPreview->setMovable(false);
#endif

    QMetaObject::invokeMethod(this, [files, this]{
        // Moved here for faster startup times
        loadIcons(); // Load all icons

        loadFiles(files);
    }, Qt::QueuedConnection);
}

void MainWindow::onHelpSyntax()
{
    QString file = QStringLiteral(":/syntax_en.md");
    QString language = QStringLiteral("en_US");

    const QStringList uiLanguages = QLocale::system().uiLanguages(); // eg. de-DE
    const QStringList languages = SpellChecker::getLanguageList(); // eg. de_DE

    // loop thought the languages
    for (const QString &lang : uiLanguages) {
        //  get first to characters
        const QString lang2 = lang.right(2);

        if (QFile::exists(QStringLiteral(":/syntax_%1.md").arg(lang2))) {
            file = QStringLiteral(":/syntax_%1.md").arg(lang2);
            language = lang;
            break;
        }
    }

    // Correct language format
    language.replace(QLatin1Char('-'), QLatin1Char('_'));

    if (!languages.contains(language)) {
        if (language.length() > 2)
            // Get the first 2 characters
            language = language.right(2);

        // Assuming language = de, try de_DE
        const QString newLang = QStringLiteral("%1_%2").arg(language, language.toUpper());
        // If the languagedict exists
        if (languages.contains(newLang)) {
            language = newLang;
        }

        // Loop thought the languages
        for (const QString &lang : languages) {
            // If lang contains language, then break out of the loop
            Q_UNUSED(lang); // Prevent cppcheck info

            if (lang.contains(language)) {
                language = lang;
                break;
            }
        }
    }

    openFile(file, language);
}

void MainWindow::toForeground()
{
    raise();
    activateWindow();
}

void MainWindow::onFileReload()
{
    QObject *widget = sender()->parent();

    const QString file = sender()->property("file").toString();

    if (file.isEmpty())
        return;

    QFile f(file);

    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Couldn't open file"),
                             tr("Could not open file <em>%1</em>: %2").arg(
                                 QDir::toNativeSeparators(file), f.errorString()));
        recentOpened.removeOne(file);
        updateOpened();
        return;
    }

    for (MarkdownEditor* editor : qAsConst(editorList)) {
        if (!editor)
            return;

        if (editor->getPath() == file) {
            editor->setText(f.readAll(), QLatin1String(), false);
            break;
        }
    }

    if (widget->objectName() == QLatin1String("widgetReloadFile")) {
        widget->deleteLater();
        delete widget;
    }
}

void MainWindow::setupThings()
{
    // Setup a toolbutton to acces the
    // recently opened menu from the toolbar
    toolbutton = new QToolButton(this);
    toolbutton->setMenu(ui->menuRecentlyOpened);
    toolbutton->setPopupMode(QToolButton::InstantPopup);

    // Use native menu bar
    menuBar()->setNativeMenuBar(true);

    // Setup shortcuts to open and close tabs
    shortcutNew = new QShortcut(this);
    shortcutClose = new QShortcut(this);

    // Setup a file watcher
    watcher = new QFileSystemWatcher(this);

    // Setup settings to load settings
    settings = new QSettings(QStringLiteral("SME"),
                             QStringLiteral("MarkdownEdit"), this);

    // Setup ComboBox to choose between Commonmark and GitHub
    mode = new QComboBox(ui->Edit);
    mode->addItems(QStringList() << QStringLiteral("Commonmark") << QStringLiteral("GitHub"));
    mode->setCurrentIndex(1);

    // Setup ComboBox to choose between Preview and Raw HTML
    widgetBox = new QComboBox(this);
    widgetBox->addItems(QStringList() << tr("Preview") << tr("HTML"));
    widgetBox->setCurrentIndex(0);

    // Setup the HTML Highliter
    htmlHighliter = new Highliter(ui->raw->document());

    // Disable preview when Portrait mode is active
    QScreen *screen = qApp->primaryScreen();
    onOrientationChanged(screen->primaryOrientation());
    connect(screen, &QScreen::primaryOrientationChanged,
            this, &MainWindow::onOrientationChanged);

    // Disable actions which doesnt work yet
    ui->actionSave->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    ui->actionRedo->setEnabled(false);

    // Add shortcuts
#ifndef Q_OS_ANDROID
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
    ui->actionPrintPreview->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_O));
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
#endif
}

void MainWindow::setupConnections()
{
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
            this, &MainWindow::onHelpSyntax);
    connect(ui->tabWidget_2->tabBar(), &QTabBar::tabMoved,
            this, &MainWindow::editorMoved);
    connect(watcher, &QFileSystemWatcher::fileChanged,
            this, &MainWindow::onFileChanged);
    connect(ui->tabWidget_2, &QTabWidget::tabCloseRequested,
            this, &MainWindow::closeEditor);
}

void MainWindow::closeCurrEditor()
{
    const int i = ui->tabWidget_2->currentIndex();

    if (i != -1)
        closeEditor(i);
}

void MainWindow::editorMoved(const int from, const int to)
{
    editorList.move(from, to);
}

void MainWindow::closeEditor(const int index)
{
    overrideEditor = true;
    overrideVal = index;

    path = currentEditor()->getPath();

    if (path == tr("Untitled.md")) {
        path.clear();
        if (currentEditor()->document()->isModified()) {
            QMessageBox d(this);
            d.setText(tr("The document has been edited, do you want to save it?"));
            d.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Abort);
            d.setDefaultButton(QMessageBox::Yes);
            d.setIcon(QMessageBox::Question);

            const int retVal = d.exec();

            if (retVal == QMessageBox::Abort) {
                overrideEditor = false;
                return;
            }
            if (retVal == QMessageBox::Yes) {
                if (!onFileSave()) {
                    overrideEditor = false;
                    return;
                }
            }
        }
    }
    else if (currentEditor()->document()->isModified()) {
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

    if (editorList.isEmpty()) { // If all editors are closed
        html.clear(); // Clear html
        setText(ui->tabWidget->currentIndex()); // apply the empty html to the preview widget
        ui->actionReload->setText(tr("Reload \"%1\"").arg('\0'));
        ui->actionReload->setEnabled(false);
    }
}

void MainWindow::onEditorChanged(const int index)
{
    MarkdownEditor* editor = editorList[index];
    if (!editor) return;

    const bool modified = editor->document()->isModified();

    setWindowTitle(editor->filePath());
    setWindowModified(modified);
    ui->actionSave->setEnabled(modified);

    ui->actionRedo->setEnabled(editor->document()->isRedoAvailable());
    ui->actionUndo->setEnabled(editor->document()->isUndoAvailable());

    path = editor->getPath();

    if (path == tr("Untitled.md")) {
        path.clear();
        ui->actionReload->setText(tr("Reload \"%1\"").arg('\0'));
        ui->actionReload->setEnabled(false);
    }
    else {
        ui->actionReload->setText(tr("Reload \"%1\"").arg(editor->getFileName()));
        ui->actionReload->setEnabled(true);
        ui->actionReload->setProperty("file", path);
    }

#ifdef Q_OS_WASM
    ui->actionReload->setEnabled(false);
#endif

    setCurrDir(editor->getDir());

    editor->setFocus(Qt::TabFocusReason);

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
            this, &MainWindow::onModificationChanged);
    connect(editor->document(), &QTextDocument::undoAvailable,
            ui->actionUndo, &QAction::setEnabled);
    connect(editor->document(), &QTextDocument::redoAvailable,
            ui->actionRedo, &QAction::setEnabled);
    connect(editor->horizontalScrollBar(), &QScrollBar::valueChanged,
            ui->textBrowser->horizontalScrollBar(), &QScrollBar::setValue);

    return editor;
}

void MainWindow::receivedMessage(const qint32 id, const QByteArray &msg)
{
    QString f = QString::fromLatin1(msg); // is a bit faster
    f.remove(0, 7); // remove "file://" which is added programaticly

    qInfo() << "markdownedit: instance" << id << "started and send folowing message:" << f;

    if (f.isEmpty()) // it's the case when you start a new app
        onFileNew();
    else
        openFiles(f.split(QChar(QChar::Space))); // if you selected files
}

void MainWindow::onModificationChanged(const bool m)
{
    setWindowModified(m);

    const int curr = ui->tabWidget_2->currentIndex();

    QString old = ui->tabWidget_2->tabBar()->tabText(curr);

    if (m && !old.endsWith(QChar('*')))
        old.append(QChar('*'));
    else if (!m && old.endsWith(QChar('*')))
        old.remove(old.length() -1, 1);

    ui->tabWidget_2->tabBar()->setTabText(curr, old);
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
    QWidget *widgetReloadFile = new QWidget(this);
    widgetReloadFile->setStyleSheet(QStringLiteral("background: orange"));
    widgetReloadFile->setObjectName(QStringLiteral("widgetReloadFile"));

    QHBoxLayout* horizontalLayout = new QHBoxLayout(widgetReloadFile);

    QLabel *labelReloadFile = new QLabel(widgetReloadFile);
    labelReloadFile->setStyleSheet(QStringLiteral("color: black"));

    horizontalLayout->addWidget(labelReloadFile);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(widgetReloadFile);
    buttonBox->setStandardButtons(QDialogButtonBox::No | QDialogButtonBox::Yes);
    buttonBox->setProperty("file", f);

    connect(buttonBox, &QDialogButtonBox::accepted,
            this, &MainWindow::onFileReload);
    connect(buttonBox, &QDialogButtonBox::rejected,
            widgetReloadFile, &QWidget::hide);

    horizontalLayout->addWidget(buttonBox);

    labelReloadFile->setText(tr("File <em>%1</em> has been modified.\n"
                                "Would you like to reload them?").arg(f));

    ui->verticalLayout_2->insertWidget(0, widgetReloadFile);
    widgetReloadFile->show();

    watcher->addPath(f);
}

void MainWindow::loadIcons()
{
#define S(x) QStringLiteral(x)
    loadIcon(S("application-exit"), ui->actionExit);
    loadIcon(S("document-new"), ui->actionNew);
    loadIcon(S("document-open-recent"), ui->actionOpen_last_document_on_start);
    loadIcon(S("document-open"), ui->actionOpen);
    loadIcon(S("document-print-preview"), ui->actionPrintPreview);
    loadIcon(S("document-print"), ui->actionPrint);
    loadIcon(S("document-save-as"), ui->actionSaveAs);
    loadIcon(S("document-save"), ui->actionSave);
    loadIcon(S("edit-copy"), ui->actionCopy);
    loadIcon(S("edit-cut"), ui->actionCut);
    loadIcon(S("edit-paste"), ui->actionPaste);
    loadIcon(S("edit-redo"), ui->actionRedo);
    loadIcon(S("edit-select-all"), ui->actionSelectAll);
    loadIcon(S("edit-undo"), ui->actionUndo);
    loadIcon(S("edit-copy"), ui->actionCopy);
    loadIcon(S("help-about"), ui->actionAbout);
    loadIcon(S("help-contents"), ui->actionMarkdown_Syntax);
    loadIcon(S("text-wrap"), ui->actionWord_wrap);
    loadIcon(S("tools-check-spelling"), ui->actionSpell_checking);
    loadIcon(S("document-revert"), ui->actionReload);
    loadIcon(S("text-wrap"), ui->actionWord_wrap);
    loadIcon(S("media-playback-pause"), ui->actionPause_preview);

    ui->actionExportHtml->setIcon(QIcon::fromTheme(S("text-html"),
                                             QIcon(S(":/icons/text-html_16.png"))));
    ui->actionExportPdf->setIcon(QIcon::fromTheme(S("application-pdf"),
                                                   QIcon(S(":/icons/application-pdf_16.png"))));

    ui->menuExport->setIcon(QIcon::fromTheme(S("document-export"),
                                             QIcon(S(":/icons/document-export.svg"))));
    ui->menuRecentlyOpened->setIcon(QIcon::fromTheme(S("document-open-recent"),
                                                     QIcon(S(":/icons/document-open-recent.svg"))));

    toolbutton->setIcon(ui->menuRecentlyOpened->icon());

    setWindowIcon(QIcon(S(":/Icon.png")));
#undef S
}

void MainWindow::loadIcon(const QString &name, QAction* a)
{
    a->setIcon(QIcon::fromTheme(name, QIcon(QLatin1String(":/icons/") + name + QLatin1String(".svg"))));
}

void MainWindow::onOrientationChanged(const Qt::ScreenOrientation t)
{
    disablePreview(t == Qt::PortraitOrientation);
}

// FIXME: Performance problems occur at this point.
void MainWindow::setText(const int index)
{
    if (index == 0) {
        ui->textBrowser->setHtml(html);
    }
    else {
        const int v = ui->raw->verticalScrollBar()->value();
        ui->raw->document()->setPlainText(html);
        ui->raw->verticalScrollBar()->setValue(v);
    }
}

void MainWindow::changeWordWrap(const bool c)
{
    for (MarkdownEditor* editor : qAsConst(editorList)) {
        if (c)
            editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        else
            editor->setLineWrapMode(QPlainTextEdit::NoWrap);
    }

    if (c) {
        ui->textBrowser->setLineWrapMode(QTextBrowser::WidgetWidth);
        ui->raw->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    }
    else {
        ui->textBrowser->setLineWrapMode(QTextBrowser::NoWrap);
        ui->raw->setLineWrapMode(QPlainTextEdit::NoWrap);
    }

    ui->actionWord_wrap->setChecked(c);
}

void MainWindow::changeSpelling(const bool checked)
{
#if defined(NO_SPELLCHECK)
    return;
#else

    for (MarkdownEditor* editor : qAsConst(editorList)) {
        editor->changeSpelling(checked);
    }

    ui->actionSpell_checking->setChecked(checked);
    spelling = checked;
#endif
}

void MainWindow::disablePreview(const bool checked)
{
    ui->tabWidget->setHidden(checked);

    dontUpdate = checked;

    ui->actionPause_preview->setChecked(checked);
    ui->actionPause_preview->setDisabled(checked);

    ui->actionDisable_preview->setChecked(checked);

    if (!checked) {
        onTextChanged();
        ui->actionDisable_preview->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-stop"),
                                                          QIcon(QStringLiteral(":/icons/media-playback-stop.svg"))));
    }
    else
        ui->actionDisable_preview->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-start"),
                                                          QIcon(QStringLiteral(":/icons/media-playback-start.svg"))));
}

void MainWindow::pausePreview(const bool checked)
{
    dontUpdate = checked;
    ui->tabWidget->setDisabled(checked);

    if (!checked) {
        onTextChanged();
        ui->actionPause_preview->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-pause"),
                                                          QIcon(QStringLiteral(":/icons/media-playback-pause.svg"))));
    }
    else
        ui->actionPause_preview->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-start"),
                                                          QIcon(QStringLiteral(":/icons/media-playback-start.svg"))));
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

void MainWindow::changeAddtoIconPath(const bool c)
{
    setPath = c;

    ui->actionAuto_add_file_path_to_icon_path->setChecked(c);

    if (!c) {
        ui->textBrowser->setSearchPaths(QStringList());
        return setText(0);
    }

    QStringList searchPaths;

    for (const QString &file : qAsConst(fileList)) {
        searchPaths << QFileInfo(file).absolutePath();
    }

    ui->textBrowser->setSearchPaths(searchPaths);
    setText(0);
}

void MainWindow::changeHighlighting(const bool enabled)
{
    dontUpdate = true;

    for (MarkdownEditor* editor : qAsConst(editorList)) {
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

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(file);

    ui->textBrowser->setHtml(html);
    ui->textBrowser->print(&printer);

#ifndef Q_OS_ANDROID
    statusBar()->show();
    statusBar()->showMessage(tr("Pdf exported to <em>%1</em>").arg(
                                 QDir::toNativeSeparators(file)), 0);
    QTimer::singleShot(5000, statusBar(), &QStatusBar::hide);
#endif
}

void MainWindow::openInWebBrowser()
{
    QTemporaryFile f(this);

    const QString name = f.fileTemplate() + QStringLiteral(".html");

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

    QDesktopServices::openUrl(QUrl::fromLocalFile(f.fileName()));
    const QString file = f.fileName();

    // Delete the file file after 5s
    QTimer::singleShot(5000, this, [file]{
        QFile::remove(file);
    });
}

void MainWindow::exportHtml()
{
#if defined(Q_OS_WASM)
    QFileDialog::saveFileContent(html.toUtf8(), QStringLiteral("Exported HTML.html"));
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
                             tr("Could not write to file <em>%1</em>: %2").arg(
                                 QDir::toNativeSeparators(path), f.errorString()));
        return;
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    QTextStream str(&f);
    str << html;

#ifndef Q_OS_ANDROID
    statusBar()->show();
    statusBar()->showMessage(tr("HTML exported to <em>%1</em>").arg(QDir::toNativeSeparators(file)), 0);
    QTimer::singleShot(5000, statusBar(), &QStatusBar::hide);
#endif

    QGuiApplication::restoreOverrideCursor();
#endif
}

void MainWindow::changeMode(const int i)
{
    _mode = i;
    onTextChanged();
}

void MainWindow::onTextChanged()
{
    // Don't continue if the preview is paused or the current editor is null.
    if (dontUpdate || !currentEditor())
        return;

    // Genarate HTML from Markdown
    html = Parser::toHtml(currentEditor()->document()->toPlainText(), _mode);

    // Apply the HTML to the visible widget
    setText(ui->tabWidget->currentIndex());
}

void MainWindow::loadFiles(const QStringList &files)
{
    // Ensure settings are set up
    const bool openLast = settings->value(QStringLiteral("openLast"), true).toBool();
    ui->actionOpen_last_document_on_start->setChecked(openLast);


    if (files.isEmpty()) {
        const QString last = settings->value(QStringLiteral("last"),
                                           QLatin1String()).toString();
        if (openLast && !last.isEmpty())
            openFile(last);
        else {
            onFileNew();
            updateOpened();
        }
    }
    else
        openFiles(files);
}

void MainWindow::openFiles(const QStringList &files)
{
    for (const QString &file : files) {
        openFile(QFileInfo(file).absoluteFilePath()); // ensure filepath is absolute
    }
}

void MainWindow::openFile(const QString &newFile, const QString &lang)
{
    // Check of file is already open
    if (fileList.contains(newFile)) {
        for (auto i = 0; editorList.length() > i; i++) {
            if (editorList.at(i)->getPath() == newFile) {
                ui->tabWidget_2->setCurrentIndex(i);
                return;
            }
        }
    }

    if (!lang.isEmpty())
        setMapAttribute(newFile, lang);

    QFile f(newFile);

    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Couldn't open file"),
                             tr("Could not open file <em>%1</em>: %2").arg(
                                 QDir::toNativeSeparators(newFile), f.errorString()));
        recentOpened.removeOne(newFile);
        updateOpened();
        return;
    }
    constexpr int limit = 500000; // 500KB
    if (f.size() > limit) {
        const auto out = QMessageBox::warning(this, tr("Large file"),
                                             tr("This is a large file that can potentially cause performance issues."),
                                             QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);

        if (out == QMessageBox::Cancel)
            return;

        pausePreview(true);
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    blockSignals(true);

    path = newFile;
    watcher->addPath(path);

    if (setPath)
        ui->textBrowser->setSearchPaths(ui->textBrowser->searchPaths() << QFileInfo(newFile).path());

    auto editor = createEditor();
    editor->setFile(newFile);
    ui->tabWidget_2->insertTab(editorList.length() -1,
                               editor, editor->getFileName());
    ui->tabWidget_2->setCurrentIndex(editorList.length() -1);
    ui->tabWidget_2->tabBar()->setTabToolTip(editorList.length() -1,
                                             QDir::toNativeSeparators(newFile));

    editor->setText(f.readAll(), newFile);

    setWindowTitle(editor->filePath());
    ui->actionReload->setText(tr("Reload \"%1\"").arg(editor->getFileName()));

    fileList.append(newFile);

    updateOpened();

#ifndef Q_OS_ANDROID
    statusBar()->show();
    statusBar()->showMessage(tr("Opened <em>%1</em>").arg(QDir::toNativeSeparators(path)), 0);
    QTimer::singleShot(5000, statusBar(), &QStatusBar::hide);
#endif

    blockSignals(false);

    QGuiApplication::restoreOverrideCursor();
}

void MainWindow::onFileNew()
{
    path.clear();
    const QString file = tr("Untitled.md");

    auto editor = createEditor();
    editor->setFile(file);

    ui->tabWidget_2->insertTab(editorList.length() -1,
                               editor, editor->getFileName());
    ui->tabWidget_2->setCurrentIndex(editorList.length() -1);

    if (!editor->setLanguage(QLocale::system().name()))
        editor->setLanguage(QLatin1String("en_US"));

#ifndef Q_OS_ANDROID
    statusBar()->show();
    statusBar()->showMessage(tr("New document created"), 0);
    QTimer::singleShot(5000, statusBar(), &QStatusBar::hide);
#endif
}

void MainWindow::onFileOpen()
{
#if defined(Q_OS_WASM)
    static auto fileContentReady = [this](const QString &newFile, const QByteArray &fileContent) {
        if (!newFile.isEmpty()) {
            QGuiApplication::setOverrideCursor(Qt::WaitCursor);

            path = newFile;

            if (setPath)
                ui->textBrowser->setSearchPaths(ui->textBrowser->searchPaths() << QFileInfo(newFile).path());

            auto editor = createEditor();
            editor->setFile(newFile);
            ui->tabWidget_2->insertTab(editorList.length() -1,
                                       editor, editor->getFileName());
            ui->tabWidget_2->setCurrentIndex(editorList.length() -1);
            editor->setText(fileContent, newFile);

            setWindowTitle(editor->filePath());
            ui->actionReload->setText(tr("Reload \"%1\"").arg(editor->getFileName()));

            fileList.append(newFile);

            statusBar()->show();
            statusBar()->showMessage(tr("Opened <em>%1</em>").arg(QDir::toNativeSeparators(path)), 0);
            QTimer::singleShot(5000, statusBar(), &QStatusBar::hide);

            updateOpened();

            QGuiApplication::restoreOverrideCursor();
        }
    };
    QFileDialog::getOpenFileContent("Markdown (*.md *.markdown *.mkd)", fileContentReady);
#else
    QFileDialog dialog(this, tr("Open Markdown File"));
    dialog.setMimeTypeFilters({"text/markdown"});
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    if (dialog.exec() == QDialog::Rejected) return;

    const QString file = dialog.selectedFiles().at(0);
    if (file == path || file.isEmpty()) return;

    openFile(file);
#endif
}

bool MainWindow::onFileSave()
{
    if (!currentEditor()->document()->isModified())
        if (QFile::exists(path))
            return true;

    if (path.isEmpty() || path.startsWith(QLatin1String(":/syntax_")))
        return onFileSaveAs();

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

#if defined(Q_OS_WASM)
    QFileDialog::saveFileContent(currentEditor()->toPlainText().toUtf8(), path);
#else

    watcher->removePath(path);

    QSaveFile f(path, this);
    f.setDirectWriteFallback(true);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::warning(this, tr("Warning"),
                             tr("Could not write to file <em>%1</em>: %2").arg(
                                 QDir::toNativeSeparators(path), f.errorString()));
        return false;
    }

    QTextStream str(&f);
    str << currentEditor()->toPlainText();

    if (!f.commit()) {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::warning(this, tr("Warning"),
                             tr("Could not write to file <em>%1</em>: %2").arg(
                                 QDir::toNativeSeparators(path), f.errorString()));
        return false;
    }

    watcher->addPath(path);
#endif

#ifndef Q_OS_ANDROID
    statusBar()->show();
    statusBar()->showMessage(tr("Wrote <em>%1</em>").arg(QDir::toNativeSeparators(path)), 0);
    QTimer::singleShot(5000, statusBar(), &QStatusBar::hide);
#endif

    updateOpened();

    currentEditor()->document()->setModified(false);

    QGuiApplication::restoreOverrideCursor();

    return true;
}

bool MainWindow::onFileSaveAs()
{
#ifdef Q_OS_WASM
    path = QLatin1String("Markdown.md");
    return onFileSave();
#else

    QFileDialog dialog(this, tr("Save MarkDown File"));
    dialog.setMimeTypeFilters({"text/markdown"});
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("md");
    if (dialog.exec() == QDialog::Rejected)
        return false;

    const QString file = dialog.selectedFiles().at(0);

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

    ui->tabWidget_2->tabBar()->setTabText(editorList.length() -1, QFileInfo(file).fileName());
    ui->tabWidget_2->tabBar()->setTabToolTip(editorList.length() -1, file);

    currentEditor()->setFile(file);
    setWindowTitle(currentEditor()->filePath()); 

    return onFileSave();
#endif
}

void MainWindow::onHelpAbout()
{
    QMessageBox::about(this, tr("About MarkdownEdit"), tr("<h2>MarkdownEdit</h2>\n"
                                                          "<p>MarkdownEdit, as the name suggests, is a simple and easy program to create and edit Markdown files.</p>\n"
                                                          "<h2>About</h2>\n"
                                                          "<table class=\"table\" style=\"border-style: none;\">\n"
                                                          "<tbody>\n"
                                                          "<tr>\n"
                                                          "<td>Version:</td>\n"
                                                          "<td>%1</td>\n"
                                                          "</tr>\n"
                                                          "<tr>\n"
                                                          "<td>Qt Version:</td>\n"
                                                          "<td>%2</td>\n"
                                                          "</tr>\n"
                                                          "<tr>\n"
                                                          "<td>Homepage:</td>\n"
                                                          "<td><span style=\"font-size: medium; white-space: pre-wrap; background-color: transparent; font-style: italic; color: #a8abb0;\"><a href=\"https://software-made-easy.github.io/MarkdownEdit/\">https://software-made-easy.github.io/MarkdownEdit/</a></span></td>\n"
                                                          "</tr>\n"
                                                          "</tbody>\n"
                                                          "</table>\n"
                                                          "<h2>Credits</h2>\n"
                                                          "<p>Thanks to <a href=\"https://github.com/Waqar144\">Waqar Ahmed</a> for help with development.</p>\n"
                                                          "<p>The conversion from Markdown to HTML is done using the <a href=\"https://github.com/mity/md4c\">md4c</a> library by <em>Martin Mit&aacute;&scaron;</em>.</p>\n"
                                                          "<p>The <a href=\"https://github.com/pbek/qmarkdowntextedit\">widget</a> used for writing was created by <em>Patrizio Bekerle</em>.</p>"
                                                          ).arg(QStringLiteral(APP_VERSION), qVersion()));
}

void MainWindow::openRecent() {
    // A QAction represents the action of the user clicking
    const QString filename = qobject_cast<QAction *>(sender())->data().toString();

    if (!QFile::exists(filename)) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("This file could not be found:\n<em>%1</em>.").arg(
                                 QDir::toNativeSeparators(filename)));
        recentOpened.removeOne(filename);
        updateOpened();
        return;
    }

    openFile(filename);
    updateOpened();
}

void MainWindow::updateOpened() {
    for (QAction* &a : ui->menuRecentlyOpened->actions()) { // Fixed warning
        disconnect(a, &QAction::triggered, this, &MainWindow::openRecent);
        a->deleteLater();
        delete a;
    }

    ui->menuRecentlyOpened->clear(); // Clear the menu

    if (recentOpened.contains(path) && recentOpened.at(0) != path)
        recentOpened.move(recentOpened.indexOf(path), 0);
    else if (!path.isEmpty() && !recentOpened.contains(path))
        recentOpened.prepend(path);

    if (recentOpened.count() > RECENT_OPENED_LIST_LENGTH)
        recentOpened.takeLast();

    for (int i = 0; i < recentOpened.count(); i++) {
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
    for(MarkdownEditor* editor : qAsConst(editorList)) {
        if (editor->document()->isModified()) {
            const auto button = QMessageBox::question(this, tr("Save changes?"),
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

void MainWindow::loadSettings() {
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

    setPath = settings->value(QStringLiteral("setPath"), true).toBool();

    recentOpened = settings->value(QStringLiteral("recent"), QStringList()).toStringList();
    if (!recentOpened.isEmpty()) {
        if (recentOpened.first().isEmpty()) {
            recentOpened.removeFirst();
        }
    }

    const bool lineWrap = settings->value(QStringLiteral("lineWrap"), false).toBool();
    changeWordWrap(lineWrap);

    setLanguageMap(settings->value(QStringLiteral("languagesMap"),
                                   QHash<QString, QVariant>()).toHash());

    spelling = settings->value(QStringLiteral("spelling"), true).toBool();
    ui->actionSpell_checking->setChecked(spelling);
    // changeSpelling(spelling); not loaded yet

    ui->splitter->setSizes(QList<int>() << QWIDGETSIZE_MAX << QWIDGETSIZE_MAX);
}

void MainWindow::saveSettings() {
    settings->setValue(QStringLiteral("geometry"), saveGeometry());
    settings->setValue(QStringLiteral("state"), saveState());
    settings->setValue(QStringLiteral("highlighting"), highlighting);
    settings->setValue(QStringLiteral("recent"), recentOpened);
    settings->setValue(QStringLiteral("openLast"), ui->actionOpen_last_document_on_start->isChecked());
    settings->setValue(QStringLiteral("last"), path);
    settings->setValue(QStringLiteral("setPath"), setPath);
    settings->setValue(QStringLiteral("spelling"), spelling);
    settings->setValue(QStringLiteral("lineWrap"), ui->actionWord_wrap->isChecked());
    settings->setValue(QStringLiteral("languagesMap"), getLanguageMap());
}

MainWindow::~MainWindow()
{
    delete ui;
}
