# Set variables for paths and filenames
APP_ID="com.github.tim_gromeyer.MarkdownEdit"
BUILD_DIR="build-flatpak"
EXPORT_DIR="export-flatpak"
EXPORT_FILENAME="MarkdownEdit.flatpak"

# Clean the build directory
printf "\nBuilding...\n----------------------------\n\n\n"
flatpak-builder "$BUILD_DIR" "include/flatpak/$APP_ID.yml" --force-clean

# Export the package
printf "\nExporting...\n----------------------------\n\n\n"
flatpak build-export "$EXPORT_DIR" "$BUILD_DIR"

# Bundle the Flatpak package
printf "\nBundeling...\n----------------------------\n\n\n"
flatpak build-bundle "$EXPORT_DIR" "$EXPORT_FILENAME" "$APP_ID"

