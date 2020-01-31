add_library(device device.cpp device.h deviceskeleton.cpp)
target_link_libraries(device cpputil)
target_include_directories(device PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
