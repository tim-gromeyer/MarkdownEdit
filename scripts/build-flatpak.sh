# Set variables for paths and filenames
APP_ID="com.github.tim_gromeyer.MarkdownEdit"
BUILD_DIR="build-flatpak"
EXPORT_DIR="export-flatpak"
EXPORT_FILENAME="MarkdownEdit.flatpak"

# Install KDE Sdk and Platform
printf "\nInstalling deps...\n----------------------------\n\n\n"
flatpak remote-add --user --if-not-exists flathub https://dl.flathub.org/repo/flathub.flatpakrepo
flatpak install --user --noninteractive org.kde.Sdk/x86_64/6.5 org.kde.Platform/x86_64/6.5

# Clean the build directory
printf "\nBuilding...\n----------------------------\n\n\n"
flatpak-builder "$BUILD_DIR" "include/flatpak/$APP_ID.yml" --force-clean

# Export the package
printf "\nExporting...\n----------------------------\n\n\n"
flatpak build-export "$EXPORT_DIR" "$BUILD_DIR"

# Bundle the Flatpak package
printf "\nBundeling...\n----------------------------\n\n\n"
flatpak build-bundle "$EXPORT_DIR" "$EXPORT_FILENAME" "$APP_ID"

