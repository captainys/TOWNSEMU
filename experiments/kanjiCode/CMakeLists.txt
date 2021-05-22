cmake_minimum_required(VERSION 3.8)
project(YS)
# Need fssimplewindow and yssystemfont libraries in public repository

add_subdirectory(../../../public/src public)

add_executable(verify MACOSX_BUNDLE verify.cpp)
target_link_libraries(verify fssimplewindow yssystemfont)
