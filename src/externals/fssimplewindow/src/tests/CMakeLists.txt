add_executable(test_resize MACOSX_BUNDLE test_resize.cpp)
target_link_libraries(test_resize fssimplewindow)
add_test(NAME FsSimpleWindow_Resize COMMAND test_resize)

list(APPEND YS_ALL_BATCH_TEST test_resize)
set(YS_ALL_BATCH_TEST ${YS_ALL_BATCH_TEST} PARENT_SCOPE)
