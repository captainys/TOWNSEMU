set(TARGET_NAME diskimg)
add_library(${TARGET_NAME} diskimg.h diskimg.cpp 1232KB.c 1440KB.c 640KB.c 720KB.c)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
