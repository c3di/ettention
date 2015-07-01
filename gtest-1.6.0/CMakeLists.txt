cmake_minimum_required(VERSION 2.8)
project(gtest)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	add_definitions(/D_VARIADIC_MAX=10 /D_LIB)
endif(MSVC)

set(GTEST_SOURCES "src/gtest-all.cc")

# this is for other tools to have access to libReconstruction headers
set(GTEST_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(GTEST_INCLUDE_DIR ${GTEST_INCLUDE_DIR} PARENT_SCOPE)

# set include directories
include_directories("${GTEST_INCLUDE_DIR}"
					"${CMAKE_CURRENT_SOURCE_DIR}"
					)

add_library(${PROJECT_NAME} STATIC ${GTEST_SOURCES})
