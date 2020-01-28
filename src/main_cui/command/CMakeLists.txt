add_library(townscommand townscommand.cpp townscommand.h)
target_link_libraries(townscommand towns cpputil)
target_include_directories(townscommand PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
