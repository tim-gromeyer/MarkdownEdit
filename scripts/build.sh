# change directory to the scripts parent directory
$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )/..

# clone dependencies
git submodule update --init --recursive -j 3
git submodule update --remote --merge

mkdir build && cd build # create build directory
qmake ../MarkdownEdit.pro # create Makefile
make # build project

# if first argument is install, run make install
if [[ $1 == "install" ]]; then
    sudo make install
fi


