add_library(townsrender render.h render.cpp)
target_link_libraries(townsrender townscrtc townsmem cpputil towns townsdef)
target_include_directories(townsrender PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
