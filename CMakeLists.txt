cmake_minimum_required(VERSION 2.8)

# if("${CMAKE_VERSION}" VERSION_GREATER 3.2.0)
# 	message( SEND_ERROR "CMake of version newer than 3.2 works weird with Ettention cmakefiles. You may not be able to build solution cause of linking errors. We are working on that issue." )
# endif()

project(ettention_cmake)

set(CMAKE_BINARY_DIR ${PROJECT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_USE_RELATIVE_PATHS FALSE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")


include(GetSourceFiles)
include(SetPolicies)
include(SetCommonDefinitions)
include(PrecompiledHeaders)
include(Cpp11)

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

find_package(Boost 1.55.0 COMPONENTS filesystem system regex log log_setup thread program_options date_time chrono serialization REQUIRED)
find_package(OpenCL REQUIRED)

# add subdirectories and dependencies

# build & debug utilities
add_subdirectory(ettention/buildTools/compileKernel)
add_subdirectory(ettention/buildTools/bin2c)
add_subdirectory(ettention/memoryManager)

# lib reconstruction
add_subdirectory(ettention/libReconstruction)
add_dependencies(libReconstruction CompileKernel)
add_dependencies(libReconstruction bin2c)
add_dependencies(libReconstruction MemoryManager)

# command line tool
add_subdirectory(ettention/frontend)

# automated tests
add_subdirectory(ettention/test)
add_dependencies(libReconstructionUnitTest libReconstruction)
add_dependencies(libReconstructionUnitTest MemoryManager)
add_dependencies(libReconstructionIntegrationTest libReconstruction)
add_dependencies(libReconstructionIntegrationTest MemoryManager)

# plugins
SUBDIRLIST(PLUGINS "${CMAKE_CURRENT_SOURCE_DIR}/ettention/plugins")
FOREACH(pluginDir ${PLUGINS})
	message(STATUS "including plugin directory ${pluginDir} in build")
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ettention/plugins/${pluginDir}")
ENDFOREACH()

# copy necessary files to destination folder
if(MSVC)
	# get destination directory
	get_property(BIN_DEBUG TARGET libReconstruction PROPERTY LOCATION_Debug)
	get_filename_component(BIN_DEBUG ${BIN_DEBUG} PATH)
	file(COPY 
		"${FREEIMAGE_DEBUG_DIR}/FreeImaged.dll" 
		"${FREEIMAGE_DEBUG_DIR}/FreeImagePlusd.dll" 
		"${CLAMDFFT_DEBUG_DIR}/clFFT.dll"
		DESTINATION ${BIN_DEBUG})

	get_property(BIN_RELEASE TARGET libReconstruction PROPERTY LOCATION_Release)
	get_filename_component(BIN_RELEASE ${BIN_RELEASE} PATH)
	file(COPY 
		"${FREEIMAGE_RELEASE_DIR}/FreeImage.dll" 
		"${FREEIMAGE_RELEASE_DIR}/FreeImagePlus.dll" 
		"${CLAMDFFT_RELEASE_DIR}/clFFT.dll"
		DESTINATION ${BIN_RELEASE})

	file(COPY 
		"ettention/buildTools/ioc64.exe"
		DESTINATION ${EXECUTABLE_OUTPUT_PATH})

endif(MSVC)
