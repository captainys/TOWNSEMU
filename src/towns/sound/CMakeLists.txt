add_library(townssound sound.h sound.cpp)
target_link_libraries(townssound device cpputil towns townsdef ym2612)
target_include_directories(townssound PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
