# Adobe Source Libraries

This started out as a fork from Adobe's original ASL Perforce server, but has since been undergoing development. Please contact the project owners if you have any questions or comments.

# Build Status: [![Build status](https://travis-ci.org/stlab/adobe_source_libraries.png?branch=master)](https://travis-ci.org/stlab/adobe_source_libraries)

We use [Travis CI](https://travis-ci.org/stlab/adobe_source_libraries) to validate ASL's build state. A build consists of building the test suite with Boost bjam in both debug and release modes.

# Building
There are multiple ways of building the Adobe Source Libraries.

## `configure && build`
From a UNIX terminal you should be able to execute `./configure.sh` and `./build.sh`, which should download all the necessary dependencies (Boost and double-conversion) and begin building. (This is the method employed by Travis CI for build validation.)

## CMake

CMake support also exists for Unix Makefiles and XCode. CMake will generate separate debug and release projects in a `build` folder (which will be a sibling to the top-level `adobe_source_libraries` folder.)

(The `./configure` script is useful in this instance, too, as it will download and place the Boost and double-conversion libraries where they need to go.)

If you have `BOOST_ROOT` defined, Boost headers and libraries will be included in the build automatically. Otherwise, CMake will generate what is necessary to build Boost dependencies as a static library.

### Makefiles

This option is invoked from the `./cmake_build_all.sh` script. It includes the ability to auto-download the double-conversion library.

### Xcode

This option is invoked from the `./cmake_xcode.sh` script. It assumes the presence of the double-conversion dependency. Once the project file is generated you can open it and build directly from within Xcode.

Documentation
=====
ASL's documentation is built with [Doxygen](http://www.doxygen.org) and [hosted on GitHub](http://stlab.github.io/adobe_source_libraries/). We are in the process of updating the documentation so please pardon the dust.

The legacy documentation is still online and can be found [here](http://stlab.adobe.com/).

Slack
=====
A Slack team has been set up to discuss all things ASL. If you would like an invitation please open an issue with your email address.
