/**
 ** This file is part of the MarkdownEdit project.
 ** Copyright 2022 Tim Gromeyer <sakul8826@gmail.com>.
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


#pragma once

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

protected:
    void highlightBlock(const QString &text) override;

private:
    QHash<Token, QTextCharFormat> _formats;
};
