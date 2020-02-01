add_library(townsfdc fdc.h fdc.cpp)
target_link_libraries(townsfdc device cpputil towns townsdef)
target_include_directories(townsfdc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
