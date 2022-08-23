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
class QVariant;
class QString;

auto getLanguageMap() -> QHash<QString, QVariant>;
void setLanguageMap(const QHash<QString, QVariant> &m);
void setMapAttribute(const QString &, const QVariant &);
auto mapAttribute(const QString &) -> const QString;
auto mapContains(const QString &) -> bool;

namespace settings {
    auto isDarkMode() -> const bool;
}

namespace common {
auto homeDict() -> const QString;
auto languages() -> const QStringList;
}; // namespace common

#if !SPELLCHECK
#define NO_SPELLCHECK
#endif
