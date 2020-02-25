add_library(townscdrom cdrom.h cdrom.cpp)
target_link_libraries(townscdrom device cpputil townsdef discimg)
target_include_directories(townscdrom PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
