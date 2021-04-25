cmake_minimum_required(VERSION 3.15)

project(TSUGARU_GUI)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(public/src)
add_subdirectory(../../src/towns/townsdef townsdef)
add_subdirectory(../../src/towns/townsparam townsparam)
add_subdirectory(../../src/diskimg diskimg)
add_subdirectory(../../src/cpputil cpputil)
add_subdirectory(../../src/externals/d77 d77)
add_subdirectory(profile)
add_subdirectory(subprocess)
add_subdirectory(main_gui)
