cd "${0%/*}"
cd ../../build-MarkdownEdit-Desktop-Release/
sudo cmake --install . --prefix ~/qtprojegt/MarkdownEdit/packaging/MarkdownEdit/usr/
# mkdir -p MarkdownEdit/usr/bin
# cp ../../build-MarkdownEdit-Desktop-Release/markdownedit MarkdownEdit/usr/local/bin/markdownedit
# chmod +x MarkdownEdit/usr/local/bin/markdownedit
cd "${0%/*}"
dpkg-deb --build MarkdownEdit
sudo dpkg -i MarkdownEdit.deb
