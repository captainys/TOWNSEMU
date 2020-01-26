add_library(townskeyboard keyboard.h keyboard.cpp)
target_link_libraries(townskeyboard device cpputil)
target_include_directories(townskeyboard PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
