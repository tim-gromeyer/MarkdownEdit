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

#include "highlighter.h"

#include <QTextDocument>

Highliter::Highliter(QTextDocument *doc)
    : QSyntaxHighlighter(doc)
{
    QTextCharFormat format;

    _formats[Token::Doctype] = format;
    _formats[Token::CodeKeyWord] = format;
    _formats[Token::CodeString] = format;
    _formats[Token::CodeBuiltIn] = format;

    _formats[Token::Doctype].setForeground(QColor(143, 89, 2));
    _formats[Token::CodeKeyWord].setForeground(QColor(249, 38, 114));
    _formats[Token::CodeString].setForeground(QColor(163, 155, 78));
    _formats[Token::CodeBuiltIn].setForeground(QColor(1, 138, 15));
}

void Highliter::highlightBlock(const QString &text)
{
    const auto textLen = text.length();
    if (textLen == 0)
        return;

    // Highlight !DOCTYPE declaration
    int doctypeIndex = text.indexOf(QLatin1String("!DOCTYPE", 8));
    if (doctypeIndex != -1) {
        int endOfDoctype = text.indexOf(u'>', doctypeIndex);
        if (endOfDoctype != -1) {
            setFormat(doctypeIndex, endOfDoctype - doctypeIndex, _formats[Token::Doctype]);
        }
    }

    for (int i = 0; i < textLen; ++i) {
        if (text[i] == u'<' && text[i + 1] != u'!') {
            const auto found = text.indexOf(u'>', i);
            if (found > 0) {
                ++i;
                if (text[i] == u'/')
                    ++i;
                setFormat(i, (int) found - i, _formats[Token::CodeKeyWord]);
            }
        }

        if (text[i] == u'=') {
            int lastSpace = text.lastIndexOf(u' ', i - (text[i - 1] == u' ' ? 2 : 1));
            if (lastSpace > 0) {
                setFormat(lastSpace, i - lastSpace, _formats[Token::CodeBuiltIn]);
            }
        }

        if (text[i] == u'\"') {
            const int pos = i;
            int cnt = 1;
            ++i;
            while (i < textLen) {
                if (text[i] == u'\"') {
                    ++cnt;
                    ++i;
                    break;
                }
                ++i;
                ++cnt;
                if ((i + 1) >= textLen) {
                    ++cnt;
                    break;
                }
            }
            setFormat(pos, cnt, _formats[Token::CodeString]);
        }
    }
}
