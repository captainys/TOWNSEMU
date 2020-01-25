add_library(townsmem physmem.h physmem.cpp memaccess.h memaccess.cpp)
target_link_libraries(townsmem device ramrom cpputil townsdef)
target_include_directories(townsmem PUBLIC .)
