set(TARGET_NAME vmbase)
add_library(${TARGET_NAME} vmbase.h vmbase.cpp)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${TARGET_NAME} device)
