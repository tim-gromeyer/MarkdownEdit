option(STATIC_BUILD "Build librarys static instead of linking" OFF)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Svg)

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
target_link_libraries(qmarkdowntextedit PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Svg)

include_directories(${CMAKE_CURRENT_LIST_DIR}/qmarkdowntextedit)
if(NOT MD4C_FOUND)
    include_directories(${CMAKE_CURRENT_LIST_DIR}/md4c/src)
endif()

set(3rdparty
    md4c-html
    qmarkdowntextedit
)
