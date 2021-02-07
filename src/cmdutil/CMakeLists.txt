add_executable(flattenbincue flattenbincue.cpp)
target_link_libraries(flattenbincue discimg cpputil)

add_executable(padbincue padbincue.cpp)
target_link_libraries(padbincue discimg cpputil)

add_executable(chopbincue chopbincue.cpp)
target_link_libraries(chopbincue discimg cpputil)

add_executable(wav2snd wav2snd.cpp)
target_link_libraries(wav2snd yssimplesound yssimplesound_nownd)
