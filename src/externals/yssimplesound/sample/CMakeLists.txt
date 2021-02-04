set(TARGET_NAME "yssimplesound_sample")
add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} fssimplewindow yssimplesound yssimplesound_platform)
