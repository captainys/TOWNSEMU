add_library(townsmem physmem.h physmem.cpp memaccess.h memaccess.cpp)
target_link_libraries(townsmem rf5c68 cpu device ramrom cpputil towns townsdef)
target_include_directories(townsmem PUBLIC .)
