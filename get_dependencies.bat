REM @echo off

set /p ETTENTION_DEPENDENCIES_VERSION=<dependencies_version.txt
set ETTENTION_DEPENDENCIES_FILE=..\ettention_dependencies_%ETTENTION_DEPENDENCIES_VERSION%.7z
set ETTENTION_DEPENDENCIES_URL="https://ettention.cg.uni-saarland.de/wp-content/uploads/dependencies/ettention_dependencies_%ETTENTION_DEPENDENCIES_VERSION%.7z"
set ETTENTION_DEPENDENCIES_FILE_ABSOLUTE=%cd%\..\ettention_dependencies_%ETTENTION_DEPENDENCIES_VERSION%.7z

IF NOT EXIST %ETTENTION_DEPENDENCIES_FILE_ABSOLUTE% (
	ECHO Downloading %ETTENTION_DEPENDENCIES_FILE_ABSOLUTE% from server...
	buildTools\wget.exe --no-check-certificate %ETTENTION_DEPENDENCIES_URL% -O %ETTENTION_DEPENDENCIES_FILE_ABSOLUTE%
	ECHO deleting old dependencies folder
	RMDIR /s /q ..\ettention_dependencies 
) ELSE (
	ECHO Dependency %ETTENTION_DEPENDENCIES_FILE_ABSOLUTE% already exists, download skipped
)

IF NOT EXIST ..\ettention_dependencies (
	ECHO Extracting dependencies...
	buildTools\7zip\7z.exe x -o.. %ETTENTION_DEPENDENCIES_FILE_ABSOLUTE%
)

IF NOT DEFINED ETTENTION_PLATFORM (
	ECHO No platform specified.
	GOTO:eof
)

SET IS_64=false
IF %ETTENTION_PLATFORM%==msvc12_x64 SET IS_64=true
IF %ETTENTION_PLATFORM%==msvc10_x64 SET IS_64=true
IF %IS_64%==true (
	IF EXIST ..\ettention_dependencies\boost\stage_x64 (
		IF EXIST ..\ettention_dependencies\boost\stage REN ..\ettention_dependencies\boost\stage stage_x86
		REN ..\ettention_dependencies\boost\stage_x64 stage
	)
) ELSE (
	IF EXIST ..\ettention_dependencies\boost\stage_x86 (
		IF EXIST ..\ettention_dependencies\boost\stage REN ..\ettention_dependencies\boost\stage stage_x64
		REN ..\ettention_dependencies\boost\stage_x86 stage
	)
)
SET IS_64=

SET QT_CURRENT_VERSION=none
IF EXIST ..\ettention_dependencies\qt SET /p QT_CURRENT_VERSION=<..\ettention_dependencies\qt\version.txt

IF NOT %QT_CURRENT_VERSION%==%ETTENTION_PLATFORM% (
	IF NOT %QT_CURRENT_VERSION%==none MOVE ..\ettention_dependencies\qt ..\ettention_dependencies\qt_raw\%QT_CURRENT_VERSION%
	MOVE ..\ettention_dependencies\qt_raw\%ETTENTION_PLATFORM% ..\ettention_dependencies\qt
)
SET QT_CURRENT_VERSION=