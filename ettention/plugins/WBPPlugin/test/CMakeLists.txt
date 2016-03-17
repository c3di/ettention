cmake_minimum_required(VERSION 2.8)
project(WBPPluginTest)

INCLUDE(../cmake/CommonFlags)
SetPolicies()

if(MSVC)
	add_definitions(/D_VARIADIC_MAX=10  /D_USE_MATH_DEFINES)
endif(MSVC)

# find all directories containing project files
set(FILE_TYPES *.c *.cpp *.h *.hpp)
find_directories(DIRS src "${FILE_TYPES}")
# generate source tree
generate_source_tree(HOST_HEADERS "${DIRS}" "${FILE_TYPES}")

set(RECONSTRUCTION_TEST_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(WBPPLUGIN_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")
set(WBPPLUGIN_INCLUDE_DIR ${WBPPLUGIN_INCLUDE_DIR} PARENT_SCOPE)

# set include directories
 include_directories("${RECONSTRUCTION_TEST_INCLUDE_DIR}"
					"${GTEST_INCLUDE_DIR}"
					"${Boost_INCLUDE_DIR}"
					"${LIBRECONSTRUCTION_INCLUDE_DIR}"
					"${MEMORYMANAGER_INCLUDE_DIR}"
					"${RECONSTRUCTION_CONSOLE_APP_INCLUDE_DIR}"
					"${OPENCL_INCLUDE_DIR}"
					"${CLAMDFFT_INCLUDE_DIR}"
					"${FREEIMAGE_INCLUDE_DIR}"
					"${WBPPLUGIN_INCLUDE_DIR}"
					)

link_directories("${Boost_LIBRARY_DIR}")

add_executable(${PROJECT_NAME}
			${SOURCES_GENERAL}
			${HOST_HEADERS}
			)

target_link_libraries(${PROJECT_NAME}
				WBPPlugin
				${GTEST_BOTH_LIBRARIES}
				)

# copy necessary files to project folder (work and data folders)
if (MSVC)
	add_definitions(/DTESTDATA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")
else(MSVC)
	add_definitions(-DTESTDATA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")
endif(MSVC)
