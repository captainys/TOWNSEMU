set(TARGET_NAME yssimplesound_test01)
add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} fssimplewindow yssimplesound)

if(MSVC)
	target_link_libraries(${TARGET_NAME} yssimplesound_dsound)
elseif(APPLE)
	find_library(COCOA_LIB Cocoa)
	mark_as_advanced(COCOA_LIB)
	target_link_libraries(${TARGET_NAME} yssimplesound_macosx ${COCOA_LIB})
elseif(UNIX)
	target_link_libraries(${TARGET_NAME} yssimplesound_linux_alsa)
endif()



add_test(NAME yssimplesound_makeFromCDDA COMMAND ${TARGET_NAME})

list(APPEND YS_ALL_BATCH_TEST ${TARGET_NAME})
set(YS_ALL_BATCH_TEST ${YS_ALL_BATCH_TEST} PARENT_SCOPE)
