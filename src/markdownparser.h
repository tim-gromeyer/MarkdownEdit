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


#pragma once

#include <QObject>


class Parser
{
public:
    enum Dialect {
        Commonmark = 0,
        GitHub = 1,
        Doxygen = 2
    };

    Q_REQUIRED_RESULT static auto toHtml(const QString &in, const int dia = GitHub, const size_t = 3) -> QString;
    Q_REQUIRED_RESULT static auto heading2HTML(const QString &in) -> QString;

    Q_REQUIRED_RESULT static auto toMarkdown(const QString &in) -> QString;
};
