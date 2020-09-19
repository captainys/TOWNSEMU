add_library(townsscsi scsi.h scsi.cpp)
target_link_libraries(townsscsi device discimg cpputil towns townsdef)
target_include_directories(townsscsi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
