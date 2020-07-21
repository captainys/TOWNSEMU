set(TARGET_NAME i8251)
add_library(${TARGET_NAME} i8251.h i8251.cpp)
target_link_libraries(${TARGET_NAME} cpputil)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
