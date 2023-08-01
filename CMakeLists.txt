cmake_minimum_required(VERSION 3.12)

project(MarkdownEdit VERSION 1.4.5
    DESCRIPTION "Simple program for editing Markdown files"
    HOMEPAGE_URL "https://tim-gromeyer.github.io/md_MarkdownEdit.html"
    LANGUAGES C CXX)
set(PROJECT_CONTACT "sakul8826@gmail.com")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/ui)

# Require c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt
find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Widgets LinguistTools PrintSupport Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools PrintSupport Svg)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Concurrent)

# Set default build type
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "Build type not specified. Release is used.")
    set(CMAKE_BUILD_TYPE "Release")
else()
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
endif()

# Link to pthread
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Improve performance
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    string(REPLACE "-O2" "-O3" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    string(REPLACE "-O2" "-O3" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

# Spell checker will not work with cross compilation
if(CMAKE_CROSSCOMPILING)
    message(STATUS "Cross compilation. The spell checker is not available!")
    option(DISABLE_SPELLCHECK "Turn off spell checker" ON)
else()
    option(DISABLE_SPELLCHECK "Turn off spell checker" OFF)
endif()

# For flatpak. Obsolete
option(FLATPAK "Build for flatpak" OFF)

if(FLATPAK)
    find_package(ECM 5.86.0 REQUIRED NO_MODULE)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

    include(KDEInstallDirs)
    include(KDECMakeSettings)

    include(ECMInstallIcons)

    add_compile_definitions(FLATPAK)

    install(FILES include/flatpak/appdata.xml DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo/)
endif()

# Project files
set(TS_FILES translations/MarkdownEdit_de_DE.ts)
set(RC_FILES
    ressources/icons.qrc
    ressources/ressources.qrc
    translations/translations.qrc
)

set(PROJECT_SOURCES
    ${RC_FILES}
    common.h
    src/highlighter.cpp
    src/highlighter.h
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/markdowneditor.cpp
    src/markdowneditor.h
    src/markdownparser.cpp
    src/markdownparser.h
    src/previewwidget.cpp
    src/previewwidget.h
    src/settings.cpp
    src/settings.h
    src/spellchecker.cpp
    src/spellchecker.h
    src/tableofcontents.cpp
    src/tableofcontents.h
    src/tablewidget.cpp
    src/tablewidget.h
    src/utils.cpp
    src/utils.h
    ui/mainwindow.ui
)

# Include windows icon
if(WIN32)
    list(APPEND PROJECT_SOURCES ressources/logo/Icon.rc)
endif()

# Include 3rdparty libs and translations
include(3rdparty/CMakeLists.txt)
include(cmake/Translation.cmake)

add_translations(TS ${TS_FILES})

# Build it!
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(markdownedit
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${TS}
    )

    # Dir for android settings
    set_property(TARGET markdownedit APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
else()
    if(ANDROID)
        add_library(markdownedit SHARED
            ${PROJECT_SOURCES}
            ${TS}
        )

        set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")

    else()
        add_executable(markdownedit
            ${PROJECT_SOURCES}
            ${TS}
        )
    endif()
endif()

# Nessesary for windows
IF(WIN32)
    SET(INTL_LDFLAGS -lintl)
ENDIF(WIN32)

# Find nuspell
if(DISABLE_SPELLCHECK)
    target_compile_definitions(markdownedit PUBLIC NO_SPELLCHECK)
else()
    find_package(Nuspell QUIET)

    if (Nuspell_FOUND AND Nuspell_VERSION GREATER_EQUAL 5.1.0)
        list(APPEND 3rdparty Nuspell::nuspell icuuc)
    else()
        message(WARNING "Nuspell not found. Spell checker is disabled!")
        target_compile_definitions(markdownedit PUBLIC NO_SPELLCHECK)
    endif()
endif()

if(Qt${QT_VERSION_MAJOR}Concurrent_FOUND)
    add_compile_definitions(CONCURRENT)
    list(APPEND 3rdparty Qt${QT_VERSION_MAJOR}::Concurrent)
endif()

target_include_directories(markdownedit PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(markdownedit PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PrintSupport
    Qt${QT_VERSION_MAJOR}::Svg
    ${INTL_LDFLAGS}
    ${3rdparty}
)

target_compile_definitions(markdownedit PUBLIC
    APP_VERSION="${PROJECT_VERSION}"
    CHECK_MARKDOWN
    HOMEPAGE="${PROJECT_HOMEPAGE_URL}"
    QT_NO_CAST_FROM_ASCII
    QT_NO_CAST_FROM_BYTEARRAY
    QT_NO_CAST_TO_ASCII
    QT_NO_FOREACH
    QT_NO_KEYWORDS
    QT_NO_NARROWING_CONVERSIONS_IN_CONNECT
    QT_NO_URL_CAST_FROM_STRING
    QT_STRICT_ITERATORS
    QT_USE_QSTRINGBUILDER
    QT${QT_VERSION_MAJOR}=1
)

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

# Packaging
include(GNUInstallDirs)

find_package(ECM 5.86.0 QUIET NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
if(ECM_FOUND)
    # install icons
    include(ECMInstallIcons)
    ecm_install_icons(
        ICONS
            ressources/logo/16-apps-MarkdownEdit.png
            ressources/logo/24-apps-MarkdownEdit.png
            ressources/logo/32-apps-MarkdownEdit.png
            ressources/logo/48-apps-MarkdownEdit.png
            ressources/logo/64-apps-MarkdownEdit.png
            ressources/logo/128-apps-MarkdownEdit.png
            ressources/logo/256-apps-MarkdownEdit.png
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons
        THEME hicolor
    )
endif()

install(TARGETS markdownedit
        BUNDLE  DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Desktop shortcut for linux
if(UNIX AND NOT EXMSCRIPTEN AND NOT ANDROID)
    install(FILES include/MarkdownEdit.desktop
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
endif()

# Icon for windows
if(WIN32)
    install(FILES ressources/logo/Icon.ico
            DESTINATION .)
endif()

if(${QT_VERSION} GREATER_EQUAL 6.3 AND NOT FLATPAK)
    qt_generate_deploy_app_script(
        TARGET markdownedit
        FILENAME_VARIABLE deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
endif()

include(cmake/Packaging.cmake)
