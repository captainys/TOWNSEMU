add_executable(Tsugaru_Headless main.cpp ../resources/tsugaru_icon.rc)
target_link_libraries(Tsugaru_Headless
	towns 
	townscommand
	townsargv
	townsrender
	outside_world
	headless_mode
)
