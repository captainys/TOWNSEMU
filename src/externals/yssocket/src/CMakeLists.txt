set(TARGET_NAME yssocket_export)
add_library(${TARGET_NAME} yssocket.cpp)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
