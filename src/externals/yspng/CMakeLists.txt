set(TARGET_NAME yspng)
add_library(${TARGET_NAME} yspng yspng.h yspng.cpp yspngenc.h yspngenc.cpp)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
