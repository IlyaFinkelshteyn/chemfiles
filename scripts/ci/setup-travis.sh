#!/bin/bash

export CMAKE_ARGS="-DCMAKE_BUILD_TYPE=debug -DCHFL_BUILD_TESTS=ON -DBUILD_SHARED_LIBS=$SHARED_LIBS"

if [[ "$TRAVIS_OS_NAME" == "linux" && "$CC" == "gcc" && "$SHARED_LIBS" == "ON" ]]; then
    export DO_COVERAGE_ON_TRAVIS=true
    export CMAKE_ARGS="$CMAKE_ARGS -DCHFL_CODE_COVERAGE=ON"
    pip install --user codecov
else
    export DO_COVERAGE_ON_TRAVIS=false
fi

cd $TRAVIS_BUILD_DIR
pip install --user -r doc/requirements.txt

if [[ "$EMSCRIPTEN" == "ON" ]]; then
    wget https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz
    tar xf emsdk-portable.tar.gz
    cd emsdk-portable
    ./emsdk update
    ./emsdk install latest
    ./emsdk activate latest
    source ./emsdk_env.sh

    export CMAKE_CONFIGURE='emcmake'
    export CMAKE_ARGS="$CMAKE_ARGS -DCHFL_TEST_RUNNER=node"
    return
fi

if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
    if [[ "$CC" == "gcc" ]]; then
        export CC=gcc-4.8
        export CXX=g++-4.8
    fi

    if [[ "$ARCH" != "x86" ]]; then
        export CMAKE_ARGS="$CMAKE_ARGS -DCHFL_TEST_RUNNER=valgrind"
    fi
fi


if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    brew update
    brew install doxygen
    if [[ "$CC" == "gcc" ]]; then
        brew rm gcc
        brew install gcc@5
        export CC=gcc-5
        export CXX=g++-5
    fi
fi


if [[ "$ARCH" == "x86" ]]; then
    export CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_C_FLAGS=-m32"
fi
