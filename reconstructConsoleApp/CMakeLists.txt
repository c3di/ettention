cmake_minimum_required(VERSION 2.8)
project(ettention)

INCLUDE(../cmake_custom/CommonFlags)

SetPolicies()

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP -Zm200")
else(MSVC)
	add_definitions(-DBOOST_LOG_DYN_LINK)
endif(MSVC)

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
					"${Boost_INCLUDE_DIRS}"
					"${OPENCL_INCLUDE_DIRS}"
					"${CLAMDFFT_INCLUDE_DIRS}"
					"${LIBRECONSTRUCTION_INCLUDE_DIR}"
					"${MEMORYMANAGER_INCLUDE_DIR}"
					)

link_directories("${Boost_LIBRARY_DIRS}")

add_executable(${PROJECT_NAME} ${HOST_SOURCES})

target_link_libraries(${PROJECT_NAME}
					libReconstruction
					)

if(CMAKE_COMPILER_IS_GNUCC)
	target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} pthread)
endif(CMAKE_COMPILER_IS_GNUCC)

add_precompiled_header(${PROJECT_NAME} stdafx.h)

add_dependencies(${PROJECT_NAME} libReconstruction)
