cmake_minimum_required(VERSION 3.12)

project(MarkdownEdit VERSION 1.4.3 LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/ui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools PrintSupport)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux" AND NOT CMAKE_CROSSCOMPILING)
    set(LINUX TRUE)
endif()

if(CMAKE_CROSSCOMPILING)
    message(STATUS "Cross compilation. The spell checker is not available!")
    option(DISABLE_SPELLCHECK "Turn off spell checker" ON)
else()
    option(DISABLE_SPELLCHECK "Turn off spell checker" OFF)
endif()
option(FLATPAK "Build for flatpak" OFF)

if(FLATPAK)
    find_package(ECM 5.86.0 REQUIRED NO_MODULE)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

    include(KDEInstallDirs)
    include(KDECMakeSettings)
    # include(KDECompilerSettings NO_POLICY_SCOPE) # enchant wont compile

    include(ECMInstallIcons)

    add_compile_definitions(
        FLATPAK
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
        src/settings.h
        src/settings.cpp
        src/tablewidget.h
        src/tablewidget.cpp
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

IF(WIN32)
    SET(INTL_LDFLAGS -lintl)
ENDIF(WIN32)

if(DISABLE_SPELLCHECK)
    message(STATUS "Sell check disabled")
    target_compile_definitions(markdownedit PUBLIC NO_SPELLCHECK)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ENCHANT enchant-2)

    if(ENCHANT_FOUND)
        message(STATUS "enchant-2 found. Enable spell check")
        target_include_directories(markdownedit PUBLIC ${ENCHANT_INCLUDE_DIRS})
        set(ENCHANT ${ENCHANT_LDFLAGS})
    else()
        target_compile_definitions(markdownedit PUBLIC NO_SPELLCHECK)
        message(STATUS "enchant-2 not found. Disable spell check.")
    endif()
endif()

# Svg package needed for android
if(ANDROID OR WIN32)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Svg)
    list(APPEND 3rdparty Qt${QT_VERSION_MAJOR}::Svg)
endif()

target_include_directories(markdownedit PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(markdownedit PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::PrintSupport ${ENCHANT} ${INTL_LDFLAGS} ${3rdparty})

target_compile_definitions(markdownedit PUBLIC
    APP_VERSION="${PROJECT_VERSION}"
    QT_DISABLE_DEPRECATED_BEFORE=0x051208
    CHECK_MARKDOWN
    QT_NO_CAST_FROM_ASCII
    QT_NO_CAST_FROM_BYTEARRAY
    QT_NO_CAST_TO_ASCII
    QT_NO_FOREACH
    QT_NO_KEYWORDS
    QT_NO_NARROWING_CONVERSIONS_IN_CONNECT
    QT_NO_URL_CAST_FROM_STRING
    QT_STRICT_ITERATORS
    QT_USE_QSTRINGBUILDER
    QT_DEBUG_PLUGINS=1
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

if(FLATPAK)
    install(TARGETS markdownedit ${INSTALL_TARGETS_DEFAULT_ARGS})

    # configure_file(packaging/MarkdownEdit/usr/share/applications/MarkdownEdit.desktop org.flatpak.markdownedit.desktop COPYONLY)
    install(PROGRAMS packaging/flatpak/MarkdownEdit.desktop DESTINATION ${KDE_INSTALL_APPDIR})
    install(FILES packaging/flatpak/io.github.software-made-easy.MarkdownEdit.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

    ecm_install_icons(
        ICONS
            ressources/16-apps-Icon.png
            ressources/24-apps-Icon.png
            ressources/32-apps-Icon.png
            ressources/48-apps-Icon.png
            ressources/64-apps-Icon.png
            ressources/128-apps-Icon.png
            ressources/256-apps-Icon.png
        DESTINATION ${KDE_INSTALL_ICONDIR}
        THEME hicolor
    )
elseif(LINUX AND NOT WIN32)
    include(GNUInstallDirs)
    install(TARGETS markdownedit DESTINATION ${CMAKE_INSTALL_BINDIR})
    if(EXISTS "/usr/share/applications/")
        install(FILES packaging/MarkdownEdit/usr/share/applications/MarkdownEdit.desktop
                DESTINATION "/usr/share/applications/")
    endif()
endif()
