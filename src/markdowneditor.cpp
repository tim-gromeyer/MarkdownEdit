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


#include "markdowneditor.h"
#include "spellchecker.h"
#include "settings.h"

#include <QFileInfo>
#include <QMimeData>
#include <QImageReader>
#include <QSignalBlocker>
#include <QDir>


MarkdownEditor::MarkdownEditor(QWidget *parent)
    : QMarkdownTextEdit(parent, false)
{
    TextEditProxy *proxy = new TextEditProxyT<QPlainTextEdit>(this, this);

    checker = new SpellChecker(proxy);
    connect(checker, &SpellChecker::languageChanged,
            this, &MarkdownEditor::onLanguageChanged);

    setAcceptDrops(true);

    searchWidget()->setDarkMode(settings::isDarkMode());
}

void MarkdownEditor::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

void MarkdownEditor::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void MarkdownEditor::dropEvent(QDropEvent *event)
{
    const QMimeData *data = event->mimeData();

    if (!data->hasUrls())
        return event->ignore();

    const QList<QUrl> files = data->urls();

    QDir dir;

    QTextCursor c = cursorForPosition(event->pos());

    for (const QUrl &file : files) {
        const QString path = dir.relativeFilePath(file.toLocalFile());

        if (QImageReader::imageFormat(file.toLocalFile()).isEmpty()) {
            c.insertText(path + QLatin1Char('\n'));
            continue;
        }

        c.insertText(QStringLiteral("![%1](%2)\n").arg(file.fileName(),
                                                    path));
    }
    event->accept();
}

bool MarkdownEditor::setLanguage(const QString &lang)
{
    if (SpellChecker::getLanguageList().contains(lang))
        return checker->setLanguage(lang);
    else
        return false;
}

QString MarkdownEditor::filePath()
{
    if (getDir() == QLatin1Char('.'))
        return getFileName() + QLatin1String("[*]");
    else {
        const QFileInfo info(fileName);
        return QStringLiteral("%1[*] (%2)").arg(info.fileName(),
                                                info.path()).replace(common::homeDict(),
                                                                     QChar('~'));
    }
}

QString MarkdownEditor::getFileName()
{
    return QFileInfo(fileName).fileName();
}

QString MarkdownEditor::getDir()
{
    return QFileInfo(fileName).path();
}

void MarkdownEditor::changeSpelling(const bool &c)
{
    checker->setSpellCheckingEnabled(c);
}

void MarkdownEditor::onLanguageChanged(const QString &l)
{
    if (!fileName.isEmpty()) {
        setMapAttribute(fileName, l);
    }
}

void MarkdownEditor::setText(const QString &t, const QString &newFile, const bool setLangugae)
{
    if (!setLangugae) {
        if (checker)
            checker->clearDirtyBlocks();

        document()->setPlainText(t);
        return document()->setModified(false);
    }

    if (!newFile.isEmpty())
        fileName = newFile;

    // Improve performance
    QMetaObject::invokeMethod(this, [this, t]{
        blockSignals(true);

        checker->clearDirtyBlocks();
        checker->setDocument(nullptr);

        if (mapContains(fileName))
            setLanguage(mapAttribute(fileName));

        document()->setPlainText(t);
        document()->setModified(false);
        checker->setDocument(document());

        blockSignals(false);
    }, Qt::QueuedConnection);
}
