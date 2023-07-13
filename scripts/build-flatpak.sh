flatpak-builder build-flatpak include/flatpak/com.github.tim_gromeyer.MarkdownEdit.yml --force-clean
flatpak build-export export build-flatpak
flatpak build-bundle export MarkdownEdit.flatpak com.github.tim_gromeyer.MarkdownEdit
