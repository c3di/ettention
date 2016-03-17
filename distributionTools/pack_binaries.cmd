@ECHO OFF

ECHO Creating binary package for Web

SETLOCAL
SET RUN7Z=%CD%\7zip\7z.exe

CALL compile.cmd

ECHO Packing binaries

PUSHD ..\

IF EXIST package RMDIR /S /Q package
MKDIR package

ECHO Packing Ettention...
xcopy /e /y /q build\bin\Release\*.dll package > NUL
xcopy /e /y /q build\bin\Release\*.exe package > NUL
DEL package\*Test.exe
DEL package\MemoryManager.dll

IF EXIST package\Etomo.7z DEL package\Etomo.7z
IF EXIST .\etomo\jar\ %RUN7Z% a package\Etomo.7z .\etomo\jar\*
REM %RUN7Z% a package\Etomo.7z .\etomo\jar\* -x!plugins*

DEL Ettention.7z
%RUN7Z% a Ettention.7z .\package\*

ECHO Deleting temporary directory...
RMDIR /S /Q package

POPD

SET RUN7Z=
ENDLOCAL

Echo ...done.