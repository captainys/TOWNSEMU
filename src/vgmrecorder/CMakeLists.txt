add_library(vgmrecorder vgmrecorder.cpp vgmrecorder.h)
target_link_libraries(vgmrecorder ym2612 rf5c68)
target_include_directories(vgmrecorder PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
