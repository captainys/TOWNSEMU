add_library(townsvndrv vndrv.h vndrv.cpp)
target_link_libraries(townsvndrv device filesys cpputil miscutil towns townsdef)
target_include_directories(townsvndrv PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
