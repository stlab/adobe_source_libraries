# Adobe Source Libraries

This started out as a fork from Adobe's original ASL Perforce server, but has since been undergoing development. Please contact the project owners if you have any questions or comments.

# Build Status: [![Build and Tests](https://github.com/stlab/adobe_source_libraries/actions/workflows/adobe_source_libraries.yml/badge.svg)](https://github.com/stlab/adobe_source_libraries/actions/workflows/adobe_source_libraries.yml)

We use [Travis CI](https://travis-ci.org/stlab/adobe_source_libraries) to validate ASL's build state. A build consists of building the test suite with Boost bjam in both debug and release modes.

# Building
There are multiple ways of building the Adobe Source Libraries.

## `configure && build`
From a UNIX terminal you should be able to execute `./configure.sh` and `./build.sh`, which should download all the necessary dependencies (Boost and double-conversion) and begin building. (This is the method employed by Travis CI for build validation.)

## CMake

CMake support also exists for Unix Makefiles and XCode. CMake will generate separate debug and release projects in a `build_asl` folder (which will be a sibling to the top-level `adobe_source_libraries` folder.)

CMake will download Boost and double-conversion from git repositories if not found, respectively in ../boost_libraries and ../double-conversion.

(The `./configure` script can also be used to download and place the Boost and double-conversion libraries where they need to go.)

### Makefiles

This option is invoked from the `./cmake_build_all.sh` script.

### Xcode

This option is invoked from the `./cmake_xcode.sh` script. Once the project file is generated you can open it and build directly from within Xcode.

Documentation
=====
ASL's documentation is built with [Doxygen](http://www.doxygen.org) and [hosted on GitHub](http://stlab.github.io/adobe_source_libraries/). We are in the process of updating the documentation so please pardon the dust.

The legacy documentation is still online and can be found [here](http://stlab.adobe.com/).

Slack
=====
A Slack team has been set up to discuss all things ASL. If you would like an invitation, please open an issue with your email address.
