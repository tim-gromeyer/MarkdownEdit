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


#ifndef HIGHLITER_H
#define HIGHLITER_H

#include <QSyntaxHighlighter>


class Highliter : public QSyntaxHighlighter
{
public:
    explicit Highliter(QTextDocument *doc);

    enum Token {
        CodeBlock,
        CodeKeyWord,
        CodeString,
        CodeBuiltIn,
    };
    Q_ENUM(Token)

protected:
    void highlightBlock(const QString &text) override;

private:
    QHash<Token, QTextCharFormat> _formats;
};

#endif // HIGHLITER_H
