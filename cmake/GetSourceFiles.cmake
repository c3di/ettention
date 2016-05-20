
# macro that adds subpath to file types
macro(add_file_type_subpath RETVAL SUBPATH FILE_TYPES_LIST)
	set(TYPES_WITH_SUBPATH "")
	foreach(FILE_TYPE ${FILE_TYPES_LIST})
		set(TYPES_WITH_SUBPATH ${TYPES_WITH_SUBPATH} ${SUBPATH}/${FILE_TYPE})
	endforeach(FILE_TYPE)
	set(${RETVAL} ${TYPES_WITH_SUBPATH})
endmacro(add_file_type_subpath)

# macro for parsing directories
macro(find_directories RETVAL SUBPATH FILE_TYPES)
	add_file_type_subpath(TYPES_WITH_SUBPATH "${SUBPATH}" "${FILE_TYPES}")
	file(GLOB_RECURSE LIST_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${TYPES_WITH_SUBPATH})

	set(LIST_DIRS "")
	foreach(FILE ${LIST_FILES})
		get_filename_component(PATH_ONLY ${FILE} PATH)
		set(LIST_DIRS ${LIST_DIRS} ${PATH_ONLY})
	endforeach(FILE)
	list(REMOVE_DUPLICATES LIST_DIRS)
	set(${RETVAL} ${LIST_DIRS})
endmacro(find_directories)

# generates source tree groups and finds all files
# msvc also uses this to hierarchically group source files
macro(generate_source_tree SOURCE_FILES DIRECTORIES_TO_PARSE FILE_TYPES)
	set(FILES_FOUND "")
	foreach(DIR ${DIRECTORIES_TO_PARSE})
		add_file_type_subpath(TYPES_WITH_SUBPATH "${DIR}" "${FILE_TYPES}")
		file(GLOB NEW_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${TYPES_WITH_SUBPATH})
		set(FILES_FOUND ${FILES_FOUND} ${NEW_FILES})

		# add to source folder for msvc
		set(SOURCE_FOLDER "${DIR}")
		file(TO_NATIVE_PATH "${SOURCE_FOLDER}" SOURCE_FOLDER)
		# replace \ with \\ to have correct folders in visual studio (linux has / so no change done)
		string(REGEX REPLACE "\\\\" "\\\\\\\\" SOURCE_FOLDER "${SOURCE_FOLDER}")
		source_group("${SOURCE_FOLDER}" FILES ${NEW_FILES})
	endforeach(DIR)
	set(${SOURCE_FILES} ${FILES_FOUND})
endmacro(generate_source_tree)
#####

macro(SUBDIRLIST result curdir)
  message(STATUS "scanning directory ${curdir}")
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
endmacro()