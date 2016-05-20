cmake_minimum_required(VERSION 2.8)
project(CompileKernel)

SetPolicies()

set(${PROJECT_NAME}_SOURCES "${PROJECT_SOURCE_DIR}/CompileKernel.cpp")

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

# hack for msvc to place executable directly into ./bin and not ./bin/Debug etc.
foreach(CONFIG IN LISTS CMAKE_CONFIGURATION_TYPES ITEMS ${CMAKE_BUILD_TYPE})
    string(TOUPPER ${CONFIG} CONFIG_UPPER)
    if(NOT CONFIG_UPPER MATCHES "RELEASENOOUTFILES")
		set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${EXECUTABLE_OUTPUT_PATH})
    endif()
endforeach()

if(WIN32)
	set(${PROJECT_NAME}_EXECUTABLE "${EXECUTABLE_OUTPUT_PATH}/CompileKernel.exe")
else(WIN32)
	set(${PROJECT_NAME}_EXECUTABLE "${EXECUTABLE_OUTPUT_PATH}/CompileKernel")
endif(WIN32)

# set path to compileKernel tool as ${${PROJECT_NAME}_EXECUTABLE} for later calling in custom build steps
set(COMPILE_KERNEL_EXECUTABLE ${${PROJECT_NAME}_EXECUTABLE} PARENT_SCOPE)
