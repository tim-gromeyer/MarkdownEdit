![Made with C++](https://forthebadge.com/images/badges/made-with-c-plus-plus.svg)
[![Powered by Qt](https://forthebadge.com/images/badges/powered-by-qt.svg)](https://qt.io)
[![CodeFactor](https://www.codefactor.io/repository/github/software-made-easy/markdownedit/badge/main)](https://www.codefactor.io/repository/github/software-made-easy/markdownedit/overview/main)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![GitHub release](https://img.shields.io/github/release/software-made-easy/MarkdownEdit.svg)](https://github.com/software-made-easy/MarkdownEdit/releases/)


# MarkdownEdit

MarkdownEdit, as the name suggests, is a simple and easy program to create and edit Markdown files.

-------

# Features:

- Spell checking
- Save and load Markdown files
- Undo-Redo
- Preview markdown files in real time(~1 ms)
- Syntax highlighting editor
- Auto complete(Close brackets, list, ...)
- Find/Replace(Ctrl+F/Ctrl+R)
- Export to HTML
- Poor in resources
- Quickly open recent files
- Open Source

![Example](doc/images/Example.png)

# Dependencies:
[Qt](https://qt.io/) and [enchant-2](https://github.com/AbiWord/enchant).

# Note(s): 
- To improve the performance when using images, disable the "Auto add file path to icon path" option in settings (Menu Extras->Options).
- When building via CMake, the translations **won't** work!

# To do:

- Open HTML files and convert them to Markdown(it works, but it's a bit buggy, that's why it's not available, to enable it anyways, define _QT_DEBUG_ when compiling [mainwindow.cpp](mainwindow.cpp) and [parser.cpp](parser.cpp))
- Synchronize scrolling(would be hard)

-------

# Build instructions
Execute the [build script](scripts/build.sh).

# Credits

- The conversion from Markdown to HTML is done with the help of the [md4c](https://github.com/mity/md4c) - library by _Martin Mitáš_.
- The [widget](https://github.com/pbek/qmarkdowntextedit) used for writing was created by _Patrizio Bekerle_.
- Spell checking is done with the [QtSpell](https://github.com/software-made-easy/qtspell) library based on the [QtSpell](https://github.com/manisandro/qtspell) library by _Sandro Mani_.
- The HTML syntax is highlighted using _Waqar Ahmed_'s [QSourceHighlite](https://github.com/Waqar144/QSourceHighlite) library.
