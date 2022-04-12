if [[ $0 == "*build.sh" ]]; then
    cd ..
fi


git submodule update --init --recursive -j 4

mkdir build && cd build
qmake ..
make
if [[ $1 == "install" ]]; then
    sudo make install
fi


