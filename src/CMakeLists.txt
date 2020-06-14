cmake_minimum_required(VERSION 3.2)
project(TSUGARU)



set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(DISC_IMAGE_DIR "D:/ISOImageSubset")

add_compile_definitions(YS_LITTLE_ENDIAN)

enable_testing()

add_subdirectory(resources)

add_subdirectory(osdependent)

add_subdirectory(cpputil)
add_subdirectory(lineparser)
add_subdirectory(miscutil)
add_subdirectory(cheapmath)

add_subdirectory(vmbase)
add_subdirectory(device)
add_subdirectory(cpu)
add_subdirectory(inout)
add_subdirectory(ramrom)
add_subdirectory(ym2612)
add_subdirectory(rf5c68)

add_subdirectory(towns)

add_subdirectory(tests)

add_subdirectory(discimg)

add_subdirectory(main_cui)


#externals
add_subdirectory(externals)
