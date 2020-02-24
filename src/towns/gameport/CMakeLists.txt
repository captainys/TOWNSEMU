add_library(townsgameport gameport.h gameport.cpp)
target_link_libraries(townsgameport device cpputil towns townsdef cheapmath)
target_include_directories(townsgameport PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
