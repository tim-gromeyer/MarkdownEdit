cmake_minimum_required(VERSION 3.12)

project(MarkdownEdit VERSION 1.2.0 LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/ui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(APP_VERSION="${PROJECT_VERSION}" QT_DISABLE_DEPRECATED_BEFORE=0x051208)

find_package(PkgConfig)
PKG_CHECK_MODULES(ENCHANT REQUIRED enchant-2)
include_directories(${ENCHANT_INCLUDE_DIRS})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools PrintSupport)

set(TS_FILES translations/MarkdownEdit_de.ts)
set(RC_FILES ressources/ressources.qrc)
set(3RDPARTY_FILES
        3rdparty/md4c/src/entity.h
        3rdparty/md4c/src/entity.c
        3rdparty/md4c/src/md4c.h
        3rdparty/md4c/src/md4c.c
        3rdparty/md4c/src/md4c-html.h
        3rdparty/md4c/src/md4c-html.c
        3rdparty/qmarkdowntextedit/markdownhighlighter.h
        3rdparty/qmarkdowntextedit/markdownhighlighter.cpp
        3rdparty/qmarkdowntextedit/linenumberarea.h
        3rdparty/qmarkdowntextedit/qmarkdowntextedit.h
        3rdparty/qmarkdowntextedit/qmarkdowntextedit.cpp
        3rdparty/qmarkdowntextedit/qownlanguagedata.h
        3rdparty/qmarkdowntextedit/qownlanguagedata.cpp
        3rdparty/qmarkdowntextedit/qplaintexteditsearchwidget.h
        3rdparty/qmarkdowntextedit/qplaintexteditsearchwidget.ui
        3rdparty/qmarkdowntextedit/qplaintexteditsearchwidget.cpp
        3rdparty/qmarkdowntextedit/media.qrc
)

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/about.cpp
        src/about.h
        src/parser.cpp
        src/parser.h
        src/highlighter.cpp
        src/highlighter.h
        src/spellchecker.cpp
        src/spellchecker.h
        ui/mainwindow.ui
        ${3RDPARTY_FILES}
        ${RC_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR}/translations ${TS_FILES})

    qt_add_executable(MarkdownEdit
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${QM_FILES}
    )
else()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR}/translations ${TS_FILES})

    if(ANDROID)
        add_library(MarkdownEdit SHARED
            ${PROJECT_SOURCES}
            ${QM_FILES}
        )
    else()
        add_executable(MarkdownEdit
            ${PROJECT_SOURCES}
            ${QM_FILES}
        )
    endif()
endif()

IF(WIN32)
    SET(INTL_LDFLAGS -lintl)
ENDIF(WIN32)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/qmarkdowntextedit)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/md4c/src)
target_link_libraries(MarkdownEdit PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::PrintSupport ${ENCHANT_LDFLAGS} ${INTL_LDFLAGS})

set_target_properties(MarkdownEdit PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MarkdownEdit)
endif()
