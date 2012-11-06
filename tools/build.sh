#!/bin/bash

# Run a command, and echo before doing so. Also checks the exit
# status and quits if there was an error.
echo_run ()
{
    echo "$@"
    "$@"
    r=$?
    if test $r -ne 0 ; then
        exit $r
    fi
}

test_path()
{
    hash $1 1>/dev/null 2>/dev/null
}

WD=`pwd`
TOOL_DIR=`dirname $0`
cd "$TOOL_DIR"

# Assuming the client only needs to run this once, we always build bjam
if [ -e bjam ] ; then
    if test_path cmd ; then
        echo_run rm bjam.exe
    else
        echo_run rm bjam
    fi
fi

echo_run ./install.sh


TEMP_HOME="/tmp/adobe-source"

if [ "$HOME" == "" ] ; then
    HOME=$TEMP_HOME
fi

if [ ! -d "$HOME" ] ; then
    echo_run mkdir -p "$HOME"
fi

MACHINE=`uname`

if [[ ! -e "$HOME/user-config.jam" || "$HOME" == "$TEMP_HOME" ]] ; then
    if [ $MACHINE == "Darwin" ]; then
        echo "import toolset : using ; using darwin ;" > "$HOME/user-config.jam"
    else
        if [ -d "$PROGRAMFILES/Microsoft Visual Studio 9.0/" ] ; then
            echo "using msvc ; import toolset : using ; using gcc ;" > "$HOME/user-config.jam"
        elif [ -d "$PROGRAMFILES/Microsoft Visual Studio 8/" ] ; then
            echo "using msvc ; import toolset : using ; using gcc ;" > "$HOME/user-config.jam"
        else
            echo "import toolset : using ; using gcc ;" > "$HOME/user-config.jam"
        fi
    fi
fi

MODE="debug release"

if [ "$1" != "" ] ; then
    MODE=$1
fi

cd "$WD"

if test_path cmd ; then
    CPU_COUNT=$NUMBER_OF_PROCESSORS
    echo_run "$TOOL_DIR"/bjam --without-python -j$CPU_COUNT msvc $MODE
elif [[ $MACHINE == "Darwin" ]] ; then
    CPU_COUNT=`sysctl -n hw.ncpu`
    echo_run "$TOOL_DIR"/bjam --without-python -j$CPU_COUNT $MODE
else
    echo_run "$TOOL_DIR"/bjam --without-python $MODE
fi

exit 0
