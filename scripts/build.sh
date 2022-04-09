if [[ $0 == "*build.sh" ]]; then
    cd ..
fi

mkdir 3rdparty
git clone https://github.com/mity/md4c 3rdparty/md4c
git clone https://github.com/Waqar144/QSourceHighlite 3rdparty/QSourceHighlite
git clone https://github.com/pbek/qmarkdowntextedit 3rdparty/qmarkdowntextedit
cd 3rdparty/qtspell
mkdir build && cd build
cmake ..
make
sudo make install

cd ../../..
mkdir build && cd build
qmake ..
make
if [[ $1 == "install" ]]; then
    sudo make install
fi

chmod +x MarkdownEdit
