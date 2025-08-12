set(TARGET_NAME townsprofile)
add_library(${TARGET_NAME} ${TARGET_NAME}.cpp townsprofile.h)
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${TARGET_NAME} townsdef townsparam cpputil)
