# libchdr (https://github.com/rtissera/libchdr) -- BSD-3-Clause.  See LICENSE.txt.
# Vendored from commit 04a177ee3cea055d93da2d5839d3413168837c6f.
# This CMakeLists is written for Tsugaru.  It builds a single static library from
# libchdr and its bundled decompressors (LZMA, miniz, Zstandard), and leaves out
# the shared-library, install, and test targets of the original build script.

add_library(chdr STATIC
	src/libchdr_bitstream.c
	src/libchdr_cdrom.c
	src/libchdr_chd.c
	src/libchdr_codec_cdfl.c
	src/libchdr_codec_cdlz.c
	src/libchdr_codec_cdzl.c
	src/libchdr_codec_cdzs.c
	src/libchdr_codec_flac.c
	src/libchdr_codec_huff.c
	src/libchdr_codec_lzma.c
	src/libchdr_codec_zlib.c
	src/libchdr_codec_zstd.c
	src/libchdr_flac.c
	src/libchdr_huffman.c

	deps/lzma-25.01/src/LzmaDec.c
	deps/miniz-3.1.1/miniz.c
	deps/zstd-1.5.7/zstddeclib.c
)

# The libchdr sources reach into deps/ with relative paths, so only include/ needs to be added.
target_include_directories(chdr PUBLIC include)

target_compile_definitions(chdr PRIVATE
	MINIZ_NO_ARCHIVE_APIS
	MINIZ_NO_DEFLATE_APIS
	MINIZ_NO_STDIO
	MINIZ_NO_TIME

	WANT_RAW_DATA_SECTOR=1  # Reconstruct sync/header/ECC of MODE1_RAW sectors.
	WANT_SUBCODE=1
	VERIFY_BLOCK_CRC=1
)

set_target_properties(chdr PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(MSVC)
	target_compile_definitions(chdr PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
