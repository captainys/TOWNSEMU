add_library(device device.cpp device.h deviceutil.cpp deviceutil.h)
target_link_libraries(device vmbase cpputil)
target_include_directories(device PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
