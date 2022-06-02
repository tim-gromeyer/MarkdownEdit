cmake_minimum_required(VERSION 3.12)

project(MarkdownEdit VERSION 1.2.0 LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/ui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(APP_VERSION="${PROJECT_VERSION}" QT_DISABLE_DEPRECATED_BEFORE=0x051208 CHECK_MARKDOWN CMAKE)

if(ANDROID)
    add_compile_definitions(OS_ANDROID)
else()
    find_package(PkgConfig)
    PKG_CHECK_MODULES(ENCHANT REQUIRED enchant-2)
    include_directories(${ENCHANT_INCLUDE_DIRS})
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools PrintSupport)

set(TS_FILES translations/MarkdownEdit_de.ts)
set(RC_FILES ressources/ressources.qrc)

include(3rdparty/CMakeLists.txt)

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/about.cpp
        src/about.h
        src/markdownparser.cpp
        src/markdownparser.h
        src/highlighter.cpp
        src/highlighter.h
        src/spellchecker.cpp
        src/spellchecker.h
        common.h
        ui/mainwindow.ui
        ${RC_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_translation(markdownedit QM_FILES ${TS_FILES})

    qt_add_executable(markdownedit
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${QM_FILES}
    )

else()
    qt5_add_translation(markdownedit QM_FILES ${TS_FILES})

    if(ANDROID)
        add_library(markdownedit SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(markdownedit
            ${PROJECT_SOURCES}
        )
    endif()
endif()

IF(WIN32)
    SET(INTL_LDFLAGS -lintl)
ENDIF(WIN32)

include_directories(${CMAKE_SOURCE_DIR}/src)
target_link_libraries(markdownedit PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::PrintSupport ${ENCHANT_LDFLAGS} ${INTL_LDFLAGS} 3rdparty)

set_target_properties(markdownedit PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(markdownedit)
endif()
