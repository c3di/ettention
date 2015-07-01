# Module for locating Ettention Dependencies.
#
# Customizable variables:
#   ETTENTION_DEPENDENCIES_ROOT
#
# Read-only variables:
#   CLAMDFFT_FOUND
#     Indicates library clAmdFft has been found
#   CLAMDFFT_INCLUDE_DIRS
#     Include directory for clAmdFft library
#   CLAMDFFT_LIBRARIES
#     Libraries that should be passed to target_link_libraries
#

include(FindPackageHandleStandardArgs)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(POSSIBLE_LIB_SUFFIXES Win64 x64 x86_64 lib/Win64 lib/x86_64 lib/x64)
else(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(POSSIBLE_LIB_SUFFIXES Win32 x86 lib/Win32 lib/x86)
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

find_path(ETTENTION_DEPENDENCIES_ROOT
  NAMES "clAmdFft/clAmdFft.h"
  PATHS "../ettention_dependencies" "../../ettention_dependencies" "../../../ettention_dependencies"
  DOC "Ettention dependencies root directory")

################
# find clAmdFft
################

find_path(CLAMDFFT_ROOT_DIR
  NAMES clAmdFft.h
  PATHS ${ETTENTION_DEPENDENCIES_ROOT}
  PATH_SUFFIXES clAmdFft
  DOC "clAmdFft root directory")

# message("clamdfft root: ${CLAMDFFT_ROOT_DIR}")

find_path(CLAMDFFT_INCLUDE_DIR
  NAMES clAmdFft.h
  HINTS ${CLAMDFFT_ROOT_DIR}
  PATH_SUFFIXES include
  DOC "clAmdFft include directory")

# message("clamdfft include dir: ${CLAMDFFT_INCLUDE_DIR}")

find_library(CLAMDFFT_LIBRARY
  NAMES clFFT
  HINTS ${CLAMDFFT_ROOT_DIR}
  PATH_SUFFIXES ${POSSIBLE_LIB_SUFFIXES})

# get path to libraries
get_filename_component(CLAMDFFT_DEBUG_DIR ${CLAMDFFT_LIBRARY} ABSOLUTE)
get_filename_component(CLAMDFFT_DEBUG_DIR ${CLAMDFFT_DEBUG_DIR} PATH)

get_filename_component(CLAMDFFT_RELEASE_DIR ${CLAMDFFT_LIBRARY} ABSOLUTE)
get_filename_component(CLAMDFFT_RELEASE_DIR ${CLAMDFFT_RELEASE_DIR} PATH)

set(CLAMDFFT_INCLUDE_DIRS ${CLAMDFFT_INCLUDE_DIR} CACHE INTERNAL "clAmdFft Include Directory")
set(CLAMDFFT_LIBRARIES ${CLAMDFFT_LIBRARY} CACHE INTERNAL "clAmdFft Libraries")

mark_as_advanced(CLAMDFFT_ROOT_DIR CLAMDFFT_INCLUDE_DIRS CLAMDFFT_LIBRARIES)

find_package_handle_standard_args(clAmdFft REQUIRED_VARS CLAMDFFT_ROOT_DIR
  CLAMDFFT_INCLUDE_DIRS CLAMDFFT_LIBRARIES)

################
# find FreeImage
################

find_path(FREEIMAGE_ROOT_DIR
  NAMES FreeImage.h
  PATHS ${ETTENTION_DEPENDENCIES_ROOT}
  PATH_SUFFIXES FreeImage
  DOC "FreeImage root directory")

find_path(FREEIMAGE_INCLUDE_DIR
  NAMES FreeImage.h
  HINTS ${FREEIMAGE_ROOT_DIR}
  PATH_SUFFIXES include
  DOC "FreeImage include directory")

find_library(FREEIMAGE_LIBRARY_RELEASE
  NAMES FreeImage
  HINTS ${FREEIMAGE_ROOT_DIR}
  PATH_SUFFIXES ${POSSIBLE_LIB_SUFFIXES})
find_library(FREEIMAGE_PLUS_LIBRARY_RELEASE
  NAMES FreeImagePlus
  HINTS ${FREEIMAGE_ROOT_DIR}
  PATH_SUFFIXES ${POSSIBLE_LIB_SUFFIXES})

find_library(FREEIMAGE_LIBRARY_DEBUG
  NAMES FreeImaged
  HINTS ${FREEIMAGE_ROOT_DIR}
  PATH_SUFFIXES ${POSSIBLE_LIB_SUFFIXES})
find_library(FREEIMAGE_PLUS_LIBRARY_DEBUG
  NAMES FreeImagePlusd
  HINTS ${FREEIMAGE_ROOT_DIR}
  PATH_SUFFIXES ${POSSIBLE_LIB_SUFFIXES})

# get path to libraries
get_filename_component(FREEIMAGE_DEBUG_DIR ${FREEIMAGE_LIBRARY_DEBUG} ABSOLUTE)
get_filename_component(FREEIMAGE_DEBUG_DIR ${FREEIMAGE_DEBUG_DIR} PATH)

get_filename_component(FREEIMAGE_RELEASE_DIR ${FREEIMAGE_LIBRARY_RELEASE} ABSOLUTE)
get_filename_component(FREEIMAGE_RELEASE_DIR ${FREEIMAGE_RELEASE_DIR} PATH)

# set important library variables
set(FREEIMAGE_INCLUDE_DIRS ${FREEIMAGE_INCLUDE_DIR} CACHE INTERNAL "FreeImage Include Directory")
set(FREEIMAGE_LIBRARIES_DEBUG ${FREEIMAGE_LIBRARY_DEBUG} ${FREEIMAGE_PLUS_LIBRARY_DEBUG} CACHE INTERNAL "FreeImage Libraries Debug")
set(FREEIMAGE_LIBRARIES_RELEASE ${FREEIMAGE_LIBRARY_RELEASE} ${FREEIMAGE_PLUS_LIBRARY_RELEASE} CACHE INTERNAL "FreeImage Libraries Release")

mark_as_advanced(FREEIMAGE_ROOT_DIR FREEIMAGE_INCLUDE_DIRS FREEIMAGE_LIBRARIES)

find_package_handle_standard_args(FreeImage REQUIRED_VARS FREEIMAGE_ROOT_DIR
  FREEIMAGE_INCLUDE_DIRS FREEIMAGE_LIBRARIES_DEBUG FREEIMAGE_LIBRARIES_RELEASE)

################
# find Boost
################

find_path(BOOST_ROOT
  NAMES boost/any.hpp
  PATHS ${ETTENTION_DEPENDENCIES_ROOT}
  PATH_SUFFIXES boost_1_55_0
  DOC "Boost root directory")


find_path(BOOST_LIBRARYDIR
  NAMES boostlibs
  PATHS ${BOOST_ROOT}  
  PATH_SUFFIXES ${POSSIBLE_LIB_SUFFIXES}
  DOC "Boost library directory")
  
if(BOOST_ROOT AND BOOST_LIBRARYDIR)
  message(STATUS "Located Boost in ettention dependencies")
 else()
  message(SEND_ERROR "Boost not present in ettention dependencies")
endif()
  
find_package_handle_standard_args(Boost REQUIRED_VARS BOOST_ROOT)

################
# find Ggtest
################

find_path(GTEST_ROOT_DIR	
  NAMES lib/x64/gtest.lib
  PATHS ${ETTENTION_DEPENDENCIES_ROOT}
  PATH_SUFFIXES gtest-1.6.0
  DOC "google test root directory")

find_path(GTEST_INCLUDE_DIR	
  NAMES gtest
  PATHS ${ETTENTION_DEPENDENCIES_ROOT}
  PATH_SUFFIXES gtest-1.6.0/include)

find_library(GTEST_LIBRARY_RELEASE
  NAMES gtest
  HINTS ${GTEST_ROOT_DIR}
  PATH_SUFFIXES ${POSSIBLE_LIB_SUFFIXES})

find_library(GTEST_LIBRARY_DEBUG
  NAMES gtestd
  HINTS ${GTEST_ROOT_DIR}
  PATH_SUFFIXES ${POSSIBLE_LIB_SUFFIXES})  
  
if(GTEST_INCLUDE_DIR)
  message(STATUS "Located google test in ettention dependencies in dir ${GTEST_INCLUDE_DIR}")
else()
  message(SEND_ERROR "google test not present in ettention dependencies")
endif()

set(GTEST_BOTH_LIBRARIES debug ${GTEST_LIBRARY_DEBUG} optimized ${GTEST_LIBRARY_RELEASE})
find_package_handle_standard_args(gtest REQUIRED_VARS GTEST_INCLUDE_DIR GTEST_LIBRARY_DEBUG GTEST_LIBRARY_RELEASE)

  