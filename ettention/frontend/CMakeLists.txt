cmake_minimum_required(VERSION 2.8)
project(Ettention)

INCLUDE(../cmake/CommonFlags)

SetPolicies()
SetCommonDefinitions()

# find all directories containing source files
set(FILE_TYPES *.c *.cc *.cpp *.h *.hpp *.tcc)
find_directories(DIRS src "${FILE_TYPES}")
# generate source tree
generate_source_tree(HOST_SOURCES "${DIRS}" "${FILE_TYPES}")

# this is for other tools to have access to libReconstruction headers
set(RECONSTRUCTION_CONSOLE_APP_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(RECONSTRUCTION_CONSOLE_APP_INCLUDE_DIR ${RECONSTRUCTION_CONSOLE_APP_INCLUDE_DIR} PARENT_SCOPE)
set(RECONSTRUCTION_TEST_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# set include directories
include_directories("${RECONSTRUCTION_TEST_INCLUDE_DIR}"
					"${RECONSTRUCTION_CONSOLE_APP_INCLUDE_DIR}"
					"${Boost_INCLUDE_DIR}"
					"${OPENCL_INCLUDE_DIR}"
					"${CLAMDFFT_INCLUDE_DIR}"
					"${LIBRECONSTRUCTION_INCLUDE_DIR}"
					"${MEMORYMANAGER_INCLUDE_DIR}"
					)

link_directories("${Boost_LIBRARY_DIR}")

add_executable(${PROJECT_NAME} ${HOST_SOURCES})

target_link_libraries(${PROJECT_NAME}
					${Boost_LIBRARIES}
					libReconstruction
					)

if(CMAKE_COMPILER_IS_GNUCC)
	target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} pthread)
endif(CMAKE_COMPILER_IS_GNUCC)

add_precompiled_header(${PROJECT_NAME} stdafx.h)

add_dependencies(${PROJECT_NAME} libReconstruction)
