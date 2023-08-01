include(InstallRequiredSystemLibraries)

set(CPACK_STRIP_FILES ON)
set(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}")
set(CPACK_PACKAGE_CONTACT "Tim Gromeyer")
set(CPACK_PACKAGE_VENDOR ${CPACK_PACKAGE_CONTACT})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_DESCRIPTION "MarkdownEdit is, as the name suggests, a simple and lightweight program for creating and editing Markdown files.")

set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/COPYING)
set(CPACK_RESOURCE_FILE_README ${PROJECT_SOURCE_DIR}/README.md)

# Speed it up!
set(CPACK_THREADS 0) # all

# Variables specific to CPack RPM generator
set(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
set(CPACK_RPM_PACKAGE_URL ${PROJECT_HOMEPAGE_URL})
# set(CPACK_RPM_PACKAGE_REQUIRES "/sbin/chkconfig, /bin/mktemp, /bin/rm, /bin/mv, libstdc++ >= 2.96") # TODO: Find correct packages
# set(CPACK_RPM_PACKAGE_SUGGESTS "doxygen-latex, doxygen-doc, doxygen-gui, graphviz, libclang1")

# Variables specific to CPack DEB generator
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE ${PROJECT_HOMEPAGE_URL})
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS YES)
set(CPACK_DEBIAN_PACKAGE_SUGGESTS "qt5ct")
set(CPACK_DEBIAN_PACKAGE_CONFLICTS "")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT} <${PROJECT_CONTACT}>")

# Variables specific to CPack NSIS generator
set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/ressources/logo/Icon.ico") # Installer icon
set(CPACK_NSIS_MUI_UNIICON "${PROJECT_SOURCE_DIR}/ressources/logo/Icon.ico") # Uninstaller icon
set(CPACK_NSIS_INSTALLED_ICON_NAME "${PROJECT_SOURCE_DIR}/ressources/logo/Icon.ico") # App icon
set(CPACK_NSIS_URL_INFO_ABOUT ${PROJECT_HOMEPAGE_URL})
set(CPACK_NSIS_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_NSIS_MODIFY_PATH OFF)
set(CPACK_NSIS_INSTALLED_ICON_NAME ${CPACK_NSIS_MUI_ICON})
set(CPACK_NSIS_HELP_LINK ${PROJECT_HOMEPAGE_URL})
set(CPACK_NSIS_URL_INFO_ABOUT ${PROJECT_HOMEPAGE_URL})
set(CPACK_NSIS_CONTACT ${PROJECT_CONTACT})
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_CREATE_ICONS_EXTRA
"CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\MarkdownEdit.lnk' '$INSTDIR\\\\bin\\\\markdownedit.exe' '' '$INSTDIR\\\\Icon.ico'")
set(CPACK_NSIS_DELETE_ICONS_EXTRA
    "Delete '$SMPROGRAMS\\\\$START_MENU\\\\MarkdownEdit.lnk'"
)

# Variables specific to CPack DragNDrop generator
set(CPACK_DMG_FORMAT "UDBZ") # UDRO=UDIF-Read-Only, UDZO=zlib, UDBZ=bzip2 -- See hdiutil
set(CPACK_DMG_VOLUME_NAME ${PROJECT_NAME})
set(CPACK_DMG_BACKGROUND_IMAGE "") # TODO: Add icon

if(WIN32)
 set(CPACK_GENERATOR "ZIP;NSIS")

elseif(APPLE)
 set(CPACK_GENERATOR "ZIP;DragNDrop;PackageMaker;Bundle" )
 set(CPACK_SYSTEM_NAME "OSX" )

elseif(UNIX AND NOT EXMSCRIPTEN AND NOT ANDROID)
 # Determine distribution and release
 execute_process(COMMAND lsb_release -si OUTPUT_VARIABLE distribution OUTPUT_STRIP_TRAILING_WHITESPACE)
 execute_process(COMMAND lsb_release -sc OUTPUT_VARIABLE release OUTPUT_STRIP_TRAILING_WHITESPACE)
 execute_process(COMMAND uname -m OUTPUT_VARIABLE CPACK_RPM_PACKAGE_ARCHITECTURE OUTPUT_STRIP_TRAILING_WHITESPACE)

 if(release STREQUAL "n/a")
     execute_process(COMMAND lsb_release -sr OUTPUT_VARIABLE release OUTPUT_STRIP_TRAILING_WHITESPACE)
 endif()

 if(distribution STREQUAL "Debian" OR distribution STREQUAL "Ubuntu" OR distribution STREQUAL "Linuxmint")
 set(CPACK_GENERATOR "DEB")
 execute_process(COMMAND dpkg --print-architecture OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE OUTPUT_STRIP_TRAILING_WHITESPACE)
 set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE})

 elseif(distribution MATCHES "RedHat.*")
 # extract the major version from RedHat full version (e.g. 6.7 --> 6)
 execute_process(COMMAND lsb_release -sr COMMAND sed s/[.].*// OUTPUT_VARIABLE redhat_version_major OUTPUT_STRIP_TRAILING_WHITESPACE)
 set(CPACK_GENERATOR "RPM")
 set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_RPM_PACKAGE_RELEASE}.el${redhat_version_major}.${CPACK_RPM_PACKAGE_ARCHITECTURE})

 elseif(distribution MATCHES "openSUSE*")
 set(CPACK_GENERATOR "RPM")
 message(STATUS "Package name: ${CPACK_PACKAGE_NAME}\nPackage version: ${CPACK_PACKAGE_VERSION}\nRelease: ${release}\nArchitecture: ${CPACK_RPM_PACKAGE_ARCHITECTURE}")
 set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${release}.${CPACK_RPM_PACKAGE_ARCHITECTURE})

 elseif(distribution STREQUAL "Fedora")
 set(CPACK_GENERATOR "RPM")
 set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}.fc${release}.${CPACK_RPM_PACKAGE_ARCHITECTURE})

 elseif(distribution STREQUAL "Scientific")
 set(CPACK_GENERATOR "RPM")
 set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${release}.${CPACK_RPM_PACKAGE_ARCHITECTURE})

 else()
 set(CPACK_GENERATOR "TGZ;STGZ")
 set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${release}.${CPACK_RPM_PACKAGE_ARCHITECTURE})
 endif()

 set(CPACK_GENERATOR "TGZ;${CPACK_GENERATOR}")
endif()

# Store the packages in a separat dir
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/packages")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})

include(CPack)
