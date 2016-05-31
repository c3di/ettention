@ECHO OFF
SETLOCAL EnableDelayedExpansion

SET COMPILEPATH=..\build
SET BUILDTOOL="C:\Program Files (x86)\MSBuild\12.0\Bin\msbuild.exe"

CALL make.cmd %COMPILEPATH%

ECHO Compiling Ettention...

PUSHD %COMPILEPATH%
	%BUILDTOOL% /p:Configuration=Release;Platform=x64 ALL_BUILD.vcxproj
POPD

SET BUILDTOOL=

REM I'm afraid MSBuild not always returns proper ERRORLEVEL, i.e. when one project of solution fails
SET BUILD_STATUS=%ERRORLEVEL%
IF %BUILD_STATUS%==1 EXIT /b 1 

Echo ...done.