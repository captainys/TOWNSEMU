cmake_minimum_required(VERSION 3.20)

project(TSUGARU_GUI)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(UNIX)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-variable -Wno-incompatible-pointer-types")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable")

	include_directories(/usr/local/include)
	link_directories(/usr/local/lib)
endif()

set(DISC_IMAGE_DIR "C:/D/TownsISO")

add_compile_definitions(YS_LITTLE_ENDIAN)
add_compile_definitions(YS_TWOS_COMPLEMENT)
# Believe or not, C/C++ does not assume 2's complement.
# C/C++ does not guarantee that 0xFFFF is interpreted as 16-bit signed negative one.
# In practice, pretty much all CPUs uses 2's complement, and safe to assume it is.
# This flag will skip a little bit of computation.

add_subdirectory(public/src)


add_subdirectory(../../src/resources resources)

add_subdirectory(../../src/osdependent osdependent)

add_subdirectory(../../src/serversample serversample)

add_subdirectory(../../src/cpputil cpputil)
add_subdirectory(../../src/lineparser lineparser)
add_subdirectory(../../src/miscutil miscutil)
add_subdirectory(../../src/cheapmath cheapmath)

add_subdirectory(../../src/vmbase vmbase)
add_subdirectory(../../src/device device)
add_subdirectory(../../src/cpu cpu)
add_subdirectory(../../src/inout inout)
add_subdirectory(../../src/ramrom ramrom)
add_subdirectory(../../src/vgmrecorder vgmrecorder)
add_subdirectory(../../src/ym2612 ym2612)
# add_subdirectory(../../src/ym2612tests ym2612tests)
add_subdirectory(../../src/rf5c68 rf5c68)
add_subdirectory(../../src/i8251 i8251)
add_subdirectory(../../src/i8253 i8253)
add_subdirectory(../../src/i8251_to_socket i8251_to_socket)
add_subdirectory(../../src/diskdrive diskdrive)

add_subdirectory(../../src/customstick customstick)

add_subdirectory(../../src/towns towns)

# add_subdirectory(../../src/tests tests)

add_subdirectory(../../src/discimg discimg)
add_subdirectory(../../src/diskimg diskimg)

add_subdirectory(../../src/main_cui main_cui)

add_subdirectory(../../src/externals/d77 d77)
add_subdirectory(../../src/externals/yspng yspng)
add_subdirectory(../../src/externals/yssocket/src yssocket)
add_subdirectory(../../src/externals/connect_fssimplewindow connect_fssimplewindow)


add_subdirectory(subprocess)
add_subdirectory(main_gui)
add_subdirectory(tests_gui)
