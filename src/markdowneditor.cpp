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

#include "markdowneditor.h"
#include "settings.h"
#include "spellchecker.h"
#include "utils.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMimeData>
#include <QMimeDatabase>
#include <QRunnable>
#include <QSaveFile>
#include <QSignalBlocker>
#include <QTextStream>

MarkdownEditor::MarkdownEditor(QWidget *parent)
    : QMarkdownTextEdit(parent, false)
{
    checker = new SpellChecker(this);
    connect(checker, &SpellChecker::languageChanged, this, &MarkdownEditor::onLanguageChanged);

    autoSaveTimer = new QTimer(this);
    autoSaveTimer->setInterval(5000);
    autoSaveTimer->setTimerType(Qt::VeryCoarseTimer);
    connect(autoSaveTimer, &QTimer::timeout, this, &MarkdownEditor::autoSave);
    autoSaveTimer->start();

    connect(document(),
            &QTextDocument::modificationChanged,
            this,
            &MarkdownEditor::onModificationChanged);

    setAcceptDrops(true);

    searchWidget()->setDarkMode(settings::isDarkMode());

    // FIXME: Zoom not working
    // connect(this, &QMarkdownTextEdit::zoomIn, this, [this] { QPlainTextEdit::zoomIn(); });
    // connect(this, &QMarkdownTextEdit::zoomOut, this, [this] { QPlainTextEdit::zoomOut(); });
}

void MarkdownEditor::onModificationChanged(bool modified)
{
    if (modified) {
        autoSaveTimer->start();
    } else {
        autoSaveTimer->stop();
        // Delete auto-save file
        deleteAutoSaveFile();
    }
}

void MarkdownEditor::autoSaveFile(const QString &content)
{
    QString fileName = File::getAutoSaveFileName(info);

    QSaveFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open auto-save file" << fileName << file.errorString();
        return;
    }
    file.setDirectWriteFallback(true);

    QTextStream stream(&file);
    stream << content;

    if (!file.commit()) {
        qWarning() << "Failed to write auto-save file" << fileName << file.errorString();
    }
}

void MarkdownEditor::deleteAutoSaveFile()
{
    QFile::remove(File::getAutoSaveFileName(info));
}

void MarkdownEditor::autoSave()
{
    // We run this in a extra thread, we don't want a lag while typing
    if (document()->isModified() && info.exists() && m_autoSaveEnabled)
        threading::runFunction([this] { autoSaveFile(toPlainText()); });
}

void MarkdownEditor::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
    QMarkdownTextEdit::dragEnterEvent(event);
}

void MarkdownEditor::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
    QMarkdownTextEdit::dragMoveEvent(event);
}

void MarkdownEditor::dropEvent(QDropEvent *event)
{
    const QMimeData *data = event->mimeData();

    if (!data->hasUrls()) {
        event->ignore();
        return;
    }

    const QList<QUrl> files = data->urls();

    QDir dir(getDir());

#if QT5
    QTextCursor c = cursorForPosition(event->pos());
#else
    QTextCursor c = cursorForPosition(event->position().toPoint());
#endif

    QMimeDatabase base;

    for (const QUrl &file : files) {
        const QString path = dir.relativeFilePath(file.toLocalFile());

        const QString mime = base.mimeTypeForFile(path).name();

        if (mime == L1("text/markdown"))
            Q_EMIT openFile(QFileInfo(file.toLocalFile()).absoluteFilePath());

        if (mime.startsWith(L1("image/"))) {
            c.insertText(STR("![%1](%2)\n").arg(file.fileName(), path));
            continue;
        }

        c.insertText(STR("[%1](%2)\n").arg(file.fileName(), path));
    }
    event->accept();
}

auto MarkdownEditor::setLanguages(const QStringList &langs) -> bool
{
    for (const QString &lang : langs) {
        setLanguage(lang);
    }

    return true;
}

auto MarkdownEditor::setLanguage(const QString &lang) -> bool
{
    threading::runFunction([this, lang] { checker->setLanguage(lang); });

    return true;
}

auto MarkdownEditor::filePath() -> QString
{
    if (getDir() == u'.')
        return getFileName() + STR("[*]");
    else {
        return STR("%1[*] (%2)")
            .arg(info.fileName(), info.path())
            .replace(common::homeDict(), QChar(u'~'));
    }
}

auto MarkdownEditor::getFileName() -> QString
{
    return info.fileName();
}

auto MarkdownEditor::getDir() -> QString
{
    return info.path();
}

void MarkdownEditor::changeSpelling(const bool c)
{
    checker->setSpellCheckingEnabled(c);
}

void MarkdownEditor::onLanguageChanged(const QString &l)
{
    const QString fileName = info.filePath();

    if (!fileName.isEmpty()) {
        setMapAttribute(fileName, checker->getLanguages());
    }
}

void MarkdownEditor::setText(const QByteArray &t, const QString &newFile, const bool setLangugae)
{
    if (!setLangugae) {
        if (checker)
            checker->clearDirtyBlocks();

        document()->setPlainText(QString::fromUtf8(t));
        document()->setModified(false);
        return;
    }

    if (!newFile.isEmpty())
        info.setFile(newFile);

    // Improve performance
    QMetaObject::invokeMethod(
        this,
        [this, t, newFile] {
            blockSignals(true);

            checker->clearDirtyBlocks();
            checker->setDocument(nullptr);

            if (mapContains(info.filePath()))
                setLanguages(mapAttribute(info.filePath()));

            document()->setPlainText(QString::fromUtf8(t));
            document()->setModified(false);

            checker->setDocument(document());

            blockSignals(false);
        },
        Qt::QueuedConnection);
}

MarkdownEditor::~MarkdownEditor() = default;
