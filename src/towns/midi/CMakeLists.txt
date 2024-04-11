add_library(townsmidi midi.h midi.cpp)
target_link_libraries(townsmidi device i8253 cpputil towns townsdef outside_world midi_interface)
target_include_directories(townsmidi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
