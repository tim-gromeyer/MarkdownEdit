if [[ $0 == "*build.sh" ]]; then
    cd ..
fi

mkdir 3rdparty
git clone https://github.com/mity/md4c 3rdparty/md4c
git clone https://github.com/Waqar144/QSourceHighlite
git clone https://github.com/pbek/qmarkdowntextedit
git clone https://github.com/software-made-easy/QtSpell qtspell

cd ..
mkdir build && cd build
qmake ..
make
if [[ $1 == "install" ]]; then
    sudo make install
fi

chmod +x MarkdownEdit

