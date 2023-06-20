#!/bin/bash

BUILDDIR=${1:-build_asl}

#
# XCODE
#
BUILDDIR=${BUILDDIR} TOOLSET=xcode BUILDMODE=RELEASE sh cmake_build.sh
BUILDDIR=${BUILDDIR} TOOLSET=xcode BUILDMODE=DEBUG   sh cmake_build.sh
