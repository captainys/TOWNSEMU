add_library(townsargv townsargv.cpp townsargv.h)
target_link_libraries(townsargv towns townsdef cpputil cpu)
target_include_directories(townsargv PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
