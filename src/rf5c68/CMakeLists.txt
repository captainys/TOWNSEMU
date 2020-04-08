add_library(rf5c68 rf5c68.cpp rf5c68.h)
target_link_libraries(rf5c68 cpputil)
target_include_directories(rf5c68 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
