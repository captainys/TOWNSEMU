add_library(ramrom ramrom.h ramrom.cpp)
target_link_libraries(ramrom device)
target_include_directories(ramrom PUBLIC .)
