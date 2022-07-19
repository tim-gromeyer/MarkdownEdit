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

    const int textLen = text.length();
    if (textLen == 0) return;

    setFormat(0, textLen, _formats[CodeBlock]);

    for (int i = 0; i < textLen; ++i) {
        if (text[i] == QLatin1Char('<') && text[i+1] != QLatin1Char('!')) {
            const int found = text.indexOf(QLatin1Char('>'), i);
            if (found > 0) {
                ++i;
                if (text[i] == QLatin1Char('/')) ++i;
                setFormat(i, found - i, _formats[CodeKeyWord]);
            }
        }

        if (text[i] == QLatin1Char('=')) {
            int lastSpace = text.lastIndexOf(QLatin1Char(' '), i);
            if (lastSpace == i-1) lastSpace = text.lastIndexOf(QLatin1Char(' '), i-2);
            if (lastSpace > 0) {
                setFormat(lastSpace, i - lastSpace, _formats[CodeBuiltIn]);
            }
        }

        if (text[i] == QLatin1Char('\"')) {
            const int pos = i;
            int cnt = 1;
            ++i;
            //bound check
            if ( (i+1) >= textLen) return;
            while (i < textLen) {
                if (text[i] == QLatin1Char('\"')) {
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
