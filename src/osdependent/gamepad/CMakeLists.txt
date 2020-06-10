set(TARGET_NAME ysgamepad)

set(SRCS ysgamepad.c ysgamepad.h)
set(DEPEND_LIB )



if(MSVC)
	list(APPEND SRCS windows/ysgamepad_windows.c)
elseif(APPLE)
	find_library(GAMECONTROLLER_LIB GameController)
	mark_as_advanced(GAMECONTROLLER_LIB)
	list(APPEND DEPEND_LIB ${GAMECONTROLLER_LIB})
	list(APPEND SRCS macos/ysgamepad_macos.c macos/ysgamepad_macos_objc.h macos/ysgamepad_macos_objc.m)
elseif(UNIX)
	list(APPEND SRCS linux/ysgamepad_linux.c)
else()
	list(APPEND SRCS null/ysgamepad_null.c)
endif()



add_library(${TARGET_NAME} ${SRCS})
target_link_libraries(${TARGET_NAME} ${DEPEND_LIB})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(test)
