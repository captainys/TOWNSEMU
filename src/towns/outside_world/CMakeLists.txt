add_library(outside_world outside_world.h outside_world.cpp icons.h icons.cpp)
target_include_directories(outside_world PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(outside_world towns townsdef townsparam townsrender discimg rf5c68 ym2612)


add_library(headless_mode headless_mode.h headless_mode.cpp)
target_include_directories(headless_mode PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(headless_mode outside_world yssimplesound_nownd)
