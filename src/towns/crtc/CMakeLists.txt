add_library(townscrtc crtc.h crtc.cpp)
target_link_libraries(townscrtc device cpputil towns townsdef cheapmath) # Mutual Dependent with towns library
target_include_directories(townscrtc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
