set(TARGET_NAME osinit)

if(MSVC)
	set(SRCS osinit_win.cpp)
elseif(APPLE)
	set(SRCS osinit_unix.cpp)
elseif(UNIX)
	set(SRCS osinit_unix.cpp)
else()
	set(SRCS osinit_null.cpp)
endif()


add_library(${TARGET_NAME} osinit.h ${SRCS})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
