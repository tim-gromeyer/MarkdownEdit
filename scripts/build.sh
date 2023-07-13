# change directory to the scripts parent directory
cd $( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )/..

# clone dependencies
git submodule update --init --recursive -j 3 --depth=1

mkdir -p build && cd build # create build directory
cmake .. -DCMAKE_BUILD_TYPE=Release # create Makefile
cmake --build . --config Release -j4 # build project
cmake --build . --config Release --target package
