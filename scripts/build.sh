mkdir -p build && cd build # create build directory
cmake .. -DCMAKE_BUILD_TYPE=Release # create Makefile
cmake --build . --config Release -j4 # build project
cmake --build . --config Release --target package # package

