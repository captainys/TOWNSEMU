add_executable(main_cui main.cpp)
target_link_libraries(main_cui 
	towns 
	townscommand 
	townsargv
	townsrender 
	outside_world
	fssimplewindow_connection
)

add_subdirectory(command)
add_subdirectory(argv)

add_test(
 	NAME 2MX_V21L30
	COMMAND 
		main_cui 
		"${CMAKE_SOURCE_DIR}/../../TOWNSEMU_TEST/ROM_MX"
		-FD0
		"${CMAKE_SOURCE_DIR}/../../TOWNSEMU_TEST/DISKIMG/testset01.bin"
		-SYM
		"${CMAKE_SOURCE_DIR}/../symtables/UNITTEST.txt"
		-UNITTEST
)

