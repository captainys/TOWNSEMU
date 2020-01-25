add_library(towns towns.h towns.cpp townsio.cpp townsio.h)
target_link_libraries(towns cpu device inout ioram ramrom townsmem townsdef)
target_include_directories(towns PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(ioram)
add_subdirectory(memory)
