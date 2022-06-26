cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd
cp ../../build-MarkdownEdit-Desktop-Release/markdownedit MarkdownEdit/usr/bin/markdownedit
dpkg-deb --build MarkdownEdit
sudo dpkg -i MarkdownEdit.deb
