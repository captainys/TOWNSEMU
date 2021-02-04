set(TARGET_NAME yssimplesound)
add_library(${TARGET_NAME} yssimplesound.cpp yssimplesound.h)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
	add_subdirectory("windows_dsound")
elseif(APPLE)
	add_subdirectory("macosx")
elseif(UNIX)
	add_subdirectory("linux_alsa")
endif()

add_subdirectory(nownd)

add_subdirectory(unittest)

set(YS_ALL_BATCH_TEST ${YS_ALL_BATCH_TEST} PARENT_SCOPE)
