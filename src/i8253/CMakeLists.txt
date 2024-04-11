set(TARGET_NAME i8253)
add_library(${TARGET_NAME} i8253.h i8253.cpp)
target_link_libraries(${TARGET_NAME} cpputil)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
