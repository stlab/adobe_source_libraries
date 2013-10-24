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

echo_run cd `dirname $0`

if [ ! -d 'gh-pages' ]; then
    echo 'INFO : gh-pages directory not found. Setting up.';

    echo_run rm -rf gh-pages

    echo_run git clone --branch=gh-pages git@github.com:stlab/adobe_source_libraries.git gh-pages
else
    echo 'INFO : gh-pages already set up.';
fi

echo_run doxygen ./documentation/doxyfile
