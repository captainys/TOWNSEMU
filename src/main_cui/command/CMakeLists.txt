add_library(townscommand townscommand.cpp townscommand.h townscommandutil.cpp townscommandutil.h townslineparser.cpp townslineparser.h)
target_link_libraries(townscommand towns cpputil cpu miscutil lineParser)
target_include_directories(townscommand PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
