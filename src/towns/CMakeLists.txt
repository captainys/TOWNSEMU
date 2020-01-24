add_library(towns towns.h towns.cpp townsio.cpp townsio.h)
target_link_libraries(towns cpu device inout ioram ramrom townsdef)
target_include_directories(towns PUBLIC .)
