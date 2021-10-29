cmake_minimum_required(VERSION 3.2)
project(TSUGARU)



set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(DISC_IMAGE_DIR "C:/D/TownsISO")

add_compile_definitions(YS_LITTLE_ENDIAN)
add_compile_definitions(YS_TWOS_COMPLEMENT)
# Believe or not, C/C++ does not assume 2's complement.
# C/C++ does not guarantee that 0xFFFF is interpreted as 16-bit signed negative one.
# In practice, pretty much all CPUs uses 2's complement, and safe to assume it is.
# This flag will skip a little bit of computation.

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
add_subdirectory(ym2612tests)
add_subdirectory(rf5c68)
add_subdirectory(i8251)

add_subdirectory(customstick)

add_subdirectory(towns)

add_subdirectory(tests)

add_subdirectory(discimg)
add_subdirectory(diskimg)

add_subdirectory(main_cui)


#externals
add_subdirectory(externals)


#command-line utilities
add_subdirectory(cmdutil)
