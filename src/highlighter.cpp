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


#include "highlighter.h"

#include <QTextDocument>


Highliter::Highliter(QTextDocument *doc)
    : QSyntaxHighlighter(doc)
{
    QTextCharFormat format;

    _formats[Token::CodeBlock] = format;

    format.setForeground(QColor(249, 38, 114));
    _formats[Token::CodeKeyWord] = format;
    format.clearForeground();

    format.setForeground(QColor(163, 155, 78));
    _formats[Token::CodeString] = format;
    format.clearForeground();

    format.setForeground(QColor(1, 138, 15));
    _formats[Token::CodeBuiltIn] = format;
}

void Highliter::highlightBlock(const QString &text)
{
    if (currentBlock() == document()->firstBlock()) {
        setCurrentBlockState(228);
    } else {
        previousBlockState() == 228 ?
            setCurrentBlockState(228) :
            setCurrentBlockState(229);
    }

    const auto textLen = text.length();
    if (textLen == 0) return;

    setFormat(0, textLen, _formats[CodeBlock]);

    for (int i = 0; i < textLen; ++i) {
        if (text[i] == u'<' && text[i+1] != u'!') {
            const auto found = text.indexOf(u'>', i);
            if (found > 0) {
                ++i;
                if (text[i] == u'/') ++i;
                setFormat(i, found - i, _formats[CodeKeyWord]);
            }
        }

        if (text[i] == u'=') {
            int lastSpace = text.lastIndexOf(u' ', i);
            if (lastSpace == i-1) lastSpace = text.lastIndexOf(u' ', i-2);
            if (lastSpace > 0) {
                setFormat(lastSpace, i - lastSpace, _formats[CodeBuiltIn]);
            }
        }

        if (text[i] == u'\"') {
            const int pos = i;
            int cnt = 1;
            ++i;
            //bound check
            if ( (i+1) >= textLen) return;
            while (i < textLen) {
                if (text[i] == u'\"') {
                    ++cnt;
                    ++i;
                    break;
                }
                ++i; ++cnt;
                //bound check
                if ( (i+1) >= textLen) {
                    ++cnt;
                    break;
                }
            }
            setFormat(pos, cnt, _formats[CodeString]);
        }
    }
}
