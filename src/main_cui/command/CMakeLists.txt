add_library(townscommand townscommand.cpp townscommand.h townscommandutil.cpp townscommandutil.h townslineparser.cpp townslineparser.h)
target_link_libraries(townscommand towns cpputil filesys cpu miscutil lineParser yspng)
target_include_directories(townscommand PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
