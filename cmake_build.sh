#!/bin/bash
set -e

: "${BUILDDIR:?BUILDDIR path required}"
: "${TOOLSET:?TOOLSET required (xcode or your c++ compiler)}"
: "${USE_SYSTEM_BOOST:?USE_SYSTEM_BOOST required (ON/OFF)}"

SRC_PATH=$(pwd)

mkdir -p "${BUILDDIR}"/"${TOOLSET}"/"${BUILDMODE}"/"${USE_SYSTEM_BOOST}"
pushd "${BUILDDIR}"/"${TOOLSET}"/"${BUILDMODE}"/"${USE_SYSTEM_BOOST}"

if [ "$TOOLSET" == "xcode" ]; then
    cmake -DUSE_SYSTEM_BOOST="${USE_SYSTEM_BOOST}" -DCMAKE_CXX_COMPILER="${TOOLSET}" -DCMAKE_BUILD_TYPE="${BUILDMODE}" -G "Xcode" "${SRC_PATH}"
    #make -j4
else

    GENERATOR="Unix Makefiles"
    BUILDCMD="make -j"

    cmake -DUSE_SYSTEM_BOOST="${USE_SYSTEM_BOOST}" -DCMAKE_CXX_COMPILER="${TOOLSET}" -DCMAKE_BUILD_TYPE="${BUILDMODE}" -G "${GENERATOR}" "${SRC_PATH}"
    ${BUILDCMD}
    # "ctest -C ${BUILDMODE}" handles CONFIGURATIONS option of CMake's add_test
    # which we use this to skip benchmarks in DEBUG builds
    # "make test" only run tests declared without any CONFIGURATIONS flag
    ctest --output-on-failure -C "${BUILDMODE}"
fi

popd

