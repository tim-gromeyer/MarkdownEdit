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

#include <QString>
#include <QVariant>
#include <QPalette>
#include <QDir>
#include <QLocale>
#include <QHash>


QHash<QString, QVariant> languages_map;
QString currentDir;

QHash<QString, QVariant> getLanguageMap()
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

const QString mapAttribute(const QString &s)
{
    return languages_map[s].toString();
}

bool mapContains(const QString &s)
{
    return languages_map.contains(s);
}


QString currDir() {
    return currentDir;
}

void setCurrDir(const QString &newDir)
{
    currentDir = newDir;
}

namespace settings {
const bool isDarkMode()
{
    static const QColor back = QPalette().base().color();
    static int r, g, b, a;
    back.getRgb(&r, &g, &b, &a);

    static const bool dark = ((r + g + b + a) / 4) < 127;

    return dark;
}
}

namespace common {
const QString homeDict()
{
    static const QString homeDirectory = QDir::homePath();
    return homeDirectory;
}

const QStringList languages()
{
    return QLocale::system().uiLanguages();
}
}
