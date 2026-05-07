add_library(rex3586 rex3586.h rex3586.cpp)
target_link_libraries(rex3586 device cpputil towns townsdef outside_world)
target_include_directories(rex3586 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
