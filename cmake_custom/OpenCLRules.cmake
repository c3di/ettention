# get and create directory for generated kernels (process includes + bin2c)
set(OCL_GENERATED_DIR "${CMAKE_BINARY_DIR}/generated")
get_filename_component(OCL_GENERATED_DIR "${OCL_GENERATED_DIR}" ABSOLUTE)
set(OCL_GENERATED_DIR "${OCL_GENERATED_DIR}" PARENT_SCOPE)
file(MAKE_DIRECTORY "${OCL_GENERATED_DIR}")

if(MSVC)
	add_definitions(/wd4482) # disable "nonstandard extension used" which will disapear in newer compilers anyways
	add_definitions(/wd4275) # disable "non DLL-interface classkey used as base for DLL-interface classkey" which is not an issue unless accessing static data
	add_definitions(/wd4251) # disable "class X needs to have dll-interface to be used by clients of class Y" which is no issue unless using static data
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif(MSVC)

set(OCL_DEPENDENCY_HEADERS "")
foreach(OCL_HEADER ${OCL_HEADERS})
	STRING(REGEX REPLACE "\\\\" "/" OCL_HEADER ${OCL_HEADER}) 
	LIST(APPEND OCL_DEPENDENCY_HEADERS ${OCL_HEADER} )
endforeach(OCL_HEADER)

set( ${OCL_FILES_WITH_INCLUDES} "" )
set( ${TEMPORARY_OCL_FILES} "" )

foreach(OCL_FILE ${OCL_SOURCES})
	get_filename_component(OCL_FILE_ABS ${OCL_FILE} ABSOLUTE)
	get_filename_component(OCL_FILE_NAME ${OCL_FILE} NAME)
	get_filename_component(OCL_FILE_PATH ${OCL_FILE_ABS} DIRECTORY)

	set(OCL_FILE_WITH_INCLUDES "${OCL_GENERATED_DIR}/${OCL_FILE_NAME}")
	get_filename_component(OCL_FILE_NO_EXT "${OCL_FILE_NAME}" NAME_WE)
	set(OCL_FILE_BIN2C "${OCL_GENERATED_DIR}/${OCL_FILE_NO_EXT}_bin2c.h")

	set(COMPILE_KERNEL_PARAMS \"${OCL_FILE_ABS}\" \"${OCL_FILE_WITH_INCLUDES}\" -I \"${OCL_FILE_PATH}\" -I \"${OCL_COMMON_HEADERS_PATH}\")
	set(IOC32_PARAMS -cmd=build -input=\"${OCL_FILE_WITH_INCLUDES}\" -output=\"${OCL_FILE_WITH_INCLUDES}.log\")
	set(BIN2C_PARAMS -o \"${OCL_FILE_BIN2C}\" -n \"${OCL_FILE_NO_EXT}_kernel\" \"${OCL_FILE_WITH_INCLUDES}\")

	add_custom_command(
		OUTPUT "${OCL_FILE_BIN2C}"
		COMMAND "${COMPILE_KERNEL_EXECUTABLE}" ARGS ${COMPILE_KERNEL_PARAMS}
		COMMAND "${BIN2C_EXECUTABLE}" ARGS ${BIN2C_PARAMS}
		WORKING_DIRECTORY ".."
		MAIN_DEPENDENCY "${OCL_FILE}"
		DEPENDS ${OCL_DEPENDENCY_HEADERS}
		COMMENT "checking ${OCL_FILE_ABS} for changes ..."
	)
	
	LIST(APPEND OCL_FILES_WITH_INCLUDES ${OCL_FILE_WITH_INCLUDES} )
	LIST(APPEND TEMPORARY_OCL_FILES ${OCL_FILE_BIN2C} )

endforeach(OCL_FILE)

source_group("OpenCL sources (generated)" FILES ${TEMPORARY_OCL_FILES})
