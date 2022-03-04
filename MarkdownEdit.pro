QT       += core gui webenginewidgets webchannel svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x051208    # disables all the APIs deprecated before Qt 5.12.8


HEADERS += \
    mainwindow.h \
    previewpage.h \
    document.h \
    about.h

SOURCES = \
    main.cpp \
    mainwindow.cpp \
    previewpage.cpp \
    document.cpp \
    about.cpp

FORMS += \
    mainwindow.ui

RESOURCES = \
    resources/markdowneditor.qrc

DISTFILES += \
    resources/3rdparty/MARKDOWN-LICENSE.txt \
    resources/3rdparty/MARKED-LICENSE.txt

# Disable Qt Quick compiler because the example doesn't use QML, but more importantly so that
# the source code of the .js files is not removed from the embedded qrc file.
CONFIG -= qtquickcompiler

CONFIG += lrelease
CONFIG += embed_translations

VERSION = 1.0.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
