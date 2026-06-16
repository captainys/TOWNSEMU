add_library(townscrtc crtc.h crtc.cpp)
target_link_libraries(townscrtc device cpputil towns townsdef cheapmath townssprite) # Mutually Dependent with towns library
target_include_directories(townscrtc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(fmt3631 fmt3631.h fmt3631.cpp)
target_link_libraries(fmt3631 device cpputil towns townsdef cheapmath townssprite ramrom)
target_include_directories(fmt3631 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
