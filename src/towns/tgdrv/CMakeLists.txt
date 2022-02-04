add_library(townstgdrv tgdrv.cpp tgdrv.h)
target_link_libraries(townstgdrv device filesys cpputil miscutil towns townsdef)
target_include_directories(townstgdrv PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
