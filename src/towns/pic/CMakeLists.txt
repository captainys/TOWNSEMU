add_library(townspic pic.h pic.cpp)
target_link_libraries(townspic device cpputil towns townsdef)
target_include_directories(townspic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
