if(NOT (ANDROID OR EMSCRIPTEN))
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(MD4C md4c-html)
endif()

if(NOT MD4C_FOUND)
    message(STATUS "No package 'md4c-html' found")

    set(MD4C_FILES
        3rdparty/md4c/src/entity.h
        3rdparty/md4c/src/entity.c
        3rdparty/md4c/src/md4c.h
        3rdparty/md4c/src/md4c.c
        3rdparty/md4c/src/md4c-html.h
        3rdparty/md4c/src/md4c-html.c
    )

    add_library(md4c-html STATIC ${MD4C_FILES})
    set(MD4C_LDFLAGS md4c-html)
endif()

set(QMARKDOWNTEXTEDIT_FILES
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

add_library(qmarkdowntextedit STATIC ${QMARKDOWNTEXTEDIT_FILES})
target_link_libraries(qmarkdowntextedit PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

include_directories(3rdparty/qmarkdowntextedit)
include_directories(3rdparty/md4c/src)

add_library(3rdparty STATIC "")
target_link_libraries(3rdparty PRIVATE ${MD4C_LDFLAGS} qmarkdowntextedit)
