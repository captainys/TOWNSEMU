add_library(discimg discimg.h discimg.cpp)
target_include_directories(discimg PUBLIC .)

add_executable(testmsf testmsf.cpp)
target_link_libraries(testmsf discimg)

add_test(NAME DISCIMG_MSF COMMAND testmsf)
