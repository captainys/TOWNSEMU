add_library(townsserial serialport.h serialport.cpp)
target_link_libraries(townsserial device cpputil towns townsdef i8251 outside_world)
target_include_directories(townsserial PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
