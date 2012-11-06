@ECHO OFF

set WD=%CD%

if EXIST "%ProgramFiles%\Microsoft Visual Studio 9.0\VC\" (
    set BOOST_JAM_TOOLSET_ROOT=%ProgramFiles%\Microsoft Visual Studio 9.0\VC\
    set VCDIR_ROOT=%BOOST_JAM_TOOLSET_ROOT%
) else if EXIST "%ProgramFiles%\Microsoft Visual Studio 8\VC\" (
    set BOOST_JAM_TOOLSET_ROOT=%ProgramFiles%\Microsoft Visual Studio 8\VC\
    set VCDIR_ROOT=%BOOST_JAM_TOOLSET_ROOT%
)
ECHO "hello"

if "_%BOOST_JAM_TOOLSET_ROOT%_" == "__" (
    echo ###
    echo ### Your installation of MSVC is not in a default location.
    echo ### You must set environment variable "BOOST_JAM_TOOLSET_ROOT" before you can continue.
    echo ### You can either do this by typing at the command line:
    echo ### "set BOOST_JAM_TOOLSET_ROOT=C:\Directory\Path\To\VCDIR"
    echo ### or by setting it as an environment variable in your Windows
    echo ### system settings control panel.
    echo ### Don't forget the final backslash!
    echo ###
    EXIT /B 1
)

echo ###
echo ### Found compiler at %BOOST_JAM_TOOLSET_ROOT%
echo ###


rem ### This will set up required MSVC.NET environment variables.

if EXIST "%BOOST_JAM_TOOLSET_ROOT%bin\VCVARS32.BAT" (
    call "%BOOST_JAM_TOOLSET_ROOT%bin\VCVARS32.BAT"
)

rem ### We now go into building the bjam executable straight from source.
rem ### Assuming the client only needs to run this once, we always build bjam

if EXIST "%~dp0\bjam.exe" (
    DEL "%~dp0\bjam.exe"
)

PUSHD "%~dp0\..\..\boost_libraries\tools\jam\src\"

DEL *.obj
DEL *.pdb

CALL build.bat 

@ECHO OFF

rem ### The call to Boost's build.bat turns echo on, so the previous line redisables it.

if ERRORLEVEL 1 (
    echo ### Error : %ERRORLEVEL%
    EXIT /B %ERRORLEVEL%
)

POPD

copy /Y "%~dp0\..\..\boost_libraries\tools\jam\src\bin.ntx86\bjam.exe" /B "%~dp0\bjam.exe" /B

set TEMP_HOME=%~dp0

if "_%HOME%_" == "__" (
    set HOME=%TEMP_HOME%
)

if NOT EXIST "%HOME%" (
    MkDir %HOME%\
)

echo HOME directory found at "%HOME%"...

if NOT EXIST "%HOME%\user-config.jam" (
    echo using msvc ; > %HOME%\user-config.jam
)

if "_%HOME%_" == "_%TEMP_HOME%_" (
    echo using msvc ; > %HOME%\user-config.jam
)

if ERRORLEVEL 1 (
    echo ### Error : %ERRORLEVEL%
    EXIT /B %ERRORLEVEL%
)

PUSHD "%WD%"

"%~dp0\bjam.exe" -v
"%~dp0\bjam.exe" --without-python -j%NUMBER_OF_PROCESSORS% debug 
"%~dp0\bjam.exe" --without_python -j%NUMBER_OF_PROCESSORS% define=_SECURE_SCL=0 release

if ERRORLEVEL 1 (
    echo ### Error : %ERRORLEVEL%
    EXIT /B %ERRORLEVEL%
)

rem ### Restore old working directory

POPD

EXIT /B 0
