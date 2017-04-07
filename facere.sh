#!/bin/bash

case "$1" in
    "cmake")
        if [ ! -d "build" ]; then
            mkdir build
        fi
        echo "Generating Makefile with CMake.."
        cd build/
        cmake -DCMAKE_INSTALL_PREFIX=$(pwd)/.. ..
        ;;
	"cmake_debug")
        if [ ! -d "build" ]; then
            mkdir build
		fi
        echo "Generating Debug Makefile with CMake.."
        cd build/
        cmake -DCMAKE_BUILD_TYPE=Debug ..
		;;
    "make")
        echo "Building with GNU Make..."
        make -C build/
        ;;
    "install")
        echo "Installing to bin..."
        if [ ! -d "bin" ]; then
            mkdir bin
        fi
        mkdir bin
        make -C build/ install
        ;;
    "clean")
        echo "Removing build & bin folders..."
        if [ -d "bin" ]; then
            rm -rf bin/
        fi
        if [ -d "build" ]; then
            rm -rf build/
        fi
        ;;
    "help")
        echo "cmake - generate Makefile using CMake"
        echo "make - build with GNU Make"
        echo "install - install to bin"
        echo "clean - remove build and bin folders"
        echo "help see this message"
        ;;
    *) echo "Unknown command, try with --help."
        ;;
esac
