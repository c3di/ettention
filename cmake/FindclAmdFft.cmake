#
# Try to find the FreeImage library and include path.
# Once done this will define
#
# CLAMDFFT_FOUND
# FREEIMAGE_INCLUDE_DIR
# FREEIMAGE_LIBRARIES_RELEASE
# FREEIMAGE_LIBRARIES_DEBUG
# 

include(FindPackageHandleStandardArgs)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(POSSIBLE_LIB_SUFFIXES Win64 x64 x86_64 lib/Win64 lib/x86_64 lib/x64 lib64)
else(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(POSSIBLE_LIB_SUFFIXES Win32 x86 lib/Win32 lib/x86 lib lib32)
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

IF (WIN32)

	message ("Use ETtention dependencies package for clFFT")

ELSE (WIN32)

	IF(CLAMDFFT_INCLUDE_DIR)
      # Already in cache, be silent
      set (CLFFT_FIND_QUIETLY TRUE)
    ENDIF ()

    FIND_PATH(CLFFT_ROOT_DIR
        NAMES include/clFFT.h
        HINTS /usr/local/ ${CLFFT_ROOT} ENV CLFFT_ROOT
        DOC "clFFT root directory.")

    FIND_PATH(_CLFFT_INCLUDE_DIR
        NAMES clFFT.h
        HINTS ${CLFFT_ROOT_DIR}/include
        DOC "clFFT Include directory")

    FIND_LIBRARY(_CLFFT_LIBRARY
        NAMES clFFT
        HINTS ${CLFFT_ROOT_DIR}/lib ${CLFFT_ROOT_DIR}/lib64) 

    SET(CLAMDFFT_INCLUDE_DIR ${_CLFFT_INCLUDE_DIR})
    SET(CLAMDFFT_LIBRARIES ${_CLFFT_LIBRARY})

    FIND_PACKAGE_HANDLE_STANDARD_ARGS(CLFFT DEFAULT_MSG CLAMDFFT_LIBRARIES CLAMDFFT_INCLUDE_DIR)

ENDIF (WIN32)

