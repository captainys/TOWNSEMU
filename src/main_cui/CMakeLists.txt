add_executable(main_cui main.cpp)
target_link_libraries(main_cui towns townscommand townsrender)

add_subdirectory(command)
