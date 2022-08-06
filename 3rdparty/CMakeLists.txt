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
target_link_libraries(qmarkdowntextedit PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(NOT ANDROID AND NOT EMSCRIPTEN)
    set(QT_DEFAULT_MAJOR_VERSION ${QT_VERSION_MAJOR})
    set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/SingleApplication)
    set(singleapp SingleApplication::SingleApplication)
else()
    set(singleapp "")
    add_compile_definitions(NOT_SUPPORTET)
endif()

# include(${CMAKE_CURRENT_LIST_DIR}/hunspell.cmake)

include_directories(${CMAKE_CURRENT_LIST_DIR}/qmarkdowntextedit)
include_directories(${CMAKE_CURRENT_LIST_DIR}/md4c/src)


set(3rdparty
    md4c-html
    qmarkdowntextedit
    # hunspell
    ${singleapp}
)
