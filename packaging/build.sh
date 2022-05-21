cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd
cp ../../build-MarkdownEdit-Desktop-Release/MarkdownEdit MarkdownEdit/usr/bin/markdownedit 
dpkg-deb --build MarkdownEdit
cp ../../build-MarkdownEdit-Desktop-Release/MarkdownEdit installer/packages/com.sme.markdownedit/data/
/home/tim/Qt/Tools/QtInstallerFramework/4.3/bin/binarycreator -c installer/config/config.xml -p installer/packages installer/Installer
sudo dpkg -i MarkdownEdit.deb
