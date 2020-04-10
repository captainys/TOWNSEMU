add_library(townssprite sprite.h sprite.cpp)
target_link_libraries(townssprite device cpputil towns townsdef)
target_include_directories(townssprite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
