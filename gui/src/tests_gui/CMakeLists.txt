set(TARGET_NAME profiletest)

add_executable(profiletest profiletest.cpp)
target_link_libraries(profiletest cpputil townsprofile)

add_test(NAME profile COMMAND profiletest)
