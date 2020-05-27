set(TARGET_NAME png2cpp)
add_executable(${TARGET_NAME} png2cpp.cpp)
target_link_libraries(${TARGET_NAME} yspng cpputil)
