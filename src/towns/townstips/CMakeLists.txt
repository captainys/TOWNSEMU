set(TARGET_NAME townstips)
add_library(${TARGET_NAME} townstips.h townstips.cpp)
target_link_libraries(${TARGET_NAME} townsdef)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
