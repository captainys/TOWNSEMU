set(TARGET_NAME filesys)

if(MSVC)
	set(SRCS filesys_win.cpp)
elseif(APPLE)
	set(SRCS filesys_unix.cpp)
elseif(UNIX)
	set(SRCS filesys_unix.cpp)
else()
	set(SRCS filesys_null.cpp)
endif()

add_library(${TARGET_NAME} filesys.h ${SRCS})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})



add_executable(filesys_test main.cpp)
target_link_libraries(filesys_test filesys)

if(MSVC)
	add_test(
	 	NAME FILESYS_TEST COMMAND filesys_test "${CMAKE_CURRENT_SOURCE_DIR}" "."
	)
elseif(APPLE)
	add_test(
	 	NAME FILESYS_TEST COMMAND filesys_test "${CMAKE_CURRENT_SOURCE_DIR}" "."
	)
elseif(UNIX)
	add_test(
	 	NAME FILESYS_TEST COMMAND filesys_test "${CMAKE_CURRENT_SOURCE_DIR}" "."
	)
else()
endif()
