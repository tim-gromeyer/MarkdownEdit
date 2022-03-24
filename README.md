![Made with C++](https://forthebadge.com/images/badges/made-with-c-plus-plus.svg)
[![Powered by Qt](https://forthebadge.com/images/badges/powered-by-qt.svg)](https://qt.io)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![GitHub release](https://img.shields.io/github/release/software-made-easy/MarkdownEdit.svg)](https://github.com/software-made-easy/MarkdownEdit/releases/)


# MarkdownEdit

MarkdownEdit, as the name suggests, is a simple and easy program to create and edit Markdown files.

# Features:

- Save and load Markdown files
- Undo-Redo
- Preview markdown files in real time
- Highlighting
- Auto complete(Close brackets, list, ...)
- Find/Replace
- Export to HTML
- Print
- Poor in resources
- Edit HTML files

![Example](doc/images/Example.png)

# Dependecies:
NOTE: There is a [script](scripts/requirements.sh) to find all dependencies. The script dependencies are 'libc-bin' and 'dlocate'

- libqt5core5a
- libqt5gui5
- libqt5network5
- libqt5positioning5
- libqt5printsupport5
- libqt5qml5
- libqt5quick5
- libqt5quickwidgets5
- libqt5webchannel5
- libqt5webenginecore5
- libqt5webenginewidgets5
- libqt5widgets5

On Debian based systems basicly call `apt-get install libqt5core5a libqt5gui5 libqt5network5 libqt5positioning5 libqt5printsupport5 libqt5qml5 libqt5quick5 libqt5quickwidgets5 libqt5webchannel5 libqt5webenginecore5 libqt5webenginewidgets5 libqt5widgets5` to install these dependecies.

# To do:

- Fix Icon theme(Include standart icons)
- Open HTML files and convert them to Markdown(it works, but it's a bit buggy, that's why it's not available, to enable it anyways, define _QT_DEBUG_ when compiling [mainwindow.cpp](mainwindow.cpp))
- Synchronize scrolling

# Credits

- The conversion from Markdown to HTML is done with the help of the [md4c](https://github.com/mity/md4c) - library by _Martin Mitáš_.
- The [widget](https://github.com/pbek/qmarkdowntextedit) used for writing was created by _Patrizio Bekerle_