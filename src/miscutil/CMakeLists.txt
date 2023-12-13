add_library(miscutil miscutil.cpp miscutil.h sjis2utf8.cpp sjis2utf8.h)
target_link_libraries(miscutil cpu ramrom cpputil)
target_include_directories(cpputil PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
