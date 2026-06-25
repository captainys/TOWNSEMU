add_library(haslayer haslayer.h haslayer.cpp)
target_link_libraries(haslayer townsdef cheapmath)
target_include_directories(haslayer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(townscrtc crtc.h crtc.cpp)
target_link_libraries(townscrtc device haslayer cpputil towns townsdef townssprite) # Mutually Dependent with towns library
target_include_directories(townscrtc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(fmt3631 fmt3631.h fmt3631.cpp)
target_link_libraries(fmt3631 device haslayer cpputil towns townsdef townssprite ramrom)
target_include_directories(fmt3631 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
