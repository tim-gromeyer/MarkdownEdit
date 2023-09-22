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

#include "settings.h"

#include <QDir>
#include <QHash>
#include <QLocale>
#include <QPalette>
#include <QString>
#include <QVariant>

#ifdef Q_OS_WASM
#include <emscripten.h>
#endif

QHash<QString, QVariant> languages_map;

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
#ifdef Q_OS_WASM
EM_JS(bool, is_dark_mode, (), { return window.matchMedia("(prefers-color-scheme: dark)").matches; });
#endif
auto isDarkMode() -> bool
{
#ifdef Q_OS_WASM
    return is_dark_mode();
#endif
    static const QPalette defaultPalette;
    static bool dark = defaultPalette.color(QPalette::WindowText).lightness()
                       > defaultPalette.color(QPalette::Window).lightness();

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

QPalette darkPalette()
{
    // Create a QPalette object
    QPalette darkPalette;

    // Set the background color and text color for QWidget
    darkPalette.setColor(QPalette::Window, QColor(51, 51, 51));        // #333333
    darkPalette.setColor(QPalette::WindowText, QColor(255, 255, 255)); // #FFFFFF

    darkPalette.setColor(QPalette::HighlightedText, QColor(0, 123, 255)); // #007BFF
    darkPalette.setColor(QPalette::Highlight, QColor(85, 85, 85));        // #555555

    // Set hyperlink text color and underline
    darkPalette.setColor(QPalette::Link, QColor(0, 123, 255));        // #007BFF
    darkPalette.setColor(QPalette::LinkVisited, QColor(0, 123, 255)); // #007BFF
    darkPalette.setColor(QPalette::Text, darkPalette.color(QPalette::WindowText));

    return darkPalette;
}
