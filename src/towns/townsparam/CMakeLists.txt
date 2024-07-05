set(TARGET_NAME townsparam)
add_library(${TARGET_NAME} townsparam.h townsparam.cpp)
target_link_libraries(${TARGET_NAME} townsdef cpu)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(townsoptions townsoptions.h townsoptions.cpp)
target_link_libraries(townsoptions townsdef cpu)
target_include_directories(townsoptions PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
