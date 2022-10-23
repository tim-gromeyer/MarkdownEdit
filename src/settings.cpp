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

#include "settings.h"

#include <QDir>
#include <QHash>
#include <QLocale>
#include <QPalette>
#include <QString>
#include <QVariant>

static QHash<QString, QVariant> languages_map;


auto getLanguageMap() -> QHash<QString, QVariant>
{
    return languages_map;
}

void setLanguageMap(const QHash<QString, QVariant> &m)
{
    languages_map = m;
}

void setMapAttribute(const QString &name, const QVariant &value)
{
    languages_map[name] = value;
}

auto mapAttribute(const QString &s) -> const QString
{
    return languages_map[s].toString();
}

auto mapContains(const QString &s) -> bool
{
    return languages_map.contains(s);
}

namespace settings {
auto isDarkMode() -> bool
{
    static const QColor back = QPalette().base().color();
    static int r, g, b, a;
    back.getRgb(&r, &g, &b, &a);

    static const bool dark = ((r + g + b + a) / 4) < 127;

    return dark;
}
} // namespace settings

namespace common {
auto homeDict() -> const QString
{
    static const QString homeDirectory = QDir::homePath();
    return homeDirectory;
}

auto languages() -> const QStringList
{
    return QLocale::system().uiLanguages();
}
} // namespace common
