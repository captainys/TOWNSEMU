set(TARGET_NAME townshighrespcm)
add_library(${TARGET_NAME} highrespcm.cpp highrespcm.h)
target_link_libraries(${TARGET_NAME} townsdmac device cpputil towns townsdef)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
