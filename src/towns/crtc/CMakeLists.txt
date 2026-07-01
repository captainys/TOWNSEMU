add_library(crtcbase crtcbase.h crtcbase.cpp)
target_link_libraries(crtcbase townsdef cheapmath device)
target_include_directories(crtcbase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(townscrtc crtc.h crtc.cpp)
target_link_libraries(townscrtc device crtcbase cpputil towns townsdef townssprite) # Mutually Dependent with towns library
target_include_directories(townscrtc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(fmt3631 fmt3631.h fmt3631.cpp)
target_link_libraries(fmt3631 device crtcbase cpputil towns townsdef townssprite ramrom)
target_include_directories(fmt3631 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
