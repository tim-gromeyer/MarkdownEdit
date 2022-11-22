cmake_minimum_required(VERSION 3.20)

project(MarkdownEdit VERSION 1.4.4
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
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools PrintSupport Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools PrintSupport Svg)

# Detect if it's on linux
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux" AND NOT CMAKE_CROSSCOMPILING)
    set(LINUX TRUE)
endif()

# Set default build type
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "Build type not specified. Release is used.")
    set(CMAKE_BUILD_TYPE "Release")
else()
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
endif()

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

    add_compile_definitions(
        FLATPAK
    )
endif()

# Project files
set(TS_FILES translations/MarkdownEdit_de_DE.ts)
set(RC_FILES
    ressources/icons.qrc
    ressources/ressources.qrc
    translations/translations.qrc
)

set(PROJECT_SOURCES
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
    src/tablewidget.cpp
    src/tablewidget.h
    src/tableofcontents.h
    src/tableofcontents.cpp
    ui/mainwindow.ui
    ${RC_FILES}
)

# Include windows icon
if(WIN32)
    list(APPEND PROJECT_SOURCES ressources/logo/Icon.rc)
endif()

# Include 3rdparty libs and translations
include(3rdparty/CMakeLists.txt)
include(cmake/TranslationUtils.cmake)

# Build it!
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_translation(QM_FILES ${TS_FILES})
    add_app_translations_resource(APP_RES ${QM_FILES})

    qt_add_executable(markdownedit
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${APP_RES}
    )

    # Dir for android settings
    set_property(TARGET markdownedit APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
else()
    qt5_add_translation(QM_FILES ${TS_FILES})
    add_app_translations_resource(APP_RES ${QM_FILES})

    if(ANDROID)
        add_library(markdownedit SHARED
            ${PROJECT_SOURCES}
            ${APP_RES}
        )

        set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")

    else()
        add_executable(markdownedit
            ${PROJECT_SOURCES}
            ${APP_RES}
        )
    endif()
endif()

# Nessesary for windows
IF(WIN32)
    SET(INTL_LDFLAGS -lintl)
ENDIF(WIN32)

find_package(PkgConfig QUIET)

# Find enchant-2
if(DISABLE_SPELLCHECK)
    message(STATUS "Spell check disabled")
    target_compile_definitions(markdownedit PUBLIC NO_SPELLCHECK)
elseif(PKG_CONFIG_FOUND)
    pkg_check_modules(ENCHANT enchant-2)

    if(ENCHANT_FOUND)
        message(STATUS "enchant-2 found. Enable spell check")
        target_include_directories(markdownedit PUBLIC ${ENCHANT_INCLUDE_DIRS})
        set(ENCHANT ${ENCHANT_LDFLAGS})
    else()
        target_compile_definitions(markdownedit PUBLIC NO_SPELLCHECK)
        message(STATUS "enchant-2 not found. Disable spell check.")
    endif()
else()
    message(WARNING "PkgConfig not found!")
    target_compile_definitions(markdownedit PUBLIC NO_SPELLCHECK)
endif()

target_include_directories(markdownedit PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(markdownedit PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::PrintSupport Qt${QT_VERSION_MAJOR}::Svg ${ENCHANT} ${INTL_LDFLAGS} ${3rdparty})

target_compile_definitions(markdownedit PUBLIC
    APP_VERSION="${PROJECT_VERSION}"
    CHECK_MARKDOWN
    HOMEPAGE="${PROJECT_HOMEPAGE_URL}"
    QT_DISABLE_DEPRECATED_BEFORE=0x051208
    QT_NO_CAST_FROM_ASCII
    QT_NO_CAST_FROM_BYTEARRAY
    QT_NO_CAST_TO_ASCII
    QT_NO_FOREACH
    QT_NO_KEYWORDS
    QT_NO_NARROWING_CONVERSIONS_IN_CONNECT
    QT_NO_URL_CAST_FROM_STRING
    QT_STRICT_ITERATORS
    QT_USE_QSTRINGBUILDER
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
if(LINUX)
    install(FILES packaging/MarkdownEdit.desktop
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
endif()

# Icon for windows
if(WIN32)
    install(FILES ressources/logo/Icon.ico
            DESTINATION .)
endif()

if(${QT_VERSION} GREATER_EQUAL 6.3)
    qt_generate_deploy_app_script(
        TARGET markdownedit
        FILENAME_VARIABLE deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
endif()

include(cmake/Packaging.cmake)
