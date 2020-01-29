add_library(townscommand townscommand.cpp townscommand.h townscommandutil.cpp townscommandutil.h)
target_link_libraries(townscommand towns cpputil cpu)
target_include_directories(townscommand PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
