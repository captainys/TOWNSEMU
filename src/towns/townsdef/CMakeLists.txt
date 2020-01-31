add_library(townsdef townsdef.cpp townsmap.cpp)
target_include_directories(townsdef PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(util_makeIOLabel util_makeIOLabel.cpp)
