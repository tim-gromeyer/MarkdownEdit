![Made with C++](https://forthebadge.com/images/badges/made-with-c-plus-plus.svg)
[![Powered by Qt](https://forthebadge.com/images/badges/powered-by-qt.svg)](https://qt.io)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![GitHub release](https://img.shields.io/github/release/software-made-easy/MarkdownEdit.svg)](https://github.com/software-made-easy/MarkdownEdit/releases/)


# MarkdownEdit

MarkdownEdit, as the name suggests, is a simple and easy program to create and edit Markdown files.

-------

# Features:

- Spell checking
- Save and load Markdown files
- Undo-Redo
- Preview markdown files in real time
- Syntax highlighting editor
- Auto complete(Close brackets, list, ...)
- Find/Replace(Ctrl+F/Ctrl+R)
- Export to HTML
- Print
- Poor in resources
- Edit HTML files(In Debug mode)
- Quickly open recent files
- Open Source

![Example](doc/images/Example.png)

# Dependencies:
There is a [script](scripts/requirements.sh) to find all dependencies. The script dependencies are `libc-bin` and `dlocate`.

# To do:

- Fix Icons
- Open HTML files and convert them to Markdown(it works, but it's a bit buggy, that's why it's not available, to enable it anyways, define _QT_DEBUG_ when compiling [mainwindow.cpp](mainwindow.cpp))
- Synchronize scrolling
- Auto save
- Word Count(maybe)

-------

# Build instructions
Execute the [build script](scripts/build.sh).

# Credits

- The conversion from Markdown to HTML is done with the help of the [md4c](https://github.com/mity/md4c) - library by _Martin Mitáš_.
- The [widget](https://github.com/pbek/qmarkdowntextedit) used for writing was created by _Patrizio Bekerle_.
- Spell checking is done using the [QtSpell](https://github.com/manisandro/qtspell) library by _Sandro Mani_.
