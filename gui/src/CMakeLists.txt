cmake_minimum_required(VERSION 3.15)

project(TSUGARU_GUI)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DISC_IMAGE_DIR "C:/D/TownsISO")

add_compile_definitions(YS_LITTLE_ENDIAN)

add_subdirectory(public/src)


add_subdirectory(../../src/resources resources)

add_subdirectory(../../src/osdependent osdependent)

add_subdirectory(../../src/cpputil cpputil)
add_subdirectory(../../src/lineparser lineparser)
add_subdirectory(../../src/miscutil miscutil)
add_subdirectory(../../src/cheapmath cheapmath)

add_subdirectory(../../src/vmbase vmbase)
add_subdirectory(../../src/device device)
add_subdirectory(../../src/cpu cpu)
add_subdirectory(../../src/inout inout)
add_subdirectory(../../src/ramrom ramrom)
add_subdirectory(../../src/ym2612 ym2612)
# add_subdirectory(../../src/ym2612tests ym2612tests)
add_subdirectory(../../src/rf5c68 rf5c68)
add_subdirectory(../../src/i8251 i8251)

add_subdirectory(../../src/customstick customstick)

add_subdirectory(../../src/towns towns)

# add_subdirectory(../../src/tests tests)

add_subdirectory(../../src/discimg discimg)
add_subdirectory(../../src/diskimg diskimg)

add_subdirectory(../../src/main_cui main_cui)

add_subdirectory(../../src/externals/d77 d77)
add_subdirectory(../../src/externals/connect_fssimplewindow connect_fssimplewindow)


add_subdirectory(profile)
add_subdirectory(subprocess)
add_subdirectory(main_gui)
