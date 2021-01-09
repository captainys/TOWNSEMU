add_library(ym2612 ym2612.cpp ym2612wave.cpp ym2612.h)
target_include_directories(ym2612 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
