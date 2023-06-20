#!/bin/bash
set -e

BUILDDIR=${1:-../build_asl}

#
# CLANG
#

# local BOOST
BUILDDIR=${BUILDDIR} TOOLSET=clang++ BUILDMODE=RELEASE sh cmake_build.sh
BUILDDIR=${BUILDDIR} TOOLSET=clang++ BUILDMODE=DEBUG   sh cmake_build.sh

# system BOOST
BUILDDIR=${BUILDDIR} TOOLSET=clang++ BUILDMODE=RELEASE sh cmake_build.sh
BUILDDIR=${BUILDDIR} TOOLSET=clang++ BUILDMODE=DEBUG   sh cmake_build.sh

#
# GCC
#

# local BOOST
BUILDDIR=${BUILDDIR} TOOLSET=g++ BUILDMODE=RELEASE sh cmake_build.sh
BUILDDIR=${BUILDDIR} TOOLSET=g++ BUILDMODE=DEBUG   sh cmake_build.sh

# system BOOST
BUILDDIR=${BUILDDIR} TOOLSET=g++ BUILDMODE=RELEASE sh cmake_build.sh
BUILDDIR=${BUILDDIR} TOOLSET=g++ BUILDMODE=DEBUG   sh cmake_build.sh
