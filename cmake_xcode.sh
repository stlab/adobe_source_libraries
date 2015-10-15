#!/bin/bash

BUILDDIR=${1:-build}

BUILDDIR=${BUILDDIR} TOOLSET=xcode BUILDMODE=release sh cmake_build.sh
BUILDDIR=${BUILDDIR} TOOLSET=xcode BUILDMODE=debug sh cmake_build.sh
