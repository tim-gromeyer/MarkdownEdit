#!/bin/bash

CLEAN_BUILD=false

# Check for command line arguments
while [[ $# -gt 0 ]]; do
    key="$1"

    case $key in
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        *)
            echo "Unknown option: $key"
            exit 1
            ;;
    esac
done

# Clean build directory if specified
if [ "$CLEAN_BUILD" = true ]; then
    if [ -d "build" ]; then
        echo "Removing previous build directory..."
        rm -r build
    fi
fi

# Create and navigate to the build directory
mkdir -p build && cd build

# Configure the project with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -G "Ninja Multi-Config"

# Build the project using all available CPU cores
cmake --build . --config Release

# Package the project
cmake --build . --config Release --target package

# Build a rpm package if the rpm command is available
if command -v rpm &> /dev/null; then
    echo "Building RPM package..."
    cpack -G RPM
fi

# Navigate back to the original directory
cd ..
