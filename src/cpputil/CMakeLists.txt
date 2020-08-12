add_library(cpputil cpputil.cpp cpputil.h)
target_include_directories(cpputil PUBLIC .)


add_executable(pathtest pathtest.cpp)
target_link_libraries(pathtest cpputil)

add_test(
	NAME cpputil_path
	COMMAND pathtest
)
