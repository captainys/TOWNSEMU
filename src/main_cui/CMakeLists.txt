add_executable(main_cui main.cpp)
target_link_libraries(main_cui 
	towns 
	townscommand 
	townsargv
	townsrender 
	outside_world
	fssimplewindow_connection
)

add_subdirectory(command)
add_subdirectory(argv)

add_test(
 	NAME 2MX_V21L30
	COMMAND 
		main_cui 
		"${CMAKE_SOURCE_DIR}/../../TOWNSEMU_TEST/ROM_MX"
		-FD0
		"${CMAKE_SOURCE_DIR}/../../TOWNSEMU_TEST/DISKIMG/testset01.bin"
		-SYM
		"${CMAKE_SOURCE_DIR}/../symtables/UNITTEST.txt"
		-UNITTEST
)

add_test(
 	NAME 2F_V21L30
	COMMAND 
		main_cui 
		"${CMAKE_SOURCE_DIR}/../../TOWNSEMU_TEST/ROM_2F"
		-FD0
		"${CMAKE_SOURCE_DIR}/../../TOWNSEMU_TEST/DISKIMG/testset01.bin"
		-SYM
		"${CMAKE_SOURCE_DIR}/../symtables/UNITTEST.txt"
		-UNITTEST
)

add_test(
 	NAME MX_V21L20
	COMMAND 
		main_cui 
		"${CMAKE_SOURCE_DIR}/../../TOWNSEMU_TEST/ROM_MX"
		-FD0
		"${CMAKE_SOURCE_DIR}/../../TOWNSEMU_TEST/DISKIMG/testset02.bin"
		-SYM
		"${CMAKE_SOURCE_DIR}/../symtables/UNITTEST.txt"
		-UNITTEST
)

add_test(
 	NAME 2F_V21L20
	COMMAND 
		main_cui 
		"${CMAKE_SOURCE_DIR}/../../TOWNSEMU_TEST/ROM_2F"
		-FD0
		"${CMAKE_SOURCE_DIR}/../../TOWNSEMU_TEST/DISKIMG/testset02.bin"
		-SYM
		"${CMAKE_SOURCE_DIR}/../symtables/UNITTEST.txt"
		-UNITTEST
)

add_test(
 	NAME MX_DOSCOMMANDS
	COMMAND 
		main_cui 
		"${CMAKE_SOURCE_DIR}/../../TOWNSEMU_TEST/ROM_MX"
		-FD0
		"${CMAKE_SOURCE_DIR}/../../TOWNSEMU_TEST/DISKIMG/testset03.bin"
		-SYM
		"${CMAKE_SOURCE_DIR}/../symtables/UNITTEST.txt"
		-UNITTEST
)

add_test(
 	NAME 2F_DOSCOMMANDS
	COMMAND 
		main_cui 
		"${CMAKE_SOURCE_DIR}/../../TOWNSEMU_TEST/ROM_2F"
		-FD0
		"${CMAKE_SOURCE_DIR}/../../TOWNSEMU_TEST/DISKIMG/testset03.bin"
		-SYM
		"${CMAKE_SOURCE_DIR}/../symtables/UNITTEST.txt"
		-UNITTEST
)

add_test(
 	NAME 2F_CDROM_ISO_V21L20
	COMMAND 
		main_cui 
		"${CMAKE_SOURCE_DIR}/../../TOWNSEMU_TEST/ROM_2F"
		-FD0
		"${CMAKE_SOURCE_DIR}/../../TOWNSEMU_TEST/DISKIMG/testcdV2.1L20.bin"
		-SYM
		"${CMAKE_SOURCE_DIR}/../symtables/UNITTEST.txt"
		-CD
		"${CMAKE_SOURCE_DIR}/../testdata/ISOIMG/TESTDISC.iso"
		-UNITTEST
)

add_test(
 	NAME MX_CDROM_ISO_V21L30
	COMMAND 
		main_cui 
		"${CMAKE_SOURCE_DIR}/../../TOWNSEMU_TEST/ROM_MX"
		-FD0
		"${CMAKE_SOURCE_DIR}/../../TOWNSEMU_TEST/DISKIMG/testcdV2.1L30.bin"
		-SYM
		"${CMAKE_SOURCE_DIR}/../symtables/UNITTEST.txt"
		-CD
		"${CMAKE_CURRENT_SOURCE_DIR}/../../testdata/ISOIMG/TESTDISC.iso"
		-UNITTEST
)

