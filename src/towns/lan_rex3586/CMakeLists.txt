add_library(network virtual_network.h virtual_network.cpp real_network real_network.h real_network.cpp)
target_link_libraries(network cpputil miscutil)
target_include_directories(network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


add_library(rex3586 rex3586.h rex3586.cpp)
target_link_libraries(rex3586 device cpputil towns townsdef outside_world miscutil network)
target_include_directories(rex3586 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
