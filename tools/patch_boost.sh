#!/bin/bash

HERE=`dirname $0`

cd "$HERE/.."

echo "looking in '"`pwd`"'..."

if [ ! -d ../boost_libraries ]; then
    echo "You must have a destination directory (boost) in order to patch!"
    exit 1;
fi

patch -uN -p0 -g0 < "tools/boost_1_44_0_patches.txt" && echo "Patching of Boost succeeded. Happy coding!"

exit 0
