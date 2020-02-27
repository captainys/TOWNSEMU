add_library(discimg discimg.h discimg.cpp)
target_link_libraries(discimg cpputil)
target_include_directories(discimg PUBLIC .)

add_executable(testmsf testmsf.cpp)
target_link_libraries(testmsf discimg)
add_test(NAME DISCIMG_MSF COMMAND testmsf)

add_executable(testiso testiso.cpp)
target_link_libraries(testiso discimg)
add_test(NAME DISCIMG_ISO COMMAND testiso ${CMAKE_CURRENT_SOURCE_DIR}/../../testdata/ISOIMG/iso4sect.iso)
