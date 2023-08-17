/**
 ** This file is part of the MarkdownEdit project.
 ** Copyright 2022 - 2023 Tim Gromeyer <sakul8826@gmail.com>.
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

// project imports
#include "mainwindow.h"
#include "highlighter.h"
#include "markdowndialog.h"
#include "markdowneditor.h"
#include "markdownparser.h"
#include "settings.h"
#include "spellchecker.h"
#include "tableofcontents.h"
#include "tablewidget.h"
#include "ui_mainwindow.h"

// Qt imports
#include <QComboBox>
#include <QDebug>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QDirIterator>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QSaveFile>
#include <QScreen>
#include <QScrollBar>
#include <QSettings>
#include <QShortcut>
#include <QTemporaryFile>
#include <QTextStream>
#include <QThreadPool>
#include <QTimer>
#include <QToolButton>
#include <QWidgetAction>
#include <QtPrintSupport/QPrinter>

// other imports
#include <chrono>

#ifdef QT_PRINTSUPPORT_LIB
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#endif

using namespace std::chrono_literals;

QT_REQUIRE_CONFIG(mainwindow);
QT_REQUIRE_CONFIG(filedialog);

MainWindow::MainWindow(const QStringList &files, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // we keep it here so the startup looks better
    ui->setupUi(this);

    // Setup settings to load settings
    settings = new QSettings(STR("SME"), STR("MarkdownEdit"), this);

#ifndef Q_OS_WASM
    const QByteArray geo = settings->value(STR("geometry"), QByteArrayLiteral("")).toByteArray();
    if (geo.isEmpty()) {
        const QRect availableGeometry = QGuiApplication::screenAt(pos())->availableGeometry();
        resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
        move((availableGeometry.width() - width()) / 2, (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geo);
    }

    restoreState(settings->value(STR("state"), QByteArrayLiteral("")).toByteArray());
#endif

#ifndef MOBILE
    QMetaObject::invokeMethod(
        this,
        [this, files] {
#endif
            loadSettings();     // load the settings
            setupThings();      // Setup things
            loadIcons();        // load icons
            setupToolbar();     // setup toolbars
            setupConnections(); // connect everything before we load the files
            loadFiles(files);   // load the files or create an new document
#ifndef MOBILE
        },
        Qt::QueuedConnection);
#endif
}

void MainWindow::androidPreview(const bool c)
{
    if (c) {
        ui->tabWidget->show();
        ui->tabWidget_2->hide();

        dontUpdate = false;
        onTextChanged();
        dontUpdate = true;
    } else {
        ui->tabWidget->hide();
        ui->tabWidget_2->show();
        if (currentEditor())
            currentEditor()->setFocus(Qt::TabFocusReason);
    }
}

void MainWindow::onHelpSyntax()
{
    QString file = STR(":/syntax/en.md");
    QString language = STR("en_US");

    static const QStringList uiLanguages = QLocale::system().uiLanguages(); // ex. de-DE
    static const QStringList languages = SpellChecker::getLanguageList();   // ex. de_DE

    // loop thought the languages
    for (const QString &lang : uiLanguages) {
        //  get first to characters
        const QString lang2 = lang.right(2);

        if (QFile::exists(STR(":/syntax/%1.md").arg(lang2))) {
            file = STR(":/syntax/%1.md").arg(lang2);
            language = lang;
            break;
        }
    }

    // Correct language format
    language.replace(u'-', u'_');

    if (!languages.isEmpty() && !languages.contains(language)) {
        if (language.length() > 2)
            // Get the first 2 characters
            language = language.mid(0, 2);

        // Assuming language = de, try de_DE
        const QString newLang = STR("%1_%2").arg(language, language.toUpper());
        // If the language dict exists
        if (languages.contains(newLang)) {
            language = newLang;
        } else {
            language = *std::find_if(languages.begin(),
                                     languages.end(),
                                     [language](const QString &lang) {
                                         return lang.contains(language);
                                     });
        }
    } else
        language.clear();

    openFile(file, language);
}

// TODO: Fix it!
void MainWindow::toForeground()
{
    setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    raise();          // For MacOS
    activateWindow(); // For Windows

    auto eFlags = windowFlags();
    setWindowFlags(eFlags | Qt::WindowStaysOnTopHint);
    show();
    setWindowFlags(eFlags);
    show();
}

void MainWindow::onFileReload()
{
    QObject *sende = sender();
    QWidget *widget = qobject_cast<QWidget *>(sende->parent());

    const QString file = sende->property("file").toString();
    QStringList files = property("reloadFiles").toStringList();

    if (file.isEmpty())
        return;

    QFile f(file);

    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this,
                             tr("Couldn't open file"),
                             tr("Could not open file <em>%1</em>: %2")
                                 .arg(QDir::toNativeSeparators(file), f.errorString()));
        recentOpened.removeOne(file);
        updateOpened();
        return;
    }

    for (MarkdownEditor *editor : qAsConst(editorList)) {
        if (!editor)
            return;

        if (editor->getPath() == file) {
            editor->setText(f.readAll(), QLatin1String(), false);
            break;
        }
    }
    files.removeOne(file);
    setProperty("reloadFiles", files);

    if (widget->objectName() == STR("widgetReloadFile")) {
        widget->deleteLater();
        delete widget;
    }
}

void MainWindow::setupThings()
{
    // Setup a QToolButton to access the
    // recently opened menu from the toolbar
    toolbutton = new QToolButton(this);
    toolbutton->setMenu(ui->menuRecentlyOpened);
    toolbutton->setPopupMode(QToolButton::InstantPopup);

    // Use native menu bar
    menuBar()->setNativeMenuBar(true);

    // Setup a file watcher
    watcher = new QFileSystemWatcher(this);

    // Setup ComboBox to choose between Commonmark, GitHub and Doxygen
    mode = new QComboBox(ui->Edit);
    mode->addItems(QStringList() << STR("Commonmark") << STR("GitHub") << STR("Doxygen"));
    mode->setCurrentIndex(1);

    // Setup ComboBox to choose between Preview and Raw HTML
    widgetBox = new QComboBox(this);
    widgetBox->addItems(QStringList() << tr("Preview") << tr("HTML"));
    widgetBox->setCurrentIndex(0);

    // Setup the HTML Highlighter
    htmlHighliter = new Highliter(ui->raw->document());

    // Setup a action used to hide the editor and show the preview
    actionPreview = new QAction(QIcon::fromTheme(STR("view-preview"),
                                                 QIcon(STR(":/icons/view-preview.svg"))),
                                tr("Preview"),
                                this);
    actionPreview->setPriority(QAction::HighPriority);
    actionPreview->setCheckable(true);
    connect(actionPreview, &QAction::triggered, this, &MainWindow::androidPreview);

    // Disable actions which doesn't work yet
    ui->actionSave->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    ui->actionRedo->setEnabled(false);

    // Setup a timer to hide the status bar
    timer = new QTimer(this);
    timer->setInterval(5s);
    timer->setSingleShot(true);
    timer->setTimerType(Qt::VeryCoarseTimer);
    connect(timer, &QTimer::timeout, statusBar(), &QStatusBar::hide);

    // Add shortcuts
#ifndef Q_OS_ANDROID
    // Setup shortcuts to open and close tabs
    shortcutNew = new QShortcut(this);
    shortcutClose = new QShortcut(this);

    shortcutNew->setKey(QKeySequence::AddTab);
    connect(shortcutNew, &QShortcut::activated, this, &MainWindow::onFileNew);

    shortcutClose->setKey(QKeySequence::Close);
    connect(shortcutClose, &QShortcut::activated, this, &MainWindow::closeCurrEditor);

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
    connect(mode, qOverload<int>(&QComboBox::currentIndexChanged), this, &MainWindow::changeMode);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onHelpAbout);
    connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->actionCopy, &QAction::triggered, this, &MainWindow::copy);
    connect(ui->actionCut, &QAction::triggered, this, &MainWindow::cut);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actionExportHtml, &QAction::triggered, this, &MainWindow::exportHtml);
    connect(ui->actionExportPdf, &QAction::triggered, this, &MainWindow::exportPdf);
    connect(ui->actionHTML, &QAction::triggered, this, &MainWindow::onImportHTML);
    connect(ui->actionMarkdown_Syntax, &QAction::triggered, this, &MainWindow::onHelpSyntax);
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::onFileNew);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onFileOpen);
    connect(ui->actionOpen_in_web_browser, &QAction::triggered, this, &MainWindow::openInWebBrowser);
    connect(ui->actionPaste, &QAction::triggered, this, &MainWindow::paste);
    connect(ui->actionPause_preview, &QAction::triggered, this, &MainWindow::pausePreview);
    connect(ui->actionPrint, &QAction::triggered, this, &MainWindow::filePrint);
    connect(ui->actionPrintPreview, &QAction::triggered, this, &MainWindow::filePrintPreview);
    connect(ui->actionRedo, &QAction::triggered, this, &MainWindow::redo);
    connect(ui->actionReload, &QAction::triggered, this, &MainWindow::onFileReload);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onFileSave);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::onFileSaveAs);
    connect(ui->actionSelectAll, &QAction::triggered, this, &MainWindow::selectAll);
    connect(ui->actionSpell_checking, &QAction::triggered, this, &MainWindow::changeSpelling);
    connect(ui->actionUndo, &QAction::triggered, this, &MainWindow::undo);
    connect(ui->actionWord_wrap, &QAction::triggered, this, &MainWindow::changeWordWrap);
    connect(ui->tabWidget, &QStackedWidget::currentChanged, this, &MainWindow::setText);
    connect(ui->tabWidget_2, &QTabWidget::currentChanged, this, &MainWindow::onEditorChanged);
    connect(ui->tabWidget_2, &QTabWidget::tabCloseRequested, this, &MainWindow::closeEditor);
    connect(ui->tabWidget_2->tabBar(), &QTabBar::tabMoved, this, &MainWindow::editorMoved);
    connect(watcher, &QFileSystemWatcher::fileChanged, this, &MainWindow::onFileChanged);

    connect(ui->actionAuto_add_file_path_to_icon_path,
            &QAction::triggered,
            this,
            &MainWindow::changeAddtoIconPath);
    connect(ui->actionDisable_preview, &QAction::triggered, this, [this](const bool c) {
        disablePreview(c);
        preview = !c;
    });
    connect(ui->actionHighlighting_activated,
            &QAction::triggered,
            this,
            &MainWindow::changeHighlighting);
    connect(widgetBox,
            qOverload<int>(&QComboBox::currentIndexChanged),
            ui->tabWidget,
            &QStackedWidget::setCurrentIndex);
}

void MainWindow::setupToolbar()
{
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
    actionWidgetBox = ui->toolBarPreview->addWidget(widgetBox);
    ui->toolBarPreview->addAction(actionPreview);
    actionWidgetBox->setPriority(QAction::HighPriority);

#ifdef NO_SPELLCHECK
    ui->menuExtras->removeAction(ui->actionSpell_checking);
#endif
#ifdef Q_OS_WASM
    ui->menuExport->removeAction(ui->actionExportPdf);
    ui->menuFile->removeAction(ui->actionPrint);
    ui->menuFile->removeAction(ui->actionPrintPreview);

    // Doesn't work on wasm
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
    ui->File->setIconSize(QSize(36, 36));
    ui->Edit->setIconSize(QSize(36, 36));
    ui->toolBarPreview->setIconSize(QSize(36, 36));
    ui->File->setMovable(false);
    ui->Edit->setMovable(false);
    ui->toolBarPreview->setMovable(false);

    qApp->setStyleSheet(STR("QSplitter { border: none; } QToolBar { border: none; }"));
#endif
    aBold = new QAction(tr("Bold"));
    aItalic = new QAction(tr("Italic"));
    aUnderline = new QAction(tr("Underline"));
    aStrikethrough = new QAction(tr("Strikethrough"));

    aInsertTable = new QAction(tr("Insert table"));
    aInsertTableOfContents = new QAction(tr("Insert Table of Contents"));

    auto *aInsertLink = new QAction(tr("Insert link"));
    auto *aInsertImage = new QAction(tr("Insert image"));

    aBold->setIcon(
        QIcon::fromTheme(STR("format-text-bold"), QIcon(STR(":/icons/format-text-bold.svg"))));
    aItalic->setIcon(
        QIcon::fromTheme(STR("format-text-italic"), QIcon(STR(":/icons/format-text-italic.svg"))));
    aUnderline->setIcon(QIcon::fromTheme(STR("format-text-underline"),
                                         QIcon(STR(":/icons/format-text-underline.svg"))));
    aStrikethrough->setIcon(QIcon::fromTheme(STR("format-text-strikethrough"),
                                             QIcon(STR(":/icons/format-text-strikethrough.svg"))));

    aInsertTable->setIcon(
        QIcon::fromTheme(STR("insert-table"), QIcon(STR(":/icons/insert-table.svg"))));
    aInsertTableOfContents->setIcon(
        QIcon::fromTheme(STR("insert-table-of-contents"),
                         QIcon(STR(":/icons/insert-table-of-contents.svg"))));

    aInsertImage->setIcon(
        QIcon::fromTheme(STR("insert-image"), QIcon(STR(":/icons/insert-image.svg"))));

    aInsertLink->setIcon(
        QIcon::fromTheme(STR("insert-link"), QIcon(STR(":/icons/insert-link.svg"))));

    connect(aBold, &QAction::triggered, this, &MainWindow::bold);
    connect(aItalic, &QAction::triggered, this, &MainWindow::italic);
    connect(aUnderline, &QAction::triggered, this, &MainWindow::underline);
    connect(aStrikethrough, &QAction::triggered, this, &MainWindow::strikethrough);

    connect(aInsertTable, &QAction::triggered, this, &MainWindow::insertTable);
    connect(aInsertTableOfContents, &QAction::triggered, this, &MainWindow::insertTableOfContents);

    connect(aInsertLink, &QAction::triggered, this, &MainWindow::insertLink);
    connect(aInsertImage, &QAction::triggered, this, &MainWindow::insertImage);

    ui->toolBarTools->addAction(aBold);
    ui->toolBarTools->addAction(aItalic);
    ui->toolBarTools->addAction(aUnderline);
    ui->toolBarTools->addAction(aStrikethrough);

    ui->toolBarTools->addSeparator();
    ui->toolBarTools->addAction(aInsertLink);
    ui->toolBarTools->addAction(aInsertImage);

    ui->toolBarTools->addSeparator();
    ui->toolBarTools->addAction(aInsertTable);
    ui->toolBarTools->addAction(aInsertTableOfContents);

#ifdef QT_DEBUG
    auto *aScreenshot = new QAction(QIcon::fromTheme(STR("gnome-screenshot")),
                                    tr("Take screenshot"),
                                    this);
    connect(aScreenshot, &QAction::triggered, this, [this, aScreenshot] {
        aScreenshot->setVisible(false);
        QTimer::singleShot(1s, this, [this, aScreenshot] {
            qDebug() << "Screenshot saved successfully:"
                     << grab().save(STR("/home/tim/Bilder/MarkdownEdit.png"), "PNG", 0);
            aScreenshot->setVisible(true);
        });
    });
    ui->toolBarTools->addAction(aScreenshot);
#endif
}

void MainWindow::insertTable()
{
    if (!currentEditor())
        return;

    TableDialog dialog(this);

    if (dialog.exec() == QDialog::Rejected)
        return;
    insertLongText(dialog.markdownTable(), true);
}

void MainWindow::insertTableOfContents()
{
    if (!currentEditor())
        return;

    TableOfContents dialog(currentEditor()->toPlainText(), this);

    if (dialog.exec() == QDialog::Rejected)
        return;
    insertLongText(dialog.markdownTOC(), true);
}

void MainWindow::insertLongText(const QString &text, const bool newLine)
{
    if (text.isEmpty() || !currentEditor())
        return;

    QTextCursor c = currentEditor()->textCursor();
    c.beginEditBlock();
    c.movePosition(QTextCursor::EndOfLine);

    if (newLine && !c.block().text().isEmpty())
        c.insertText(QChar(u'\n'));

    c.insertText(text);

    if (newLine)
        c.insertText(QChar(u'\n'));

    c.endEditBlock();
}

void MainWindow::insertText(QString what, const bool h)
{
    if (!currentEditor())
        return;

    QTextCursor c = currentEditor()->textCursor();
    c.beginEditBlock();

    if (!c.hasSelection())
        c.select(QTextCursor::WordUnderCursor);

    int start = c.selectionStart();
    int end = c.selectionEnd();

    c.setPosition(start);
    c.insertText(what);
    c.setPosition(end + what.size());
    c.insertText(h ? what.insert(1, u'/') : what);

    c.endEditBlock();

    if (h) {
        --start;
        --end;
    }

    // Reselect the text
    c.setPosition(start + what.size());
    c.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, end - start);

    currentEditor()->setTextCursor(c);
}

void MainWindow::insertLink()
{
    MarkdownDialog dialog(MarkdownDialog::Link, this);
    insertLongText(dialog.getMarkdown(), false);
}

void MainWindow::insertImage()
{
    MarkdownDialog dialog(MarkdownDialog::Image, this);
    insertLongText(dialog.getMarkdown(), false);
}

void MainWindow::bold()
{
    insertText(STR("**"));
}

void MainWindow::italic()
{
    insertText(QChar(u'*'));
}

void MainWindow::underline()
{
    insertText(STR("<u>"), true);
}

void MainWindow::strikethrough()
{
    insertText(QChar(u'~'));
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
    overrideVal = (short) index;

    auto *editor = editorList.at(index);

    path = editor->getPath();

    onOrientationChanged(orientation);

    if (path == tr("Untitled.md")) {
        path.clear();
        if (editor->document()->isModified()) {
            QMessageBox d(this);
            d.setText(tr("The document has been edited, do you want to save it?"));
            d.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Abort);
            d.setDefaultButton(QMessageBox::Yes);
            d.setIcon(QMessageBox::Question);

            const auto retVal = d.exec();

            if (retVal == QMessageBox::Abort) {
                overrideEditor = false;
                return;
            }
            if (retVal == QMessageBox::Yes) {
                if (!onFileSaveAs()) {
                    overrideEditor = false;
                    return;
                }
            }
        }
    } else if (editor->document()->isModified()) {
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
    editor->deleteLater();
    delete editor;

    if (editorList.isEmpty()) {                 // If all editors are closed
        html.clear();                           // Clear HTML
        setText(ui->tabWidget->currentIndex()); // apply the empty HTML to the preview widget
        ui->actionReload->setText(tr("Reload \"%1\"").remove(L1("%1")));
        ui->actionReload->setEnabled(false);
        setWindowModified(false);
        setWindowTitle(STR("MarkdownEdit"));
    }
}

void MainWindow::onEditorChanged(const int index)
{
    MarkdownEditor *editor = editorList.value(index, nullptr);
    if (!editor)
        return;

    const bool modified = editor->document()->isModified();

    setWindowTitle(editor->filePath());
    setWindowModified(modified);
    ui->actionSave->setEnabled(modified);

    ui->actionRedo->setEnabled(editor->document()->isRedoAvailable());
    ui->actionUndo->setEnabled(editor->document()->isUndoAvailable());

    path = editor->getPath();
    bigFile = editor->bigFile;
    onOrientationChanged(orientation);

    if (path == tr("Untitled.md")) {
        path.clear();
        ui->actionReload->setText(tr("Reload \"%1\"").arg('\0'));
        ui->actionReload->setEnabled(false);
    } else {
        ui->actionReload->setText(tr("Reload \"%1\"").arg(editor->getFileName()));
        ui->actionReload->setEnabled(true);
        ui->actionReload->setProperty("file", path);
    }

#ifdef Q_OS_WASM
    ui->actionReload->setEnabled(false);
#endif

    ui->textBrowser->setProperty("dir", editor->getDir());

    editor->setFocus(Qt::TabFocusReason);

    onTextChanged();
}

auto MainWindow::createEditor() -> MarkdownEditor *
{
    auto *editor = new MarkdownEditor(this);
    editor->changeSpelling(spelling);
    editor->getChecker()->setMarkdownHighlightingEnabled(highlighting);

    if (ui->actionWord_wrap->isChecked())
        editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    else
        editor->setLineWrapMode(QPlainTextEdit::NoWrap);

    editorList.append(editor);

    connect(editor, &QPlainTextEdit::textChanged, this, &MainWindow::onTextChanged);
    connect(editor->document(),
            &QTextDocument::modificationChanged,
            ui->actionSave,
            &QAction::setEnabled);
    connect(editor->document(),
            &QTextDocument::modificationChanged,
            this,
            &MainWindow::onModificationChanged);
    connect(editor->document(), &QTextDocument::undoAvailable, ui->actionUndo, &QAction::setEnabled);
    connect(editor->document(), &QTextDocument::redoAvailable, ui->actionRedo, &QAction::setEnabled);
    connect(editor->horizontalScrollBar(),
            &QScrollBar::valueChanged,
            ui->textBrowser->horizontalScrollBar(),
            &QScrollBar::setValue);
    connect(editor, &MarkdownEditor::openFile, this, &MainWindow::onOpenFile);
    connect(editor, &QMarkdownTextEdit::urlClicked, this, &MainWindow::onUrlClicked);

    return editor;
}

void MainWindow::receivedMessage(const quint32 id, const QByteArray &msg)
{
    QString f = QString::fromLatin1(msg); // is a bit faster
    f.remove(0, 7);                       // Removing "file://", which is added programmatically

    if (f.isEmpty()) // it's the case when you start a new app
        onFileNew();
    else
        openFiles(f.split(u' ')); // if you selected files
}

void MainWindow::onUrlClicked(const QString &url)
{
    const QUrl urlFromString = QUrl(url);
    const bool isRelativeFileUrl = url.startsWith(L1("file://"));

    if (!urlFromString.isValid() || isRelativeFileUrl)
        return;

    ui->textBrowser->openUrl(urlFromString);
}

void MainWindow::onModificationChanged(const bool m)
{
    setWindowModified(m);

    const int curr = ui->tabWidget_2->currentIndex();

    QString old = ui->tabWidget_2->tabBar()->tabText(curr);

    if (m && !old.endsWith(u'*'))
        old.append(u'*');
    else if (!m && old.endsWith(u'*'))
        old.remove(old.length() - 1, 1);

    ui->tabWidget_2->tabBar()->setTabText(curr, old);
}

auto MainWindow::currentEditor() -> MarkdownEditor *
{
    if (overrideEditor)
        return editorList.at(overrideVal);
    else {
        return editorList.value(ui->tabWidget_2->currentIndex(), nullptr);
    }
}

void MainWindow::onFileChanged(const QString &f)
{
    QStringList files = property("reloadFiles").toStringList();
    if (files.contains(f))
        return;

    auto *widgetReloadFile = new QWidget(this);
    widgetReloadFile->setStyleSheet(STR("background: orange"));
    widgetReloadFile->setObjectName(STR("widgetReloadFile"));

    auto *horizontalLayout = new QHBoxLayout(widgetReloadFile);

    auto *labelReloadFile = new QLabel(widgetReloadFile);
    labelReloadFile->setStyleSheet(STR("color: black"));

    horizontalLayout->addWidget(labelReloadFile);

    auto *buttonBox = new QDialogButtonBox(widgetReloadFile);
    buttonBox->setStandardButtons(QDialogButtonBox::No | QDialogButtonBox::Yes);
    buttonBox->setObjectName(STR("buttonBox"));
    buttonBox->setProperty("file", f);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &MainWindow::onFileReload);
    connect(buttonBox, &QDialogButtonBox::rejected, widgetReloadFile, &QWidget::deleteLater);

    horizontalLayout->addWidget(buttonBox);

    labelReloadFile->setText(tr("File <em>%1</em> has been modified.\n"
                                "Would you like to reload them?")
                                 .arg(f));

    ui->verticalLayout_2->insertWidget(0, widgetReloadFile);
    widgetReloadFile->show();

    watcher->addPath(f);
    files.append(f);
    setProperty("reloadFiles", files);
}

void MainWindow::loadIcons()
{
    loadIcon(STR("application-exit"), ui->actionExit);
    loadIcon(STR("application-pdf"), ui->actionExportPdf);
    loadIcon(STR("document-new"), ui->actionNew);
    loadIcon(STR("document-open"), ui->actionOpen);
    loadIcon(STR("document-open-recent"), ui->actionOpen_last_document_on_start);
    loadIcon(STR("document-print"), ui->actionPrint);
    loadIcon(STR("document-print-preview"), ui->actionPrintPreview);
    loadIcon(STR("document-revert"), ui->actionReload);
    loadIcon(STR("document-save"), ui->actionSave);
    loadIcon(STR("document-save-as"), ui->actionSaveAs);
    loadIcon(STR("edit-copy"), ui->actionCopy);
    loadIcon(STR("edit-copy"), ui->actionCopy);
    loadIcon(STR("edit-cut"), ui->actionCut);
    loadIcon(STR("edit-paste"), ui->actionPaste);
    loadIcon(STR("edit-redo"), ui->actionRedo);
    loadIcon(STR("edit-select-all"), ui->actionSelectAll);
    loadIcon(STR("edit-undo"), ui->actionUndo);
    loadIcon(STR("help-about"), ui->actionAbout);
    loadIcon(STR("help-contents"), ui->actionMarkdown_Syntax);
    loadIcon(STR("media-playback-pause"), ui->actionPause_preview);
    loadIcon(STR("text-html"), ui->actionExportHtml);
    loadIcon(STR("text-wrap"), ui->actionWord_wrap);
    loadIcon(STR("text-wrap"), ui->actionWord_wrap);
    loadIcon(STR("tools-check-spelling"), ui->actionSpell_checking);

    loadIcon(STR("document-export"), ui->menuExport);
    loadIcon(STR("document-import"), ui->menuImport_from);

    ui->menuRecentlyOpened->setIcon(ui->actionOpen_last_document_on_start->icon());

#ifndef Q_OS_WASM
    toolbutton->setIcon(ui->menuRecentlyOpened->icon());
#endif
    ui->actionHTML->setIcon(ui->actionExportHtml->icon());

    qApp->setWindowIcon(QIcon(STR(":/logo/Icon.svg")));
}

void MainWindow::loadIcon(const QString &name, QAction *a)
{
#ifndef FLATPAK
    a->setIcon(QIcon::fromTheme(name, QIcon(STR(":/icons/%1.svg").arg(name))));
#else
    a->setIcon(QIcon::fromTheme(name));
#endif
}

void MainWindow::loadIcon(const QString &name, QMenu *m)
{
#ifndef FLATPAK
    m->setIcon(QIcon::fromTheme(name, QIcon(STR(":/icons/%1.svg").arg(name))));
#else
    m->setIcon(QIcon::fromTheme(name));
#endif
}

void MainWindow::onOrientationChanged(const Qt::ScreenOrientation t)
{
    if (!actionPreview || !actionWidgetBox)
        return;

    if (t == Qt::PortraitOrientation || bigFile) {
        actionWidgetBox->setVisible(false);
        actionPreview->setVisible(true);
    } else {
        actionPreview->setVisible(false);
        actionWidgetBox->setVisible(true);
    }
    androidPreview(false);
    actionPreview->setChecked(false);

    if (preview)
        disablePreview(t == Qt::PortraitOrientation || bigFile);
}

void MainWindow::setText(const int index)
{
    if (index == 0) {
        ui->textBrowser->setHtml(html);
    } else {
        const int v = ui->raw->verticalScrollBar()->value();
        ui->raw->document()->setPlainText(html);
        ui->raw->verticalScrollBar()->setValue(v);
    }
}

void MainWindow::changeWordWrap(const bool c)
{
    for (MarkdownEditor *editor : qAsConst(editorList)) {
        if (c)
            editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        else
            editor->setLineWrapMode(QPlainTextEdit::NoWrap);
    }

    if (c) {
        ui->textBrowser->setLineWrapMode(QTextBrowser::WidgetWidth);
        ui->raw->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    } else {
        ui->textBrowser->setLineWrapMode(QTextBrowser::NoWrap);
        ui->raw->setLineWrapMode(QPlainTextEdit::NoWrap);
    }

    ui->actionWord_wrap->setChecked(c);
}

void MainWindow::changeSpelling(const bool checked)
{
#ifndef NO_SPELLCHECK
    for (MarkdownEditor *editor : qAsConst(editorList)) {
        editor->changeSpelling(checked);
    }

    ui->actionSpell_checking->setChecked(checked);
    spelling = checked;
#endif
}

void MainWindow::disablePreview(const bool checked)
{
    if (!bigFile)
        ui->tabWidget->setVisible(!checked);

    dontUpdate = checked;

    ui->actionPause_preview->setChecked(checked);
    ui->actionPause_preview->setDisabled(checked);

    ui->actionDisable_preview->setChecked(checked);

    if (!checked) {
        onTextChanged();
#ifndef FLATPAK
        ui->actionDisable_preview->setIcon(
            QIcon::fromTheme(STR("media-playback-stop"),
                             QIcon(STR(":/icons/media-playback-stop.svg"))));
#else
        ui->actionDisable_preview->setIcon(QIcon(STR(":/icons/media-playback-stop.svg")));
#endif
    } else
#ifndef FLATPAK
        ui->actionDisable_preview->setIcon(
            QIcon::fromTheme(STR("media-playback-start"),
                             QIcon(STR(":/icons/media-playback-start.svg"))));
#else
        ui->actionDisable_preview->setIcon(QIcon(STR(":/icons/media-playback-start.svg")));
#endif
}

void MainWindow::pausePreview(const bool checked)
{
    dontUpdate = checked;
    ui->tabWidget->setDisabled(checked);

    if (!checked) {
        onTextChanged();
        ui->actionPause_preview->setIcon(
            QIcon::fromTheme(STR("media-playback-pause"),
                             QIcon(STR(":/icons/media-playback-pause.svg"))));
    } else
        ui->actionPause_preview->setIcon(
            QIcon::fromTheme(STR("media-playback-start"),
                             QIcon(STR(":/icons/media-playback-start.svg"))));
}

void MainWindow::cut()
{
    if (ui->textBrowser->hasFocus())
        ui->textBrowser->cut();
    else if (ui->raw->hasFocus())
        ui->raw->cut();
    else if (currentEditor())
        currentEditor()->cut();
}

void MainWindow::copy()
{
    if (ui->textBrowser->hasFocus())
        ui->textBrowser->copy();
    else if (ui->raw->hasFocus())
        ui->raw->copy();
    else if (currentEditor())
        currentEditor()->copy();
}

void MainWindow::paste()
{
    if (ui->textBrowser->hasFocus())
        ui->textBrowser->paste();
    else if (ui->raw->hasFocus())
        ui->raw->paste();
    else if (currentEditor())
        currentEditor()->paste();
}

void MainWindow::selectAll()
{
    if (ui->textBrowser->hasFocus())
        ui->textBrowser->selectAll();
    else if (ui->raw->hasFocus())
        ui->raw->selectAll();
    else if (currentEditor())
        currentEditor()->selectAll();
}

void MainWindow::undo()
{
    if (ui->raw->hasFocus())
        ui->raw->undo();
    else if (currentEditor())
        currentEditor()->undo();
}

void MainWindow::redo()
{
    if (ui->raw->hasFocus())
        ui->raw->redo();
    else if (currentEditor())
        currentEditor()->redo();
}

void MainWindow::changeAddtoIconPath(const bool c)
{
    setPath = c;

    ui->actionAuto_add_file_path_to_icon_path->setChecked(c);

    if (!c) {
        ui->textBrowser->setLoadImages(false);
        setText(0);
        return;
    }

    QStringList searchPaths;
    searchPaths.reserve(editorList.size());

    for (const QString &file : qAsConst(fileList)) {
        searchPaths << QFileInfo(file).absolutePath();
    }

    ui->textBrowser->setLoadImages(true);
    ui->textBrowser->setSearchPaths(searchPaths);
    setText(0);
}

void MainWindow::changeHighlighting(const bool enabled)
{
    dontUpdate = true;

    // When disabled show warning
    if (!enabled && isVisible()) {
        auto ret = QMessageBox::warning(
            this,
            tr("Warning"),
            tr("If you disable highlighting, the spell checker will not work properly."),
            QMessageBox::Ok | QMessageBox::Abort,
            QMessageBox::Ok);

        if (ret == QMessageBox::Abort) {
            ui->actionHighlighting_activated->setChecked(true);
            return;
        }
    }

    for (MarkdownEditor *editor : qAsConst(editorList)) {
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
    connect(&preview, &QPrintPreviewDialog::paintRequested, this, &MainWindow::printPreview);

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

    if (ui->tabWidget->currentIndex() == 1)
        ui->textBrowser->setHtml(html);
    ui->textBrowser->print(printer);

    QGuiApplication::restoreOverrideCursor();
#endif
}

void MainWindow::exportPdf()
{
    QFileDialog dialog(this, tr("Export PDF"));
    dialog.setMimeTypeFilters({STR("application/pdf")});
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix(STR("pdf"));
    if (dialog.exec() == QDialog::Rejected)
        return;

    const QString file = dialog.selectedFiles().at(0);

    if (file.isEmpty())
        return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(file);

    if (ui->tabWidget->currentIndex() == 1)
        ui->textBrowser->setHtml(html);
    ui->textBrowser->print(&printer);

#ifndef Q_OS_ANDROID
    statusBar()->show();
    statusBar()->showMessage(tr("Pdf exported to %1").arg(QDir::toNativeSeparators(file)), 0);
    timer->start();
#endif
}

void MainWindow::openInWebBrowser()
{
    QTemporaryFile f(this);

    const QString name = f.fileTemplate() + L1(".html");

    f.setFileTemplate(name);
    f.setAutoRemove(false);

    if (!f.open()) {
        qWarning() << "Could not create temporary file: " << f.errorString();

        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("Could not create temporary file: %1").arg(f.errorString()));

        return;
    }

    QTextStream out(&f);
    out << html;

    QDesktopServices::openUrl(QUrl::fromLocalFile(f.fileName()));
    const QString file = f.fileName();

    // Delete the file file after 5s
    QTimer::singleShot(5s, this, [file] { QFile::remove(file); });
}

void MainWindow::exportHtml()
{
#if defined(Q_OS_WASM)
    QFileDialog::saveFileContent(html.toUtf8(), STR("Exported HTML.html"));
#else
    QFileDialog dialog(this, tr("Export HTML"));
    dialog.setMimeTypeFilters({STR("text/html")});
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix(STR("html"));
    if (dialog.exec() != QDialog::Accepted)
        return;

    const QString file = dialog.selectedFiles().at(0);

    if (file.isEmpty())
        return;

    QFile f(file, this);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("Could not write to file <em>%1</em>: %2")
                                 .arg(QDir::toNativeSeparators(path), f.errorString()));
        return;
    }

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    QTextStream str(&f);
    str << html;

#ifndef Q_OS_ANDROID
    statusBar()->show();
    statusBar()->showMessage(tr("HTML exported to %1").arg(QDir::toNativeSeparators(file)), 0);
    timer->start();
#endif

    QGuiApplication::restoreOverrideCursor();
#endif
}

void MainWindow::changeMode(const int i)
{
    _mode = (short) i;
    onTextChanged();
    currentEditor()->setFocus();
}

void MainWindow::onTextChanged()
{
    // Don't continue if the preview is paused or the current editor is null.
    if (dontUpdate || !currentEditor())
        return;

    // Generate HTML from Markdown
    html = Parser::toHtml(currentEditor()->document()->toPlainText(), _mode);

    // Apply the HTML to the visible widget
    setText(ui->tabWidget->currentIndex());
}

void MainWindow::loadFiles(const QStringList &files)
{
    // Ensure settings are set up
    Q_ASSERT(settings);
    const bool openLast = settings->value(STR("openLast"), true).toBool();
    ui->actionOpen_last_document_on_start->setChecked(openLast);

    if (files.isEmpty()) {
        const QString last = settings->value(STR("last"), QLatin1String()).toString();
        if (openLast && !last.isEmpty())
            openFile(last);
        else {
            onFileNew();
            updateOpened();
        }
    } else
        openFiles(files);
}

void MainWindow::openFiles(const QStringList &files)
{
    for (const QString &file : files) {
        openFile(QFileInfo(file).absoluteFilePath()); // ensure file path is absolute
    }
}

void MainWindow::openFile(const QString &newFile, const QString &lang)
{
    // Check of file is already open
    if (fileList.contains(newFile)) {
        for (auto i = 0; editorList.length() > i; ++i) {
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
        QMessageBox::warning(this,
                             tr("Couldn't open file"),
                             tr("Could not open file <em>%1</em>: %2")
                                 .arg(QDir::toNativeSeparators(newFile), f.errorString()));
        recentOpened.removeOne(newFile);
        updateOpened();
        return;
    }
    constexpr int limit = 400000; // 400KB
    if (f.size() > limit) {
        const auto out = QMessageBox::warning(
            this,
            tr("Large file"),
            tr("This is a large file that can potentially cause performance issues."),
            QMessageBox::Ok | QMessageBox::Cancel,
            QMessageBox::Ok);

        if (out == QMessageBox::Cancel) {
            if (fileList.isEmpty())
                updateOpened();

            return;
        }

        bigFile = true;
    } else
        bigFile = false;

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    blockSignals(true);

    path = newFile;
    watcher->addPath(path);

    if (setPath)
        ui->textBrowser->appenSearchPath(QFileInfo(newFile).path());

    auto *editor = createEditor();
    editor->setFile(newFile);
    // Store it there so we can use it later
    editor->bigFile = bigFile;
    ui->tabWidget_2->insertTab(editorList.length() - 1, editor, editor->getFileName());
    ui->tabWidget_2->setCurrentIndex(editorList.length() - 1);
    ui->tabWidget_2->tabBar()->setTabToolTip(editorList.length() - 1,
                                             QDir::toNativeSeparators(newFile));

    editor->setText(f.readAll(), newFile);

    setWindowTitle(editor->filePath());
    ui->actionReload->setText(tr("Reload \"%1\"").arg(editor->getFileName()));

    fileList.append(newFile);

    updateOpened();

#ifndef Q_OS_ANDROID
    statusBar()->show();
    statusBar()->showMessage(tr("Opened %1").arg(QDir::toNativeSeparators(path)), 0);
    timer->start();
#endif

    blockSignals(false);

    QGuiApplication::restoreOverrideCursor();
}

void MainWindow::onFileNew()
{
    path.clear();
    static const QString file = tr("Untitled.md");

    auto *editor = createEditor();
    editor->setFile(file);

    ui->tabWidget_2->insertTab(editorList.length() - 1, editor, editor->getFileName());
    ui->tabWidget_2->setCurrentIndex(editorList.length() - 1);

    if (!editor->setLanguage(QLocale::system().name()))
        editor->setLanguage(STR("en_US"));

#ifndef Q_OS_ANDROID
    statusBar()->show();
    statusBar()->showMessage(tr("New document created"), 0);
    timer->start();
#endif
}

void MainWindow::onFileOpen()
{
#if defined(Q_OS_WASM)
    static auto fileContentReady = [this](const QString &newFile, const QByteArray &fileContent) {
        if (!newFile.isEmpty()) {
            QGuiApplication::setOverrideCursor(Qt::WaitCursor);

            path = newFile;

            bigFile = fileContent.size() >= 500000;

            if (setPath)
                ui->textBrowser->appenSearchPath(QFileInfo(newFile).path());

            auto *editor = createEditor();
            editor->setFile(newFile);
            ui->tabWidget_2->insertTab(editorList.length() - 1, editor, editor->getFileName());
            ui->tabWidget_2->setCurrentIndex(editorList.length() - 1);
            editor->setText(fileContent, newFile);
            editor->bigFile = bigFile;

            setWindowTitle(editor->filePath());
            ui->actionReload->setText(tr("Reload \"%1\"").arg(editor->getFileName()));

            fileList.append(newFile);

            statusBar()->show();
            statusBar()->showMessage(tr("Opened %1").arg(QDir::toNativeSeparators(path)), 0);
            timer->start();

            updateOpened();

            QGuiApplication::restoreOverrideCursor();
        }
    };
    QFileDialog::getOpenFileContent(tr("Markdown (*.md *.markdown *.mkd)"), fileContentReady);
#else
    QFileDialog dialog(this, tr("Open Markdown File"));
    dialog.setMimeTypeFilters({STR("text/markdown")});
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    if (dialog.exec() == QDialog::Rejected)
        return;

    const QString file = dialog.selectedFiles().at(0);
    if (file == path || file.isEmpty())
        return;

    openFile(file);
#endif
}

auto MainWindow::onFileSave() -> bool
{
    if (!currentEditor()->document()->isModified())
        if (QFile::exists(path))
            return true;

    if (path.isEmpty() || path.startsWith(L1(":/syntax_")))
        return onFileSaveAs();

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

#if defined(Q_OS_WASM)
    QFileDialog::saveFileContent(currentEditor()->toPlainText().toUtf8());
#else

    watcher->removePath(path);

    QSaveFile f(path, this);
    f.setDirectWriteFallback(true);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("Could not write to file <em>%1</em>: %2")
                                 .arg(QDir::toNativeSeparators(path), f.errorString()));
        return false;
    }

    QTextStream str(&f);
    str << currentEditor()->toPlainText();

    if (!f.commit()) {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("Could not write to file <em>%1</em>: %2")
                                 .arg(QDir::toNativeSeparators(path), f.errorString()));
        return false;
    }

    watcher->addPath(path);
#endif

#ifndef Q_OS_ANDROID
    statusBar()->show();
    statusBar()->showMessage(tr("Wrote %1").arg(QDir::toNativeSeparators(path)), 0);
    timer->start();
#endif

    updateOpened();

    currentEditor()->document()->setModified(false);

    QGuiApplication::restoreOverrideCursor();

    return true;
}

auto MainWindow::onFileSaveAs() -> bool
{
#ifdef Q_OS_WASM
    path = L1("Markdown.md");
    return onFileSave();
#else

    QFileDialog dialog(this, tr("Save Markdown File"));
    dialog.setMimeTypeFilters({STR("text/markdown")});
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix(STR("md"));
    if (dialog.exec() == QDialog::Rejected)
        return false;

    const QString file = dialog.selectedFiles().at(0);

    if (file.isEmpty())
        return false;
    else
        path = file;

    if (!(path.endsWith(L1(".md")) || path.endsWith(L1(".markdown")) || path.endsWith(L1(".mkd"))))
        path.append(L1(".md"));

    watcher->addPath(file);

    if (fileList.contains(file))
        openFile(file);

    setMapAttribute(path, currentEditor()->getChecker()->getLanguage());

    ui->tabWidget_2->tabBar()->setTabText(editorList.length() - 1, QFileInfo(file).fileName());
    ui->tabWidget_2->tabBar()->setTabToolTip(editorList.length() - 1, file);

    currentEditor()->setFile(file);
    setWindowTitle(currentEditor()->filePath());

    return onFileSave();
#endif
}

void MainWindow::onHelpAbout()
{
    QMessageBox::about(
        this,
        tr("About MarkdownEdit"),
        tr("<h2>MarkdownEdit</h2>\n"
           "<p>MarkdownEdit, as the name suggests, is a simple and easy program to create and edit "
           "Markdown files.</p>\n"
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
           "<td><span style=\"font-size: medium; white-space: pre-wrap; background-color: "
           "transparent; font-style: italic; color: #a8abb0;\"><a href=\"%3\">%3</a></span></td>\n"
           "</tr>\n"
           "</tbody>\n"
           "</table>\n"
           "<h2>Credits</h2>\n"
           "<p>Thanks to <a href=\"https://github.com/Waqar144\">Waqar Ahmed</a> for help with "
           "development.</p>\n"
           "<p>The conversion from Markdown to HTML is done using the <a "
           "href=\"https://github.com/mity/md4c\">md4c</a> library by <em>Martin "
           "Mit&aacute;&scaron;</em>.</p>\n"
           "<p>The <a href=\"https://github.com/pbek/qmarkdowntextedit\">widget</a> used for "
           "writing was created by <em>Patrizio Bekerle</em>.</p>")
            .arg(QStringLiteral(APP_VERSION), QLatin1String(qVersion()), QStringLiteral(HOMEPAGE)));
}

void MainWindow::openRecent()
{
    // A QAction represents the action of the user clicking
    const QString filename = qobject_cast<QAction *>(sender())->data().toString();

    if (!QFile::exists(filename)) {
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("This file could not be found:\n<em>%1</em>.")
                                 .arg(QDir::toNativeSeparators(filename)));
        recentOpened.removeOne(filename);
        updateOpened();
        return;
    }

    openFile(filename);
    updateOpened();
}

void MainWindow::updateOpened()
{
    for (QAction *&a : ui->menuRecentlyOpened->actions()) { // Fixed warning
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

    for (int i = 0; i < recentOpened.count(); ++i) {
        const QString document = recentOpened.at(i);
        auto *action = new QAction(STR("&%1 | %2").arg(QString::number(i + 1), document), this);
        connect(action, &QAction::triggered, this, &MainWindow::openRecent);

        action->setData(document);
        ui->menuRecentlyOpened->addAction(action);
    }
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    for (MarkdownEditor *editor : qAsConst(editorList)) {
        if (editor->document()->isModified()) {
            const auto button = QMessageBox::question(this,
                                                      tr("Save changes?"),
                                                      tr("The file <em>%1</em> has been changed.\n"
                                                         "Do you want to leave anyway?")
                                                          .arg(editor->getFileName()));
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

void MainWindow::resizeEvent(QResizeEvent *e)
{
    const QSize size = e->size();
    const QSize old = e->oldSize();

    if (size.height() > size.width() && (!old.isValid() || old.height() <= old.width())) {
        onOrientationChanged(Qt::PortraitOrientation);
        orientation = Qt::PortraitOrientation;
    } else if (size.height() < size.width() && (!old.isValid() || old.height() >= old.width())) {
        onOrientationChanged(Qt::LandscapeOrientation);
        orientation = Qt::LandscapeOrientation;
    }

    e->accept();
    QMainWindow::resizeEvent(e);
}

void MainWindow::loadSettings()
{
    highlighting = settings->value(STR("highlighting"), true).toBool();
    ui->actionHighlighting_activated->setChecked(highlighting);

    setPath = settings->value(STR("setPath"), true).toBool();
    ui->actionAuto_add_file_path_to_icon_path->setChecked(setPath);

    recentOpened = settings->value(STR("recent"), QStringList()).toStringList();
    if (!recentOpened.isEmpty()) {
        if (recentOpened.at(0).isEmpty()) {
            recentOpened.removeFirst();
        }
    }

#ifndef Q_OS_ANDROID
    const bool lineWrap = settings->value(STR("lineWrap"), false).toBool();
#else
    constexpr bool lineWrap = true;
#endif
    changeWordWrap(lineWrap);

    preview = settings->value(STR("preview"), true).toBool();

    setLanguageMap(settings->value(STR("languagesMap"), QHash<QString, QVariant>()).toHash());

    spelling = settings->value(STR("spelling"), true).toBool();
    ui->actionSpell_checking->setChecked(spelling);

    ui->splitter->setSizes(QList<int>() << QWIDGETSIZE_MAX << QWIDGETSIZE_MAX);
}

void MainWindow::saveSettings()
{
    settings->setValue(STR("geometry"), saveGeometry());
    settings->setValue(STR("state"), saveState());
    settings->setValue(STR("highlighting"), highlighting);
    settings->setValue(STR("recent"), recentOpened);
    settings->setValue(STR("openLast"), ui->actionOpen_last_document_on_start->isChecked());
    settings->setValue(STR("last"), path);
    settings->setValue(STR("setPath"), setPath);
    settings->setValue(STR("spelling"), spelling);
    settings->setValue(STR("lineWrap"), ui->actionWord_wrap->isChecked());
    settings->setValue(STR("languagesMap"), getLanguageMap());
    settings->setValue(STR("preview"), preview);
}

void MainWindow::onImportHTML()
{
    QInputDialog dia(this);
    dia.setInputMode(QInputDialog::TextInput);
    dia.setLabelText(tr("Enter HTML"));
    dia.setWindowTitle(tr("Import from HTML"));

    const auto out = dia.exec();
    if (out == QDialog::Rejected)
        return;

    const QString html = dia.textValue();

    onFileNew();
    currentEditor()->setText(Parser::toMarkdown(html).toLocal8Bit());
}

MainWindow::~MainWindow()
{
    int activeThreadCount = QThreadPool::globalInstance()->activeThreadCount();
    if (activeThreadCount > 0) {
        qInfo() << "Waiting for" << activeThreadCount << "threads to finish...";
        QThreadPool::globalInstance()->waitForDone();
        qInfo() << "All threads ended!";
    }

    delete ui;
    delete htmlHighliter;
    delete toolbutton;
    delete widgetBox;
    delete actionWidgetBox;
    delete actionPreview;
    delete mode;
    delete timer;
    delete watcher;
    delete settings;
    delete shortcutNew;
    delete shortcutClose;
}
