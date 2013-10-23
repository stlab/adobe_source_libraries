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

# Check that a command is in the PATH.
test_path ()
{
    hash $1 1>/dev/null 2>/dev/null
}

HERE=`dirname $0`

cd "$HERE"

BIN_DIR="$PWD"

cd "${BIN_DIR}/../../boost_libraries/tools/jam/src/"

BJAM=$PWD

rm -rf bootstrap.*
rm -rf bin.*

if test_path cmd ; then 
    echo_run cmd /C "${BJAM}/build.bat"
else
    echo_run "${BJAM}/build.sh"
fi

cd `find . -type d -maxdepth 1 | grep bin.`

cp bjam "$BIN_DIR"

cd "$BIN_DIR"

./bjam -v

exit 0
