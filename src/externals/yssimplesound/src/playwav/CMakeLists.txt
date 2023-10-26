if(MSVC)
	set(LIBS yssimplesound_dsound)
elseif(APPLE)
	find_library(COCOA_LIB Cocoa)
	mark_as_advanced(COCOA_LIB)
	set(LIBS yssimplesound_macosx ${COCOA_LIB})
elseif(UNIX)
	set(LIBS yssimplesound_linux_alsa pthread)
endif()


set(TARGET_NAME playwav)
add_executable(${TARGET_NAME} playwav.cpp)
target_link_libraries(${TARGET_NAME} fssimplewindow yssimplesound ${LIBS})
