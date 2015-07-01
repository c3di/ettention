cmake_minimum_required(VERSION 2.8)
project(compile_kernel)

SetPolicies()

set(${PROJECT_NAME}_SOURCES "${PROJECT_SOURCE_DIR}/compile_kernel.cpp")

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(${PROJECT_NAME} ${compile_kernel_SOURCES})

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

# hack for msvc to place executable directly into ./bin and not ./bin/Debug etc.
foreach(CONFIG IN LISTS CMAKE_CONFIGURATION_TYPES ITEMS ${CMAKE_BUILD_TYPE})
    string(TOUPPER ${CONFIG} CONFIG_UPPER)
    if(NOT CONFIG_UPPER MATCHES "RELEASENOOUTFILES")
		set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${EXECUTABLE_OUTPUT_PATH})
    endif()
endforeach()

# set path to compileKernel tool as ${COMPILE_KERNEL_EXECUTABLE} for later calling in custom build steps
if(WIN32)
	set(COMPILE_KERNEL_EXECUTABLE "${EXECUTABLE_OUTPUT_PATH}/compile_kernel.exe")
else(WIN32)
	set(COMPILE_KERNEL_EXECUTABLE "${EXECUTABLE_OUTPUT_PATH}/compile_kernel")
endif(WIN32)
set(COMPILE_KERNEL_EXECUTABLE ${COMPILE_KERNEL_EXECUTABLE} PARENT_SCOPE)
