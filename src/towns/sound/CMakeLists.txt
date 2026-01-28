add_library(townssound sound.h sound.cpp)
target_link_libraries(townssound device cpputil vgmrecorder towns townsdef ym2612 rf5c68 outside_world)
target_include_directories(townssound PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(townssound PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../externals/yssimplesound/src)

# townssound library only requires yssimplesound.h
# One of yssimplesound or yssimplesound_nownd will be actually linked depending on which of fssimplewindow_interface or headless_mode is linked to the executable.
