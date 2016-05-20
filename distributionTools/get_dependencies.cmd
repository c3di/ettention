@echo off

ECHO Downloading Ettention dependencies...

set /p ETTENTION_DEPENDENCIES_VERSION=<..\dependencies_version.txt
set ETTENTION_DEPENDENCIES_FILE=..\ettention_dependencies_%ETTENTION_DEPENDENCIES_VERSION%.7z
set ETTENTION_DEPENDENCIES_URL="https://ettention.cg.uni-saarland.de/wp-content/uploads/dependencies/ettention_dependencies_%ETTENTION_DEPENDENCIES_VERSION%.7z"
set ETTENTION_DEPENDENCIES_FILE_ABSOLUTE=%cd%\..\ettention_dependencies_%ETTENTION_DEPENDENCIES_VERSION%.7z
echo %ETTENTION_DEPENDENCIES_FILE_ABSOLUTE%

IF NOT EXIST %ETTENTION_DEPENDENCIES_FILE_ABSOLUTE% (
	ECHO Downloading archive from server to %ETTENTION_DEPENDENCIES_FILE_ABSOLUTE%...
	.\wget.exe --no-check-certificate %ETTENTION_DEPENDENCIES_URL% -O %ETTENTION_DEPENDENCIES_FILE_ABSOLUTE%
	ECHO deleting old dependencies folder
	IF EXIST ..\ettention_dependencies RMDIR /s /q ..\ettention_dependencies 
) ELSE (
	ECHO Dependency %ETTENTION_DEPENDENCIES_FILE_ABSOLUTE% already exists, download skipped
)

IF NOT EXIST ..\ettention_dependencies (
	ECHO Extracting dependencies...
	.\7zip\7z.exe x -o.. %ETTENTION_DEPENDENCIES_FILE_ABSOLUTE% > NUL
) ELSE (
	ECHO Ettention dependencies already unpacked
)


IF EXIST ..\ettention_dependencies\boost\stage_x64 (
	IF EXIST ..\ettention_dependencies\boost\stage REN ..\ettention_dependencies\boost\stage stage_x86
	REN ..\ettention_dependencies\boost\stage_x64 stage
)

IF NOT DEFINED ETTENTION_PLATFORM (
	ECHO No platform specified to explicitly check Qt dependency.
	GOTO:eof
)

ECHO Resolving Qt dependencies...
SET QT_CURRENT_VERSION=none
IF EXIST ..\ettention_dependencies\qt SET /p QT_CURRENT_VERSION=<..\ettention_dependencies\qt\version.txt

IF NOT %QT_CURRENT_VERSION%==%ETTENTION_PLATFORM% (
	IF NOT %QT_CURRENT_VERSION%==none MOVE ..\ettention_dependencies\qt ..\ettention_dependencies\qt_raw\%QT_CURRENT_VERSION%
	MOVE ..\ettention_dependencies\qt_raw\%ETTENTION_PLATFORM% ..\ettention_dependencies\qt
)
SET QT_CURRENT_VERSION=

Echo ...done.