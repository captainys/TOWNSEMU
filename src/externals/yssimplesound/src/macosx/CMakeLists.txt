set(TARGET_NAME yssimplesound_macosx)

add_library(${TARGET_NAME} yssimplesound_macosx_cpp.cpp yssimplesound_macosx_objc.m ../yssimplesound.h)

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(${TARGET_NAME} yssimplesound)

if(APPLE)
	find_library(AVFOUNDATION_LIB AVFoundation)
	mark_as_advanced(AVFOUNDATION_LIB)
	target_link_libraries(${TARGET_NAME} ${AVFOUNDATION_LIB})
endif()

set_property(TARGET yssimplesound_macosx APPEND_STRING PROPERTY COMPILE_FLAGS "-fobjc-arc")

add_library(yssimplesound_platform ALIAS ${TARGET_NAME})
