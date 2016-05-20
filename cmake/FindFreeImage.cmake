#
# Try to find the FreeImage library and include path.
# Once done this will define
#
# FREEIMAGE_FOUND
# FREEIMAGE_INCLUDE_DIR
# FREEIMAGE_LIBRARIES_RELEASE
# FREEIMAGE_LIBRARIES_DEBUG
# 

include(FindPackageHandleStandardArgs)

IF (WIN32)

	message ("Use ETtention dependencies package for FreeImage")

ELSE (WIN32)
	FIND_PATH( FREEIMAGE_INCLUDE_DIR FreeImage.h
		/usr/include
		/usr/local/include
		/sw/include
		/opt/local/include
		DOC "The directory where FreeImage.h resides")
	FIND_LIBRARY( FREEIMAGE_LIBRARY_RELEASE
		NAMES FreeImage freeimage
		PATHS
		/usr/lib64
		/usr/lib
		/usr/local/lib64
		/usr/local/lib
		/sw/lib
		/opt/local/lib
		DOC "The FreeImage library")

	FIND_LIBRARY( FREEIMAGE_PLUS_LIBRARY_RELEASE
		NAMES FreeImage freeimageplus
		PATHS
		/usr/lib64
		/usr/lib
		/usr/local/lib64
		/usr/local/lib
		/sw/lib
		/opt/local/lib
		DOC "The FreeImage Plus library")
	SET(FREEIMAGE_LIBRARY_DEBUG ${FREEIMAGE_LIBRARY_RELEASE})
	SET(FREEIMAGE_PLUS_LIBRARY_DEBUG ${FREEIMAGE_PLUS_LIBRARY_RELEASE})

	set(FREEIMAGE_INCLUDE_DIR ${FREEIMAGE_INCLUDE_DIR} CACHE INTERNAL "FreeImage Include Directory")
	set(FREEIMAGE_LIBRARIES_DEBUG ${FREEIMAGE_LIBRARY_DEBUG} ${FREEIMAGE_PLUS_LIBRARY_DEBUG} CACHE INTERNAL "FreeImage Libraries Debug")
	set(FREEIMAGE_LIBRARIES_RELEASE ${FREEIMAGE_LIBRARY_RELEASE} ${FREEIMAGE_PLUS_LIBRARY_RELEASE} CACHE INTERNAL "FreeImage Libraries Release")

	mark_as_advanced(FREEIMAGE_INCLUDE_DIR FREEIMAGE_LIBRARIES_RELEASE, FREEIMAGE_LIBRARIES_DEBUG)

	find_package_handle_standard_args(FreeImage REQUIRED_VARS FREEIMAGE_INCLUDE_DIR FREEIMAGE_LIBRARIES_DEBUG FREEIMAGE_LIBRARIES_RELEASE)

ENDIF (WIN32)

