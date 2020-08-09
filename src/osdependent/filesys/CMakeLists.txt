set(TARGET_NAME filesys)
add_library(${TARGET_NAME} filesys.h filesys_null.cpp)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
