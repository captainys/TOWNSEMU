add_library(virtual_network virtual_network.h virtual_network.cpp)
target_link_libraries(virtual_network cpputil miscutil)
target_include_directories(virtual_network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


add_library(rex3586 rex3586.h rex3586.cpp)
target_link_libraries(rex3586 device cpputil towns townsdef outside_world miscutil virtual_network)
target_include_directories(rex3586 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
