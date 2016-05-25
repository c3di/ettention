cmake_minimum_required(VERSION 2.8)
project(EtomoPlugin)

INCLUDE(../cmake/CommonFlags)
SetPolicies()
SetCommonDefinitions()


# find all directories containing source files
set(FILE_TYPES *.c *.cc *.cpp *.h *.hpp *.tcc)
find_directories(DIRS src "${FILE_TYPES}")
# generate source tree
generate_source_tree(HOST_SOURCES "${DIRS}" "${FILE_TYPES}")

add_subdirectory(test)
add_dependencies(EtomoPluginTest EtomoPlugin)

set(ETOMOPLUGIN_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(ETOMOPLUGIN_INCLUDE_DIR ${ETOMOPLUGIN_INCLUDE_DIR} PARENT_SCOPE)


# set include directories
include_directories("${LIBRECONSTRUCTION_INCLUDE_DIR}"
					"${GTEST_INCLUDE_DIR}"
					"${CMAKE_CURRENT_SOURCE_DIR}/kernel"
					"${ETOMOPLUGIN_INCLUDE_DIR}"
					"${MEMORYMANAGER_INCLUDE_DIR}"
					"${Boost_INCLUDE_DIR}"
					"${OPENCL_INCLUDE_DIR}"
					"${CLAMDFFT_INCLUDE_DIR}"
					)
					
# set library directories
link_directories("${Boost_LIBRARY_DIR}")

add_library(${PROJECT_NAME} SHARED ${HOST_SOURCES} ${OCL_SOURCES} ${OCL_HEADERS})

add_dependencies(EtomoPlugin libReconstruction)

target_link_libraries(${PROJECT_NAME}
					libReconstruction
					${Boost_LIBRARIES}
					)

# precompiled headers
add_precompiled_header(${PROJECT_NAME} stdafx.h)