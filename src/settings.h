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


#pragma once

#include "common.h"

#include <QString>

// Namespace
template <typename Key, typename T> class QHash;
class QVariant;

QHash<QString, QVariant> getLanguageMap();
void setLanguageMap(const QHash<QString, QVariant> &m);
void setMapAttribute(const QString &, const QVariant &);
const QString mapAttribute(const QString &);
bool mapContains(const QString &);

QString currDir();
void setCurrDir(const QString &);

namespace settings {
    const bool isDarkMode();
}

namespace common {
const QString homeDict();
const QStringList languages();
}; // namespace common

#if !SPELLCHECK
#define NO_SPELLCHECK
#endif
