add_library(cpu cpu.h cpu.cpp i486.h i486.cpp i486inst.h i486inst.cpp)
target_link_libraries(cpu device ramrom inout cpputil)
target_include_directories(cpu PUBLIC .)
