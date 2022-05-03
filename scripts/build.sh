$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )/..

git submodule update --init --recursive -j 3

mkdir build && cd build
qmake ../MarkdownEdit.pro
make
if [[ $1 == "install" ]]; then
    sudo make install
fi


