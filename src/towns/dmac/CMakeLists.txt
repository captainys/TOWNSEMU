add_library(townsdmac dmac.h dmac.cpp)
target_link_libraries(townsdmac device cpputil towns townsdef)
target_include_directories(townsdmac PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
