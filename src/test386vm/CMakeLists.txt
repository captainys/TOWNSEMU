set(TEST386_BIN "${CMAKE_SOURCE_DIR}/../../TOWNSEMU_TEST/TEST386/test386.bin")

add_executable(test386 main.cpp)
target_link_libraries(test386 cpu vmbase)

add_test(NAME TEST386 COMMAND test386 ${TEST386_BIN})
