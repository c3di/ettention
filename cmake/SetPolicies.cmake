function(SetPolicies)
	cmake_policy(SET CMP0015 NEW) # link_directories() command will interpret relative paths as relative to CMAKE_CURRENT_SOURCE_DIR instead of verbatim
	if(POLICY CMP0026)
		cmake_policy(SET CMP0026 OLD) # allow use of PROPERTY LOCATION 
	endif(POLICY CMP0026)
	if(POLICY CMP0043)
		cmake_policy(SET CMP0043 NEW)
	endif(POLICY CMP0043)
endfunction()