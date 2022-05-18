cp ../../build-MarkdownEdit-Desktop-Release/MarkdownEdit MarkdownEdit/usr/bin/markdownedit 
dpkg-deb --build MarkdownEdit
/home/tim/Qt/Tools/QtInstallerFramework/4.3/bin/binarycreator -c installer/config/config.xml -p installer/packages installer/Installer
sudo dpkg -i MarkdownEdit.deb
