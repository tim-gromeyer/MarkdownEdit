# Find Qt for QmarkdownTextEdit and SingleApplication
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(MD4C_FILES
    ${CMAKE_CURRENT_LIST_DIR}/md4c/src/entity.h
    ${CMAKE_CURRENT_LIST_DIR}/md4c/src/entity.c
    ${CMAKE_CURRENT_LIST_DIR}/md4c/src/md4c.h
    ${CMAKE_CURRENT_LIST_DIR}/md4c/src/md4c.c
    ${CMAKE_CURRENT_LIST_DIR}/md4c/src/md4c-html.h
    ${CMAKE_CURRENT_LIST_DIR}/md4c/src/md4c-html.c
)

add_library(md4c-html STATIC ${MD4C_FILES})
target_include_directories(md4c-html PUBLIC ${CMAKE_CURRENT_LIST_DIR}/md4c/src)


set(QMARKDOWNTEXTEDIT_FILES
    ${CMAKE_CURRENT_LIST_DIR}/qmarkdowntextedit/markdownhighlighter.h
    ${CMAKE_CURRENT_LIST_DIR}/qmarkdowntextedit/markdownhighlighter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/qmarkdowntextedit/linenumberarea.h
    ${CMAKE_CURRENT_LIST_DIR}/qmarkdowntextedit/qmarkdowntextedit.h
    ${CMAKE_CURRENT_LIST_DIR}/qmarkdowntextedit/qmarkdowntextedit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/qmarkdowntextedit/qownlanguagedata.h
    ${CMAKE_CURRENT_LIST_DIR}/qmarkdowntextedit/qownlanguagedata.cpp
    ${CMAKE_CURRENT_LIST_DIR}/qmarkdowntextedit/qplaintexteditsearchwidget.h
    ${CMAKE_CURRENT_LIST_DIR}/qmarkdowntextedit/qplaintexteditsearchwidget.ui
    ${CMAKE_CURRENT_LIST_DIR}/qmarkdowntextedit/qplaintexteditsearchwidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/qmarkdowntextedit/media.qrc
)

add_library(qmarkdowntextedit STATIC ${QMARKDOWNTEXTEDIT_FILES})
target_link_libraries(qmarkdowntextedit PUBLIC Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(qmarkdowntextedit PUBLIC ${CMAKE_CURRENT_LIST_DIR}/qmarkdowntextedit)
target_compile_definitions(qmarkdowntextedit PUBLIC
    QT_NO_CAST_FROM_BYTEARRAY
    QT_NO_CAST_TO_ASCII
    QT_NO_FOREACH
    QT_NO_KEYWORDS
    QT_NO_NARROWING_CONVERSIONS_IN_CONNECT
    QT_NO_URL_CAST_FROM_STRING
    QT_STRICT_ITERATORS
    QT_USE_QSTRINGBUILDER
)

if(NOT ANDROID AND NOT EMSCRIPTEN)
    set(QT_DEFAULT_MAJOR_VERSION ${QT_VERSION_MAJOR})
    set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/SingleApplication)
    set(singleapp SingleApplication::SingleApplication)
else()
    add_compile_definitions(NOT_SUPPORTET)
endif()
# include(${CMAKE_CURRENT_LIST_DIR}/hunspell.cmake)

set(3rdparty
    md4c-html
    qmarkdowntextedit
    ${singleapp}
)
