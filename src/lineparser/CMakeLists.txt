add_library(lineParser lineparser.h lineparser.cpp)
target_include_directories(lineParser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})



set(TARGET_NAME lineParserTest)
add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} lineParser)
