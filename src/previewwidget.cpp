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


#include "previewwidget.h"
#include "settings.h"

#include <QUrl>
#include <QDesktopServices>
#include <QFile>
#include <QScrollBar>


PreviewWidget::PreviewWidget(QWidget *parent)
    : QTextBrowser(parent)
{
    QTextBrowser::setOpenLinks(false);
    QTextBrowser::setOpenExternalLinks(true);
    QTextEdit::setReadOnly(true);
    QTextEdit::setUndoRedoEnabled(false);
    QTextEdit::setAcceptRichText(false);

    connect(this, &QTextBrowser::anchorClicked,
            this, &PreviewWidget::openUrl);
}

void PreviewWidget::setHtml(const QString &html)
{
    blockSignals(true);

    setUpdatesEnabled(false);

    const int v = verticalScrollBar()->value();

    QTextEdit::setHtml(html);

    verticalScrollBar()->setValue(v);

    blockSignals(false);
    setUpdatesEnabled(true);
}

void PreviewWidget::openUrl(const QUrl &url)
{
    const QString s = url.toString();
    const QString filePath = currDir() + QChar('/') + s;

    if (s.startsWith(QLatin1String("http")))
        QDesktopServices::openUrl(url);

    else
    if (QFile::exists(filePath))
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));

    else
    if (s.startsWith(QLatin1Char('#')))
        scrollToHeader(s);
}

void PreviewWidget::scrollToHeader(QString name)
{
    if (name.startsWith(QLatin1Char('#')))
        name.remove(0, 1);

    name.replace(QChar('_'), QChar(' '));

    QTextCursor c = textCursor();

    const QString text = toPlainText();

    const int length = name.length();

    int i = 0;

    while (true) {
        i = text.indexOf(name, i, Qt::CaseInsensitive);

        if (i == -1) return;

        c.setPosition(i, QTextCursor::KeepAnchor);

        const QTextBlockFormat f = c.blockFormat();
        if (f.headingLevel() > 0) {
            c.setPosition(i, QTextCursor::MoveAnchor);
            setTextCursor(c);

            // cursorRect().center().x() is always 4
            verticalScrollBar()->setValue(verticalScrollBar()->value() + (cursorRect().top() - rect().center().x())
                                          - (cursorRect().height() /2));

            return;
        }

        // Prevent invinity loop
        i += length;
    }
}
