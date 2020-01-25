add_library(ioram ioram.h ioram.cpp)
target_link_libraries(ioram device cpputil)
target_include_directories(ioram PUBLIC .)
