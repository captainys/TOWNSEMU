add_library(towns_cmd towns_cmd.cpp towns_cmd.h)
target_link_libraries(towns_cmd towns cpputil)
target_include_directories(towns_cmd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
