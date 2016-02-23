#!/bin/bash

# Run a command, and echo before doing so. Also checks the exit
# status and quits if there was an error.
echo_run ()
{
    echo "EXEC : $@"
    "$@"
    r=$?
    if test $r -ne 0 ; then
        exit $r
    fi
}

BOOST_MAJOR=1
BOOST_MINOR=60
BOOST_DOT=0
BOOST_DT_VER=${BOOST_MAJOR}.${BOOST_MINOR}.${BOOST_DOT}
BOOST_US_VER=${BOOST_MAJOR}_${BOOST_MINOR}_${BOOST_DOT}
BOOST_DIR=boost_${BOOST_US_VER}
BOOST_TAR=$BOOST_DIR.tar.gz

#
# Make sure we're at the top-level directory when we set up all our siblings.
#

cd ..

#
# fetch the double-conversion library.
#

if [ ! -e 'double-conversion' ]; then
    echo "INFO : double-conversion not found: setting up."

    echo_run git clone --depth=50 --branch=master git://github.com/stlab/double-conversion.git
else
    echo "INFO : double-conversion found: skipping setup."
fi

#
# If need be, download Boost and unzip it, moving it to the appropriate location.
#

if [ ! -e 'boost_libraries' ]; then
    echo "INFO : boost_libraries not found: setting up."

    if [ ! -e $BOOST_TAR ]; then
        echo "INFO : $BOOST_TAR not found: downloading."

        echo_run curl -L "http://sourceforge.net/projects/boost/files/boost/$BOOST_DT_VER/$BOOST_TAR/download" -o $BOOST_TAR;
    else
        echo "INFO : $BOOST_TAR found: skipping download."
    fi

    echo_run rm -rf $BOOST_DIR

    echo_run tar -xf $BOOST_TAR

    echo_run rm -rf boost_libraries

    echo_run mv $BOOST_DIR boost_libraries
else
    echo "INFO : boost_libraries found: skipping setup."
fi

#
# If we have a patch for the version of Boost we are using, patch boost.
#
# The patchfile should be created with something like the following:
#     diff -wur --unidirectional-new-file -xstatus -xadobe_information -xp4config -x'bin.*' -xmacosxx86_64 -x'*.log' -xproject-config.jam -xb2 -xbjam -xjam0 -x'.DS_Store' ./boost_1_54_0/ ./boost_libraries/ > adobe_source_libraries/tools/boost_1_54_0_patches.txt
#

PATCHFILE=boost_${BOOST_US_VER}_patches.txt
FULL_PATCHFILE=adobe_source_libraries/tools/$PATCHFILE

if [ -e "$FULL_PATCHFILE" ] ; then

    if [ -e "boost_libraries/$PATCHFILE" ] ; then
        echo "INFO : Patchfile previously applied. Skipping."
    else
        echo "INFO : Applying $PATCHFILE..."

        # Copying the patchfile into the boost_libraries folder
        # lets us use it as a marker to see if the patch has
        # already been applied to this instance of Boost.
        echo_run cp $FULL_PATCHFILE boost_libraries/

        echo_run cd boost_libraries

        echo_run patch -uN -p2 -g0 -i $PATCHFILE

        echo "Boost patched OK."

        echo_run cd ..
    fi
else
    echo "INFO : No patchfile found for this version of Boost. Skipping."
fi

#
# Create b2/bjam via boostrapping, again if need be.
#

if [ ! -e './boost_libraries/b2' ]; then
    echo "INFO : b2 not found: boostrapping."

    echo_run cd boost_libraries;

    echo_run ./bootstrap.sh --with-toolset=${TOOLSET:-clang}

    echo_run cd ..
else
    echo "INFO : b2 found: skipping boostrap."
fi

#
# A blurb of diagnostics to make sure everything is set up properly.
#

ls -alF

echo "INFO : You are ready to go!"
