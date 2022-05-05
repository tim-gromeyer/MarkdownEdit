![Made with C++](https://forthebadge.com/images/badges/made-with-c-plus-plus.svg)
[![Powered by Qt](https://forthebadge.com/images/badges/powered-by-qt.svg)](https://qt.io)
[![CodeFactor](https://www.codefactor.io/repository/github/software-made-easy/markdownedit/badge/main)](https://www.codefactor.io/repository/github/software-made-easy/markdownedit/overview/main)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![GitHub release](https://img.shields.io/github/release/software-made-easy/MarkdownEdit.svg)](https://github.com/software-made-easy/MarkdownEdit/releases/)


# MarkdownEdit

MarkdownEdit, as the name suggests, is a simple and easy program to create and edit Markdown files.

-------

## Features:

- Spell checking
- Save and load Markdown files
- Undo-Redo
- Preview markdown files in real time(~1 ms)
- Syntax highlighting editor
- Auto complete(Close brackets, list, ...)
- Find/Replace(Ctrl+F/Ctrl+R)
- Export to HTML
- Poor in resources
- Quickly open recent files(Menu <u>F</u>ile->Menu Recently opened)
- Open Source
- Native look and feel

![Example](doc/images/Example.png)

## Dependencies:
[Qt](https://qt.io/) and [enchant-2](https://github.com/AbiWord/enchant).

## Note(s): 
- To improve the performance when using images, disable the "Auto add file path to icon path" option (Menu <u>V</u>iew).
- When building via CMake, the translations **won't** work!


-------

## Build instructions
Execute the [build script](scripts/build.sh) or follow the instructions below.

- Clone MarkdownEdit: `git clone https://github.com/software-made-easy/MarkdownEdit && cd MarkdownEdit`
- Clone all repositories needed by MarkdownEdit by running the command `git submodule update --init --recursive -j 3`.
- Create the build folder: `mkdir build && cd build`
- Now create a Makefile using CMake: `cmake ..` or qmake(recommended): `qmake ..`
- Execute make: `make -j4`

To sum up:
```bash
git clone https://github.com/software-made-easy/MarkdownEdit && cd MarkdownEdit
git submodule update --init --recursive -j 3
mkdir build && cd build
qmake ..
make
```

## Credits

- The conversion from Markdown to HTML is done with the help of the [md4c](https://github.com/mity/md4c) - library by *Martin Mitáš*.
- The [widget](https://github.com/pbek/qmarkdowntextedit) used for writing was created by *Patrizio Bekerle*.
- Spell checking is done with the [QtSpell](https://github.com/software-made-easy/qtspell) library based on the [QtSpell](https://github.com/manisandro/qtspell) library by *Sandro Mani*.