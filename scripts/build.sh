if [[ $0 == "*build.sh" ]]; then
    cd ..
fi

mkdir 3rdparty && cd 3rdparty
git clone https://github.com/mity/md4c
cd md4c
mkdir build && cd build
cmake ..
make
sudo make install
cd ../..

git clone https://github.com/pbek/qmarkdowntextedit
git clone https://github.com/manisandro/qtspell
cd qtspell
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

