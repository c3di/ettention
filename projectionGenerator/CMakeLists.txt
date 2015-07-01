cmake_minimum_required(VERSION 2.8)
project(projectionGenerator)

INCLUDE(../cmake_custom/CommonFlags)

SetPolicies()

if(MSVC)
	add_definitions("/DPLUGIN_API=__declspec(dllexport)")
	add_definitions(/DNOMINMAX /D_CRT_SECURE_NO_WARNINGS /D_USE_MATH_DEFINES -Zm200)
else(MSVC)
	add_definitions(-DPLUGIN_API=)
endif(MSVC)

# find all directories containing source files
set(FILE_TYPES *.c *.cc *.cpp *.h *.hpp *.tcc)
find_directories(DIRS src "${FILE_TYPES}")
# generate source tree
generate_source_tree(HOST_SOURCES "${DIRS}" "${FILE_TYPES}")

# set include directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src"
					"${Boost_INCLUDE_DIRS}"
					"${OPENCL_INCLUDE_DIRS}"
					"${CLAMDFFT_INCLUDE_DIRS}"
					"${FREEIMAGE_INCLUDE_DIRS}"
					"${LIBRECONSTRUCTION_INCLUDE_DIR}"
					"${MEMORYMANAGER_INCLUDE_DIR}"
					"${STEMPLUGIN_INCLUDE_DIR}")

link_directories("${Boost_LIBRARY_DIRS}")

add_executable(${PROJECT_NAME} ${HOST_SOURCES} src/conio_linux.h)

target_link_libraries(${PROJECT_NAME} 
				libReconstruction
				STEMPlugin
				)

add_dependencies(${PROJECT_NAME} libReconstruction)
add_dependencies(${PROJECT_NAME} STEMPlugin)
				
if(CMAKE_COMPILER_IS_GNUCC)
	target_link_libraries(${PROJECT_NAME} pthread ${Boost_LIBRARIES})
endif(CMAKE_COMPILER_IS_GNUCC)
				
add_precompiled_header(${PROJECT_NAME} stdafx.h)