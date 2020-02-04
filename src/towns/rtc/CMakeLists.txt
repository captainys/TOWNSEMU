add_library(townsrtc rtc.h rtc.cpp)
target_link_libraries(townsrtc device cpputil towns townsdef)
target_include_directories(townsrtc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
