set(MD4C_FILES
    3rdparty/md4c/src/entity.h
    3rdparty/md4c/src/entity.c
    3rdparty/md4c/src/md4c.h
    3rdparty/md4c/src/md4c.c
    3rdparty/md4c/src/md4c-html.h
    3rdparty/md4c/src/md4c-html.c
)

add_library(md4c STATIC ${MD4C_FILES})

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
target_link_libraries(3rdparty PRIVATE md4c qmarkdowntextedit)
