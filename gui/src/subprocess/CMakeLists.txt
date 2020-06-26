set(TARGET_NAME subproc)

if(MSVC)
	set(PLATFORM_SRCS subproc_win32.cpp)
	set(PLATFROM_LIBS user32.lib kernel32.lib)
endif()

add_library(${TARGET_NAME} subproc.h subproc.cpp ${PLATFORM_SRCS})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${TARGET_NAME} ${PLATFROM_LIBS})
