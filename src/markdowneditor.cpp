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

#include <QDir>
#include <QFileInfo>
#include <QMimeData>
#include <QMimeDatabase>
#include <QRunnable>
#include <QSignalBlocker>

#include <thread>

MarkdownEditor::MarkdownEditor(QWidget *parent)
    : QMarkdownTextEdit(parent, false)
{
    checker = new SpellChecker(this);
    connect(checker, &SpellChecker::languageChanged, this, &MarkdownEditor::onLanguageChanged);

    setAcceptDrops(true);

    searchWidget()->setDarkMode(settings::isDarkMode());
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
        setMapAttribute(fileName, l);
    }
}

void MarkdownEditor::setText(const QByteArray &t, const QString &newFile, const bool setLangugae)
{
    if (!setLangugae) {
        if (checker)
            checker->clearDirtyBlocks();

        document()->setPlainText(QString::fromLocal8Bit(t));
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
                setLanguage(mapAttribute(info.filePath()));

            document()->setPlainText(QString::fromLocal8Bit(t));
            document()->setModified(false);

            checker->setDocument(document());

            blockSignals(false);
        },
        Qt::QueuedConnection);
}

MarkdownEditor::~MarkdownEditor() = default;
