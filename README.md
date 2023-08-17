![Made with C++](https://forthebadge.com/images/badges/made-with-c-plus-plus.svg)
[![Powered by Qt](https://forthebadge.com/images/badges/powered-by-qt.svg)](https://qt.io)
[![CodeFactor](https://www.codefactor.io/repository/github/tim-gromeyer/markdownedit/badge/main)](https://www.codefactor.io/repository/github/tim-gromeyer/markdownedit/overview/main)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![GitHub release](https://img.shields.io/github/release/tim-gromeyer/MarkdownEdit.svg)](https://github.com/tim-gromeyer/MarkdownEdit/releases/)


# [MarkdownEdit](https://tim-gromeyer.github.io/MarkdownEdit/)

Experience the speed and simplicity of MarkdownEdit.

## Table of Contents

1. [Features](#features)
2. [Preview](#preview)
3. [Dependencies](#dependencies)
4. [Build instructions](#build-instructions)
5. [Credits](#credits)


## Features

MarkdownEdit offers a range of powerful features to make editing and creating Markdown files quick and easy:

- **Spell checker**
- Save and load Markdown files quickly and easily
- Powerful undo/redo capabilities
- **Real-time Markdown preview**
- **Editor with syntax highlighting**
- Auto-completion for brackets, lists, and more
- Find/replace functionality for efficient editing
- Export to HTML and PDF
- Lightweight and resource-efficient
- Quick access to recent files from the toolbar or file menu
- Open source and native look and feel

![Example](doc/images/Example.png)

## Preview

(Note: Bugs are expected, it's a desktop app and not a web app)

You can preview MarkdownEdit in action [here](https://tim-gromeyer.github.io/MarkdownEdit/markdownedit.html).

Please note:

- The online preview may not perform as well as the actual app
- Spell check is not available in the online preview
- Drag and drop functionality is not supported in the online preview

## Dependencies

MarkdownEdit requires [Qt](https://qt.io/) and [nuspell](https://github.com/nuspell/nuspell) (optional) to be installed.

## Build instructions

To build MarkdownEdit, follow these steps:

1. Clone the MarkdownEdit repository: `git clone https://github.com/tim-gromeyer/MarkdownEdit --depth=1 && cd MarkdownEdit`
2. Initialize and update the submodules: `git submodule update --init --recursive -j 3 --depth=1`
3. Create a build directory and navigate to it: `mkdir build && cd build`
4. Generate a Makefile using CMake: `cmake ..`
5. Build the project: `cmake --build . -j4`


## Credits

- Thanks to [Waqar Ahmed](https://github.com/Waqar144) for help with development.
- The conversion from Markdown to HTML is done using the [md4c](https://github.com/mity/md4c) library by *Martin Mitáš*.
- The [widget](https://github.com/pbek/qmarkdowntextedit) used for writing was created by *Patrizio Bekerle*.
