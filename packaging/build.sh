cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd
mkdir -p MarkdownEdit/usr/bin
cp ../../build-MarkdownEdit-Desktop-Release/markdownedit MarkdownEdit/usr/bin/markdownedit
chmod +x MarkdownEdit/usr/bin/markdownedit
dpkg-deb --build MarkdownEdit
sudo dpkg -i MarkdownEdit.deb
