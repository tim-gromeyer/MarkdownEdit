QT       += core gui svg printsupport webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x051208    # disables all the APIs deprecated before Qt 5.12.8

INCLUDEPATH += \
    3dparty/md4c/src/


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    about.cpp \
    parser.cpp \
    settings.cpp

HEADERS += \
    mainwindow.h \
    about.h \
    parser.h \
    settings.h

FORMS += \
    mainwindow.ui \
    settings.ui

TRANSLATIONS += \
    translations/MarkdownEdit_de.ts

include(3rdparty/qmarkdowntextedit/qmarkdowntextedit.pri)

CONFIG += lrelease
CONFIG += embed_translations
CONFIG += link_pkgconfig
CONFIG -= qtquickcompiler

PKGCONFIG += QtSpell-qt5 md4c-html

VERSION = 0.5.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
CONFIG(release, debug | release): DEFINES += QT_NO_DEBUG_OUTPUT


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ressource.qrc

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

contains(ANDROID_TARGET_ARCH,arm64-v8a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}
