#!/bin/bash

# This script assumes clang-format exists and is discoverable. Also note that
# this script required clang-format from version 3.4 or better of the Clang
# tools.

cd `dirname $0`

find . -name '*.[c|h]pp' \! -name "shavs.cpp" -print | xargs -n 1 clang-format -i -style=file
