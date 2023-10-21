if(MSVC)
	set(LIBS yssimplesound_dsound)
elseif(APPLE)
	find_library(COCOA_LIB Cocoa)
	mark_as_advanced(COCOA_LIB)
	set(LIBS yssimplesound_macosx ${COCOA_LIB})
elseif(UNIX)
	set(LIBS yssimplesound_linux_alsa pthread)
endif()


set(TARGET_NAME yssimplesound_test01)
add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} fssimplewindow yssimplesound ${LIBS})



add_test(NAME yssimplesound_makeFrom44K COMMAND ${TARGET_NAME})
list(APPEND YS_ALL_BATCH_TEST ${TARGET_NAME})


add_executable(yssimplesound_stressTest stress.cpp)
target_link_libraries(yssimplesound_stressTest fssimplewindow yssimplesound ${LIBS})
add_test(NAME yssimplesound_stressTest COMMAND yssimplesound_stressTest)
list(APPEND YS_ALL_BATCH_TEST yssimplesound_stressTest)


set(YS_ALL_BATCH_TEST ${YS_ALL_BATCH_TEST} PARENT_SCOPE)





add_executable(yssimplesound_streamtest streamtest.cpp)
target_link_libraries(yssimplesound_streamtest fssimplewindow yssimplesound)
