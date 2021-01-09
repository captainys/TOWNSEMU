set(TARGET_NAME yssimplesound)
add_library(${TARGET_NAME} yssimplesound.cpp yssimplesound.h)

if(MSVC)
	add_subdirectory("windows_dsound")
elseif(APPLE)
	add_subdirectory("macosx")
elseif(UNIX)
	add_subdirectory("linux_alsa")
endif()

add_subdirectory(nownd)

add_subdirectory(unittest)
