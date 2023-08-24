set(TARGET_NAME telnetServer)

add_executable(${TARGET_NAME} server.cpp)
target_link_libraries(${TARGET_NAME} yssocket_export)

if(UNIX)
	target_link_libraries(${TARGET_NAME} pthread)
endif()
