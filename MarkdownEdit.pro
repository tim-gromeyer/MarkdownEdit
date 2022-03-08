QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x051208    # disables all the APIs deprecated before Qt 5.12.8

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    about.cpp

HEADERS += \
    mainwindow.h \
    about.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    MarkdownEdit_de.ts

include(3dparty/qmarkdowntextedit/qmarkdowntextedit.pri)
include(3dparty/md4c/md4c.pri)

CONFIG += lrelease
CONFIG += embed_translations
CONFIG -= qtquickcompiler

VERSION = 0.5.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ressource.qrc
