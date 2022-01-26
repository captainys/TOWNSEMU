set(TARGET_NAME test_fullscreen)

add_executable(${TARGET_NAME} MACOSX_BUNDLE fullscreen.cpp)
target_link_libraries(${TARGET_NAME} fssimplewindow)

add_executable(test_resize MACOSX_BUNDLE test_resize.cpp)
target_link_libraries(test_resize fssimplewindow)
add_test(NAME FsSimpleWindow_Resize COMMAND test_resize)

list(APPEND YS_ALL_BATCH_TEST test_resize)
set(YS_ALL_BATCH_TEST ${YS_ALL_BATCH_TEST} PARENT_SCOPE)
