cmake_minimum_required(VERSION 3.9)

project(MarkdownEdit VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/ui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
        3rdparty/qtspell/src/Checker.cpp
        3rdparty/qtspell/src/QtSpell.hpp
        3rdparty/qtspell/src/Checker_p.hpp
        3rdparty/qtspell/src/TextEditChecker.cpp
        3rdparty/qtspell/src/Codetable.cpp
        3rdparty/qtspell/src/TextEditChecker_p.hpp
        3rdparty/qtspell/src/Codetable.hpp
        3rdparty/qtspell/src/UndoRedoStack.cpp
        3rdparty/qtspell/src/QtSpellExport.hpp
        3rdparty/qtspell/src/UndoRedoStack.hpp
)
set(QTSPELL_TS
        3rdparty/qtspell/locale/QtSpell_af.ts
        3rdparty/qtspell/locale/QtSpell_lv.ts
        3rdparty/qtspell/locale/QtSpell_ak.ts
        3rdparty/qtspell/locale/QtSpell_mn.ts
        3rdparty/qtspell/locale/QtSpell_be.ts
        3rdparty/qtspell/locale/QtSpell_ms.ts
        3rdparty/qtspell/locale/QtSpell_ca.ts
        3rdparty/qtspell/locale/QtSpell_nb.ts
        3rdparty/qtspell/locale/QtSpell_cs.ts
        3rdparty/qtspell/locale/QtSpell_nl.ts
        3rdparty/qtspell/locale/QtSpell_da.ts
        3rdparty/qtspell/locale/QtSpell_nso.ts
        3rdparty/qtspell/locale/QtSpell_de.ts
        3rdparty/qtspell/locale/QtSpell_pl.ts
        3rdparty/qtspell/locale/QtSpell_el.ts
        3rdparty/qtspell/locale/QtSpell_pt_BR.ts
        3rdparty/qtspell/locale/QtSpell_eo.ts
        3rdparty/qtspell/locale/QtSpell_pt.ts
        3rdparty/qtspell/locale/QtSpell_es_ES.ts
        3rdparty/qtspell/locale/QtSpell_rm.ts
        3rdparty/qtspell/locale/QtSpell_es.ts
        3rdparty/qtspell/locale/QtSpell_ro.ts
        3rdparty/qtspell/locale/QtSpell_eu.ts
        3rdparty/qtspell/locale/QtSpell_ru.ts
        3rdparty/qtspell/locale/QtSpell_ff.ts
        3rdparty/qtspell/locale/QtSpell_rw.ts
        3rdparty/qtspell/locale/QtSpell_fi.ts
        3rdparty/qtspell/locale/QtSpell_sk.ts
        3rdparty/qtspell/locale/QtSpell_fo.ts
        3rdparty/qtspell/locale/QtSpell_sl.ts
        3rdparty/qtspell/locale/QtSpell_fr.ts
        3rdparty/qtspell/locale/QtSpell_sq.ts
        3rdparty/qtspell/locale/QtSpell_ga.ts
        3rdparty/qtspell/locale/QtSpell_sr.ts
        3rdparty/qtspell/locale/QtSpell_gl.ts
        3rdparty/qtspell/locale/QtSpell_st.ts
        3rdparty/qtspell/locale/QtSpell_he.ts
        3rdparty/qtspell/locale/QtSpell_sv.ts
        3rdparty/qtspell/locale/QtSpell_hr.ts
        3rdparty/qtspell/locale/QtSpell_th.ts
        3rdparty/qtspell/locale/QtSpell_hu.ts
        3rdparty/qtspell/locale/QtSpell_tr.ts
        3rdparty/qtspell/locale/QtSpell_hy.ts
        3rdparty/qtspell/locale/QtSpell_uk.ts
        3rdparty/qtspell/locale/QtSpell_id.ts
        3rdparty/qtspell/locale/QtSpell_vi.ts
        3rdparty/qtspell/locale/QtSpell_is.ts
        3rdparty/qtspell/locale/QtSpell_zh_CN.ts
        3rdparty/qtspell/locale/QtSpell_it.ts
        3rdparty/qtspell/locale/QtSpell_zh_HK.ts
        3rdparty/qtspell/locale/QtSpell_ja.ts
        3rdparty/qtspell/locale/QtSpell_zh_TW.ts
        3rdparty/qtspell/locale/QtSpell_ky.ts
        3rdparty/qtspell/locale/QtSpell_zu.ts
        3rdparty/qtspell/locale/QtSpell_lg.ts
        3rdparty/qtspell/locale/QtSpell_lt.ts
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
        ui/mainwindow.ui
        ${3RDPARTY_FILES}
        ${RC_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR}/translations ${TS_FILES})
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR}/3rdparty/qtspell/locale ${QTSPELL_TS})

    qt_add_executable(MarkdownEdit
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${QM_FILES}
    )
else()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR}/translations ${TS_FILES})
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR}/3rdparty/qtspell/locale ${QTSPELL_TS})

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
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/qtspell/src)
target_link_libraries(MarkdownEdit PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::PrintSupport ${ENCHANT_LDFLAGS} ${INTL_LDFLAGS})

set_target_properties(MarkdownEdit PROPERTIES COMPILE_DEFINITIONS "APP_VERSION=\"1.0.0\"")

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
