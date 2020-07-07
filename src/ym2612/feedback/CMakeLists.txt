set(TARGET_NAME feedback)
add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} fssimplewindow ym2612)
