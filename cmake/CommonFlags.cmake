if(MSVC)
	add_definitions(/wd4482) # disable "nonstandard extension used" which will disapear in newer compilers anyways
	add_definitions(/wd4275) # disable "non – DLL-interface classkey used as base for DLL-interface classkey" which is not an issue unless accessing static data
	add_definitions(/wd4251) # disable "class X needs to have dll-interface to be used by clients of class Y" which is no issue unless using static data
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
endif(MSVC)

