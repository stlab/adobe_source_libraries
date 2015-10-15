#!/bin/bash

BUILDDIR=${BUILDDIR:-build}
TOOLSET=${TOOLSET:-clang++}
BUILDMODE=${BUILDMODE:-RELEASE}

if [ "$TOOLSET" == "xcode" ]; then
    mkdir -p ../${BUILDDIR}/xcode/${BUILDMODE}
    pushd ../${BUILDDIR}/xcode/${BUILDMODE}
    cmake -DCMAKE_CXX_COMPILER=${TOOLSET} -DCMAKE_BUILD_TYPE=${BUILDMODE} -DCMAKE_INSTALL_PREFIX=stage -G "Xcode" ../../../adobe_source_libraries
    #make -j4
    #make test
    popd
else
    mkdir -p ../${BUILDDIR}/${TOOLSET}/${BUILDMODE}
    pushd ../${BUILDDIR}/${TOOLSET}/${BUILDMODE}
    cmake -DCMAKE_CXX_COMPILER=${TOOLSET} -DCMAKE_BUILD_TYPE=${BUILDMODE} -DCMAKE_INSTALL_PREFIX=stage -G "Unix Makefiles" ../../../adobe_source_libraries
    make -j4
    make test
    popd
fi

