function(SetCommonDefinitions)

	if(MSVC)
		add_definitions("/DPLUGIN_API=__declspec(dllexport)")
		add_definitions(/DTESTDATA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")

		add_definitions(/DNOMINMAX /D_CRT_SECURE_NO_WARNINGS /D_VARIADIC_MAX=10 /D_USE_MATH_DEFINES /Zm200)
	else(MSVC)
		add_definitions(-DPLUGIN_API=)
		add_definitions(-DTESTDATA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")

		add_definitions(-fPIC)
		add_definitions(-DBOOST_LOG_DYN_LINK)
	endif(MSVC)

endfunction()