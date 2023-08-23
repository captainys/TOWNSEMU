set(TARGET_NAME i8251tosocket)
add_library(${TARGET_NAME} i8251tosocket.h i8251tosocket.cpp)
target_link_libraries(${TARGET_NAME} cpputil i8251 yssocket_export)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
