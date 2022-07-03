# change directory to the scripts parent directory
$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )/..

# clone dependencies
git submodule update --init --recursive -j 3 --depth=1

mkdir build && cd build # create build directory
cmake .. # create Makefile
cmake --build . -j4 # build project



