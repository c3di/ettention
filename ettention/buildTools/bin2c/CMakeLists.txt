cmake_minimum_required(VERSION 2.8)
project(bin2c)

SetPolicies()

set(${PROJECT_NAME}_SOURCES "${PROJECT_SOURCE_DIR}/bin2c.cpp")

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

# hack for MSVC to prevent warnings about fopen and strcpy
if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

add_executable(${PROJECT_NAME} ${bin2c_SOURCES})

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

# hack for msvc to place executable directly into ./bin and not ./bin/Debug etc.
foreach(CONFIG IN LISTS CMAKE_CONFIGURATION_TYPES ITEMS ${CMAKE_BUILD_TYPE})
    STRING(TOUPPER ${CONFIG} CONFIG_UPPER)
    IF(NOT CONFIG_UPPER MATCHES "RELEASENOOUTFILES")
		set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${EXECUTABLE_OUTPUT_PATH})
    ENDIF()
ENDFOREACH() 

# set path to compileKernel tool as ${BIN2C_EXECUTABLE} for later calling in custom build steps
if(WIN32)
	set(BIN2C_EXECUTABLE "${EXECUTABLE_OUTPUT_PATH}/bin2c.exe")
else(WIN32)
	set(BIN2C_EXECUTABLE "${EXECUTABLE_OUTPUT_PATH}/bin2c")
endif(WIN32)
set(BIN2C_EXECUTABLE ${BIN2C_EXECUTABLE} PARENT_SCOPE)
