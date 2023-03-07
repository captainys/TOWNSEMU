#Run CMake from this directory to compile in the High-Fidelity Mode.
add_compile_definitions(TSUGARU_I486_HIGH_FIDELITY)
add_subdirectory(../src ${CMAKE_BINARY_DIR})
set(TSUGARU_I486_HIGH_FIDELITY TRUE)
