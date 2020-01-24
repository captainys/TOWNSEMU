add_library(towns towns.h towns.cpp)
target_link_libraries(towns cpu device inout ioram ramrom)
target_include_directories(towns PUBLIC .)
