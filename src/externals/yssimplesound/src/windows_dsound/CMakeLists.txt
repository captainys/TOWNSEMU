set(TARGET_NAME yssimplesound_dsound)
add_library(${TARGET_NAME} yssimplesound_dsound.cpp)
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(${TARGET_NAME} yssimplesound)

add_library(yssimplesound_platform ALIAS ${TARGET_NAME})
