add_library(diskdrive diskdrive.h diskdrive.cpp)
target_link_libraries(diskdrive d77 device cpputil)
target_include_directories(diskdrive PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
