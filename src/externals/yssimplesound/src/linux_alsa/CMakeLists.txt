set(TARGET_NAME yssimplesound_linux_alsa)



find_library(ALSALIB NAMES asound)
if(NOT "${ALSALIB}" STREQUAL "ALSALIB-NOTFOUND")
  set(LIBS ${ALSALIB})
  set(SRCS yssimplesound_linux_alsa.cpp)
else()
  message("ALSA library not found.")
  message("No sound will be played with yssimplesound library.")
  set(LIBS)
  set(SRCS ../nownd/yssimplesound_nownd.cpp)
endif()



add_library(${TARGET_NAME} ${SRCS})
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(${TARGET_NAME} yssimplesound ${LIBS})

add_library(yssimplesound_platform ALIAS ${TARGET_NAME})
