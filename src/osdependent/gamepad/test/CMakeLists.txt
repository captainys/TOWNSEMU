set(TARGET_NAME ysgamepad_test)
add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} ysgamepad fssimplewindow)
