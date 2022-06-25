cmake_minimum_required(VERSION 3.12)

project(MarkdownEdit VERSION 1.3.0 LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/ui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools PrintSupport Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools PrintSupport Svg)

if (EMSCRIPTEN)
    add_compile_definitions(NO_THREADING)

    set(CONCURRENT
        ""
    )
else()
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Concurrent)

    set(CONCURRENT
        Qt${QT_VERSION_MAJOR}::Concurrent
    )
endif()

set(TS_FILES translations/MarkdownEdit_de_DE.ts)
set(RC_FILES
    ressources/icons.qrc
    ressources/ressources.qrc
    translations/translations.qrc
)

include(3rdparty/CMakeLists.txt)
include(cmake/TranslationUtils.cmake)

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/about.cpp
        src/about.h
        src/common.cpp
        src/markdowneditor.cpp
        src/markdowneditor.h
        src/markdownparser.cpp
        src/markdownparser.h
        src/highlighter.cpp
        src/highlighter.h
        src/spellchecker.cpp
        src/spellchecker.h
        src/previewwidget.cpp
        src/previewwidget.h
        common.h
        ui/mainwindow.ui
        ${RC_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_translation(QM_FILES ${TS_FILES})
    add_app_translations_resource(APP_RES ${QM_FILES})

    qt_add_executable(markdownedit
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${APP_RES}
    )

else()
    qt5_add_translation(QM_FILES ${TS_FILES})
    add_app_translations_resource(APP_RES ${QM_FILES})

    if(ANDROID)
        add_library(markdownedit SHARED
            ${PROJECT_SOURCES}
            ${APP_RES}
        )
    else()
        add_executable(markdownedit
            ${PROJECT_SOURCES}
            ${APP_RES}
        )
    endif()
endif()

IF(WIN32)
    SET(INTL_LDFLAGS -lintl)
ENDIF(WIN32)

if(ANDROID OR EMSCRIPTEN)
    target_compile_definitions(markdownedit PUBLIC NO_SPELLCHECK)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ENCHANT enchant-2)
endif()

if(ENCHANT_FOUND)
    target_include_directories(markdownedit PUBLIC ${ENCHANT_INCLUDE_DIRS})
else()
    target_compile_definitions(markdownedit PUBLIC NO_SPELLCHECK)
    message(STATUS "enchant-2 not found. Disable spell check.")
endif()

target_include_directories(markdownedit PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(markdownedit PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::PrintSupport Qt${QT_VERSION_MAJOR}::Svg ${CONCURRENT} ${ENCHANT_LDFLAGS} ${INTL_LDFLAGS} ${3rdparty})

target_compile_definitions(markdownedit PUBLIC APP_VERSION="${PROJECT_VERSION}" QT_DISABLE_DEPRECATED_BEFORE=0x051208 CHECK_MARKDOWN)

set_target_properties(markdownedit PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER org.qtproject.markdownedit
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(markdownedit)
endif()

if(LINUX)
    include(GNUInstallDirs)

    install(TARGETS markdownedit DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(FILES packaging/MarkdownEdit/usr/share/applications/MarkdownEdit.desktop
        DESTINATION "/usr/share/applications")
endif()
