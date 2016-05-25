cmake_minimum_required(VERSION 2.8)
project(libReconstruction)

INCLUDE(../cmake/CommonFlags)

SetPolicies()
SwitchOnCpp11()
SetCommonDefinitions()

# find all directories containing source files
set(FILE_TYPES *.c *.cc *.cpp *.h *.hpp *.tcc)
find_directories(DIRS src "${FILE_TYPES}")
# generate source tree
generate_source_tree(HOST_SOURCES "${DIRS}" "${FILE_TYPES}")

# get OpenCL source files and add them to source tree
set(OCL_HEADERS "")
file(GLOB         OCL_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ocl_src/*.cl)
file(GLOB_RECURSE OCL_HEADERS ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR} *.hl)

source_group("OpenCL sources" FILES ${OCL_SOURCES} ${OCL_HEADERS})

set(OCL_COMMON_HEADERS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ocl_src" PARENT_SCOPE)

# create custom build step for OpenCL sources
INCLUDE(../cmake/OpenCLRules)

# this is for other tools to have access to libReconstruction headers
set(LIBRECONSTRUCTION_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIBRECONSTRUCTION_INCLUDE_DIR ${LIBRECONSTRUCTION_INCLUDE_DIR} PARENT_SCOPE)

set(LIBRECONSTRUCTION_OCL_INCLUDE_DIR "${OCL_GENERATED_DIR}")
set(LIBRECONSTRUCTION_OCL_INCLUDE_DIR ${LIBRECONSTRUCTION_INCLUDE_DIR} PARENT_SCOPE)

# set include directories
include_directories("${LIBRECONSTRUCTION_INCLUDE_DIR}"
					"${GTEST_INCLUDE_DIR}"
					"${MEMORYMANAGER_INCLUDE_DIR}"
					"${Boost_INCLUDE_DIR}"
					"${OPENCL_INCLUDE_DIR}"
					"${CLAMDFFT_INCLUDE_DIR}"
					"${FREEIMAGE_INCLUDE_DIR}"
					"${OCL_GENERATED_DIR}"
					)

# set library directories
link_directories("${Boost_LIBRARY_DIR}")

# Lukas: On Gcc, libReconstruction must be a dynamic library.
# I have not found a way how to link correctly the combination of static/dynamic libraries
# that the static libReconstruction and dynamic plugins represent
IF(CMAKE_COMPILER_IS_GNUCXX)
add_library(${PROJECT_NAME} SHARED ${HOST_SOURCES} ${OCL_SOURCES} ${OCL_HEADERS} ${TEMPORARY_OCL_FILES})
ELSE()
add_library(${PROJECT_NAME} STATIC ${HOST_SOURCES} ${OCL_SOURCES} ${OCL_HEADERS} ${TEMPORARY_OCL_FILES})
ENDIF()

target_link_libraries(${PROJECT_NAME}
				"${OPENCL_LIBRARIES}"
				"${CLAMDFFT_LIBRARIES}"
				MemoryManager
				debug
				"${FREEIMAGE_LIBRARIES_DEBUG}"
				optimized
				"${FREEIMAGE_LIBRARIES_RELEASE}"
				${GTEST_BOTH_LIBRARIES}
				)

if(CMAKE_COMPILER_IS_GNUCC)
	target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} pthread)
endif(CMAKE_COMPILER_IS_GNUCC)

# precompiled headers
add_precompiled_header(${PROJECT_NAME} stdafx.h)
