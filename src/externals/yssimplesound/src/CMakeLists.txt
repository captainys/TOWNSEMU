# I am sick and tired of ld.  ld should learn two-pass link or be burnt in the hellfire.



if(MSVC)
	set(DUMMY_LIBRARY_NAME yssimplesound_dsound)
	set(SUBDIR windows_dsound)
	set(SRCS ${SUBDIR}/yssimplesound_dsound.cpp)
	set(LIBS "")
elseif(APPLE)
	find_library(AVFOUNDATION_LIB AVFoundation)
	mark_as_advanced(AVFOUNDATION_LIB)

	set(DUMMY_LIBRARY_NAME yssimplesound_macosx)
	set(SUBDIR macosx)
	set(SRCS ${SUBDIR}/yssimplesound_macosx_cpp.cpp ${SUBDIR}/yssimplesound_macosx_objc.m)
	set(LIBS ${AVFOUNDATION_LIB})
elseif(UNIX)
	find_library(ALSALIB NAMES asound)
	set(DUMMY_LIBRARY_NAME yssimplesound_linux_alsa)
	set(SUBDIR linux_alsa)
	if(NOT "${ALSALIB}" STREQUAL "ALSALIB-NOTFOUND")
		set(LIBS ${ALSALIB})
		set(SRCS ${SUBDIR}/yssimplesound_linux_alsa.cpp)
	else()
		message("ALSA library not found.")
		message("No sound will be played with yssimplesound library.")
		set(LIBS)
		set(SRCS nownd/yssimplesound_nownd.cpp)
	endif()
endif()



set(TARGET_NAME yssimplesound)
add_library(${TARGET_NAME} yssimplesound.cpp yssimplesound.h ${SRCS})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${TARGET_NAME} PRIVATE ${SUBDIR})
target_link_libraries(${TARGET_NAME} ${LIBS})

if(APPLE)
	set_property(TARGET ${TARGET_NAME} APPEND_STRING PROPERTY COMPILE_FLAGS "-fobjc-arc")
endif()

add_library(yssimplesound_platform ALIAS ${TARGET_NAME})



add_library(yssimplesound_nownd yssimplesound.cpp yssimplesound.h nownd/yssimplesound_nownd.cpp)
target_include_directories(yssimplesound_nownd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(unittest)



set(YS_ALL_BATCH_TEST ${YS_ALL_BATCH_TEST} PARENT_SCOPE)

# For legacy build.
add_library(${DUMMY_LIBRARY_NAME} empty.cpp)
