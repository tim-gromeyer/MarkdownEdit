![Made with C++](https://forthebadge.com/images/badges/made-with-c-plus-plus.svg)
[![Powered by Qt](https://forthebadge.com/images/badges/powered-by-qt.svg)](https://qt.io)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![GitHub release](https://img.shields.io/github/release/software-made-easy/MarkdownEdit.svg)](https://github.com/software-made-easy/MarkdownEdit/releases/)


# MarkdownEdit 

MarkdownEdit, as the name suggests, is a simple and easy program to create and edit Markdown files.

# Features:

- Save and load Markdown files
- Undo-Redo
- Preview Markdown files
- Highlighting
- Auto complete(Close brackets, list, ...)
- Find/Replace
- Export to HTML
- Print
- Poor in resources

![Example](doc/images/Example.png)

# Dependecies:
Note: Most of them should already be installed

- libdouble-conversion3
- libgcc-s1
- libgl1
- libglvnd0
- libglx0
- libgraphite2-3
- libharfbuzz0b
- libicu66
- libpcre2-16-0
- libpng16-16
- libqt5core5a
- libqt5gui5
- libqt5printsupport5
- libqt5widgets5

On Debian based systems basicly call `apt-get install libdouble-conversion3 libgcc-s1 libgl1 libglvnd0 libglx0 libgraphite2-3 libharfbuzz0b libicu66 libpcre2-16-0 libpng16-16 libqt5core5a libqt5gui5 libqt5printsupport5 libqt5widgets5` to install these dependecies.

# To do:

- Fix Icon theme(Include standart icons)
- Open HTML files and convert them to Markdown(it works, but it's a bit buggy, that's why it's not available)
- Finish translations

# Credits

- The conversion from Markdown to HTML is done with the help of the [md4c](https://github.com/mity/md4c) - library by _Martin Mitáš_.
- The [widget](https://github.com/pbek/qmarkdowntextedit) used for writing was created by _Patrizio Bekerle_
