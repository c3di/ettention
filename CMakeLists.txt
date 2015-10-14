cmake_minimum_required(VERSION 2.8)
project(ettention_cmake)

set(CMAKE_BINARY_DIR ${PROJECT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

set(CMAKE_USE_RELATIVE_PATHS FALSE)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake_custom")
INCLUDE(GetSourceFiles)
INCLUDE(SetPolicies)
INCLUDE(PrecompiledHeaders)
INCLUDE(Cpp11)

SetPolicies()
SwitchOnCpp11()

if(MSVC)
	set(Boost_USE_STATIC_LIBS ON)
endif()

set(Boost_USE_MULTITHREADED ON)

if(WIN32)
    find_package(EttentionDependencies)
else()
    find_package(FreeImage REQUIRED)
    find_package(clAmdFft REQUIRED)
    find_package(GTest REQUIRED)
endif()

find_package(Boost 1.55.0 COMPONENTS filesystem system regex log thread program_options REQUIRED)
find_package(OpenCL REQUIRED)

# add subdirectories and dependencies

# build & debug utilities
add_subdirectory(buildTools/compile_kernel)
add_subdirectory(buildTools/bin2c)
add_subdirectory(memoryManager)

# lib reconstruction
add_subdirectory(libReconstruction)
add_dependencies(libReconstruction compile_kernel)
add_dependencies(libReconstruction bin2c)
add_dependencies(libReconstruction MemoryManager)

# command line tool
add_subdirectory(reconstructConsoleApp)

# automated tests
add_subdirectory(libReconstructionTest)
add_dependencies(libReconstructionUnitTest libReconstruction)
add_dependencies(libReconstructionUnitTest MemoryManager)
add_dependencies(libReconstructionIntegrationTest libReconstruction)
add_dependencies(libReconstructionIntegrationTest MemoryManager)

# plugins
SUBDIRLIST(PLUGINS "${CMAKE_CURRENT_SOURCE_DIR}/plugins")
FOREACH(pluginDir ${PLUGINS})
	message(STATUS "including plugin directory ${pluginDir} in build")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/plugins/${pluginDir}")
ENDFOREACH()

# experiments
SUBDIRLIST(PLUGINS "${CMAKE_CURRENT_SOURCE_DIR}/experiments")
FOREACH(pluginDir ${PLUGINS})
	message(STATUS "including experiments directory ${pluginDir} in build")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/experiments/${pluginDir}")
ENDFOREACH()

# utilities
add_subdirectory(phantomGenerator)
add_subdirectory(projectionGenerator)

# copy necessary files to destination folder
if(MSVC)
	# get destination directory
	get_property(BIN_DEBUG TARGET libReconstruction PROPERTY LOCATION_Debug)
	get_filename_component(BIN_DEBUG ${BIN_DEBUG} PATH)
	get_property(BIN_RELEASE TARGET libReconstruction PROPERTY LOCATION_Release)
	get_filename_component(BIN_RELEASE ${BIN_RELEASE} PATH)

	file(COPY 
		"${FREEIMAGE_DEBUG_DIR}/FreeImaged.dll" 
		"${FREEIMAGE_DEBUG_DIR}/FreeImagePlusd.dll" 
		"${CLAMDFFT_DEBUG_DIR}/clFFT.dll"
		DESTINATION ${BIN_DEBUG})

	file(COPY 
		"${FREEIMAGE_RELEASE_DIR}/FreeImage.dll" 
		"${FREEIMAGE_RELEASE_DIR}/FreeImagePlus.dll" 
		"${CLAMDFFT_RELEASE_DIR}/clFFT.dll"
		DESTINATION ${BIN_RELEASE})

	file(COPY 
		"buildTools/ioc32.exe" 
		"buildTools/ioc64.exe"
		DESTINATION ${EXECUTABLE_OUTPUT_PATH})

endif(MSVC)
