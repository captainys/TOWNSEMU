set(TARGET_NAME townsparam)
add_library(${TARGET_NAME} townsparam.h townsparam.cpp)
target_link_libraries(${TARGET_NAME} townsdef)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
