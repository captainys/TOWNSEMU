add_library(cpputil cpputil.cpp cpputil.h)
target_include_directories(cpputil PUBLIC .)


add_executable(pathtest pathtest.cpp)
target_link_libraries(pathtest cpputil)

add_test(
	NAME cpputil_path
	COMMAND pathtest
)


add_executable(inttest inttest.cpp)
target_link_libraries(inttest cpputil)

add_test(
	NAME cpputil_int
	COMMAND inttest
)


add_executable(inttest2 inttest2.cpp)
target_link_libraries(inttest2 cpputil)

add_test(
	NAME cpputil_int_not_little_endian
	COMMAND inttest2
)
