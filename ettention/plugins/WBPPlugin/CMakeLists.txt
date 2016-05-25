cmake_minimum_required(VERSION 2.8)
project(WBPPlugin)

INCLUDE(../cmake/CommonFlags)
SetPolicies()
SetCommonDefinitions()

# find all directories containing source files
set(FILE_TYPES *.c *.cc *.cpp *.h *.hpp *.tcc)
find_directories(DIRS src "${FILE_TYPES}")
# generate source tree
generate_source_tree(HOST_SOURCES "${DIRS}" "${FILE_TYPES}")

add_subdirectory(test)
add_dependencies(WBPPluginTest WBPPlugin)

# get OpenCL source files and add them to source tree
set(OCL_HEADERS "")
file(GLOB         OCL_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ocl_src/*.cl)
file(GLOB_RECURSE OCL_HEADERS ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR} *.hl)
source_group("OpenCL sources" FILES ${OCL_SOURCES} ${OCL_HEADERS})

# create custom build step for OpenCL sources
INCLUDE(../cmake/OpenCLRules)

set(WBPPLUGIN_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(WBPPLUGIN_INCLUDE_DIR ${WBPPLUGIN_INCLUDE_DIR} PARENT_SCOPE)

set(WBPPLUGIN_OCL_INCLUDE_DIR "${OCL_GENERATED_DIR}")
set(WBPPLUGIN_OCL_INCLUDE_DIR ${STEMPLUGIN_INCLUDE_DIR} PARENT_SCOPE)

# set include directories
include_directories("${LIBRECONSTRUCTION_INCLUDE_DIR}"
					"${GTEST_INCLUDE_DIR}"
					"${WBPPLUGIN_INCLUDE_DIR}"
					"${MEMORYMANAGER_INCLUDE_DIR}"
					"${Boost_INCLUDE_DIR}"
					"${OPENCL_INCLUDE_DIR}"
					"${CLAMDFFT_INCLUDE_DIR}"
					"${FREEIMAGE_INCLUDE_DIR}"
					"${OCL_GENERATED_DIR}"
					)

# set library directories
link_directories("${Boost_LIBRARY_DIR}")

add_library(${PROJECT_NAME} SHARED ${HOST_SOURCES} ${OCL_SOURCES} ${OCL_HEADERS} ${TEMPORARY_OCL_FILES})

add_dependencies(WBPPlugin CompileKernel bin2c libReconstruction MemoryManager)

target_link_libraries(${PROJECT_NAME}
				libReconstruction
				${OPENCL_LIBRARIES}
				${CLAMDFFT_LIBRARIES}
				${Boost_LIBRARIES}
				)

if(CMAKE_COMPILER_IS_GNUCC)
	target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} pthread)
endif(CMAKE_COMPILER_IS_GNUCC)

# precompiled headers
add_precompiled_header(${PROJECT_NAME} stdafx.h)