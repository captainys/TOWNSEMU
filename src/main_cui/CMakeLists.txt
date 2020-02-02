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
