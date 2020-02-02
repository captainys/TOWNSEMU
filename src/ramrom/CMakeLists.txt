add_library(ramrom ramrom.h ramrom.cpp)
target_link_libraries(ramrom cpputil cpu)
target_include_directories(ramrom PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
