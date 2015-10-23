#!/bin/bash

BUILDDIR=${BUILDDIR:-build}
TOOLSET=${TOOLSET:-clang++}
BUILDMODE=${BUILDMODE:-RELEASE}

if [ "$TOOLSET" == "xcode" ]; then
    mkdir -p ../${BUILDDIR}/xcode/${BUILDMODE}
    pushd ../${BUILDDIR}/xcode/${BUILDMODE}
    cmake -DUSE_SYSTEM_BOOST=${SYSTEM_BOOST:-OFF} -DCMAKE_CXX_COMPILER=${TOOLSET} -DCMAKE_BUILD_TYPE=${BUILDMODE} -DCMAKE_INSTALL_PREFIX=stage -G "Xcode" ../../../adobe_source_libraries
    #make -j4
    popd
else
    mkdir -p ../${BUILDDIR}/${TOOLSET}/${BUILDMODE}
    pushd ../${BUILDDIR}/${TOOLSET}/${BUILDMODE}
    cmake -DUSE_SYSTEM_BOOST=${SYSTEM_BOOST:-ON} -DCMAKE_CXX_COMPILER=${TOOLSET} -DCMAKE_BUILD_TYPE=${BUILDMODE} -DCMAKE_INSTALL_PREFIX=stage -G "Unix Makefiles" ../../../adobe_source_libraries
    make -j4
    # "ctest -C ${BUILDMODE}" handles CONFIGURATIONS option of CMake's add_test
    # "make test" only run tests declared without any CONFIGURATIONS flag
    ctest -C ${BUILDMODE}
    popd
fi


