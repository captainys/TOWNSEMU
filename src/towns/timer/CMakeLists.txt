add_library(townstimer timer.h timer.cpp)
target_link_libraries(townstimer device cpputil towns townsdef)
target_include_directories(townstimer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
