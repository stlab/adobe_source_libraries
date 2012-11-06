#!/bin/bash

if [ "$1" == "" ]; then
    echo "usage: $0 [ file ]"
    echo "       compares the ASL MD5 hash function with OpenSSL MD5."
    echo "       Note: you must have the md5_asl binaries compiled"
    echo "       before you can use this smoke test."

    exit 1;
fi

OPENSSL_RESULT=`openssl md5 $1`

echo "    OpenSSL: $OPENSSL_RESULT"

for TEST_EXE in "./bindebug/md5_asl" "./bin/md5_asl" ; do

    if [ -x $TEST_EXE ]; then

        TEST_RESULT=`$TEST_EXE $1`
    
        if [ "$OPENSSL_RESULT" == "$TEST_RESULT" ]; then

            echo "##### PASS ##### ($TEST_EXE)"

        else

            echo "##### FAIL ##### ($TEST_EXE)"

        fi

        echo "        ASL: $TEST_RESULT"

    else

        echo "##### NO TEST ##### ($TEST_EXE)"
        echo "    Executable '$TEST_EXE' could not be found and was not tested"

    fi

done
