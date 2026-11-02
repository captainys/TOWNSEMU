set(TARGET_NAME yssocket_export)
add_library(${TARGET_NAME} yssocket.cpp)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(MINGW)
	target_link_libraries(${TARGET_NAME} ws2_32 wsock32)
endif()
