set(TARGET_NAME customstick)
add_library(${TARGET_NAME} customstick.h customstick.cpp)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
