cmake_minimum_required(VERSION 3.8)
project(shootinggame)



set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



enable_testing()

add_subdirectory(cpputil)
add_subdirectory(miscutil)

add_subdirectory(device)
add_subdirectory(cpu)
add_subdirectory(inout)
add_subdirectory(ramrom)
add_subdirectory(ym2612)

add_subdirectory(towns)

add_subdirectory(tests)

add_subdirectory(main_cui)


#externals
add_subdirectory(externals)
