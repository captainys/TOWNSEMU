add_library(vgmrecorder vgmrecorder.cpp vgmrecorder.h vgmrecorder_ym2612.cpp vgmrecorder_rf5c68.cpp vgmrecorder_ym2203.cpp vgmrecorder_ay8910.cpp)
target_link_libraries(vgmrecorder ym2612 rf5c68)
target_include_directories(vgmrecorder PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
