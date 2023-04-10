cmake_minimum_required(VERSION 3.2)
project(TEST386)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(YS_LITTLE_ENDIAN)
add_compile_definitions(YS_TWOS_COMPLEMENT)

add_compile_definitions(TSUGARU_I486_UPDATE_PAGE_DA_FLAGS)
add_compile_definitions(TSUGARU_I486_MORE_EXCEPTION_HANDLING)
add_compile_definitions(TSUGARU_I486_PROTECT_ESP_HIGHWORD)
add_compile_definitions(TSUGARU_I486_HIGH_FIDELITY)

add_subdirectory(../src/cpputil ${CMAKE_BINARY_DIR}/cpputil)
add_subdirectory(../src/device ${CMAKE_BINARY_DIR}/device)
add_subdirectory(../src/vmbase ${CMAKE_BINARY_DIR}/vmbase)
add_subdirectory(../src/ramrom ${CMAKE_BINARY_DIR}/ramrom)
add_subdirectory(../src/inout ${CMAKE_BINARY_DIR}/inout)
add_subdirectory(../src/cpu ${CMAKE_BINARY_DIR}/cpu)

enable_testing()

set(TEST386_BIN "${CMAKE_CURRENT_SOURCE_DIR}/../../TOWNSEMU_TEST/TEST386/test386.bin")

add_executable(test386 main.cpp)
target_link_libraries(test386 cpu vmbase)

add_test(NAME TEST386 COMMAND test386 ${TEST386_BIN})
