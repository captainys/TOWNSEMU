set(TARGET_NAME yssimplesound_test)
add_executable(${TARGET_NAME} 
	test.cpp
	wavedata.cpp
	prop9.cpp
	gun.cpp
	wavedata.h
	prop9.h
	gun.h
)
target_link_libraries(${TARGET_NAME} yssimplesound fssimplewindow ysbitmapfont)

if(MSVC)
	target_link_libraries(${TARGET_NAME} yssimplesound_dsound)
elseif(APPLE)
	target_link_libraries(${TARGET_NAME} yssimplesound_macosx)
elseif(UNIX)
	target_link_libraries(${TARGET_NAME} yssimplesound_linux_alsa)
endif()

list(APPEND YS_ALL_BATCH_TEST yssimplesound_test)
set(YS_ALL_BATCH_TEST ${YS_ALL_BATCH_TEST} PARENT_SCOPE)
