@ECHO OFF

SET COMPILEPATH=..\build
SET BUILDTOOL="C:\Program Files (x86)\MSBuild\12.0\Bin\msbuild.exe"

CALL make.cmd %COMPILEPATH%

ECHO Compiling Ettention...

PUSHD %COMPILEPATH%
%BUILDTOOL% /p:Configuration=Release;Platform=x64 ALL_BUILD.vcxproj
POPD

SET BUILDTOOL=

Echo ...done.