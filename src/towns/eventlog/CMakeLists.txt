add_library(townseventlog eventlog.h eventlog.cpp)
target_link_libraries(townseventlog cpputil towns townsdef cheapmath)
target_include_directories(townseventlog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
