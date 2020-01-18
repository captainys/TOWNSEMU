add_library(inout inout.h inout.cpp)
target_link_libraries(inout device cpputil)
target_include_directories(inout PUBLIC .)
