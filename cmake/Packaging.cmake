# these are cache variables, so they could be overwritten with -D,
set(CPACK_PACKAGE_NAME ${PROJECT_NAME}
    CACHE STRING "The resulting package name"
)

# which is useful in case of packing only selected components instead of the whole thing
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Simple program for editing Markdown files"
    CACHE STRING "Package description for the package metadata"
)
set(CPACK_PACKAGE_DESCRIPTION "Simple and lightweight program for creating and editing Markdown files.")

set(CPACK_VERBATIM_VARIABLES YES)

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

set (CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/ressources/logo/256-apps-MarkdownEdit.png")

set(CPACK_PACKAGE_CONTACT "sakul8826@gmail.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Tim Gromeyer <${CPACK_PACKAGE_CONTACT}>")

set(CPACK_STRIP_FILES YES)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS YES)

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_SUGGESTS "qt5ct")

set(CPACK_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
)

include(CPack)
