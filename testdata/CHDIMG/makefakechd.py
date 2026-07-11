#!/usr/bin/env python3
# Regenerates the .CHD test images of testdata/CHDIMG.  Needs chdman (MAME tools) in the path.
#
#   python3 makefakechd.py
#
# It writes two images, both from a fake disc whose contents are a function of the sector number,
# so that src/discimg/testchd.cpp can tell where each byte came from:
#
#   fake2048.chd  One MODE1/2048 data track.  TYPE:MODE1 in the CHD.  Compressed with cdzs (Zstandard).
#   fakemixed.chd One MODE1/2352 data track and two audio tracks, which covers both kinds of pre-gap:
#                 track 2 has an INDEX 00 pre-gap, whose sectors are in the binary and hence in the
#                 CHD (PGTYPE:VAUDIO), and track 3 has a PREGAP pre-gap, whose sectors are in neither
#                 and have to be made up out of silence (PGTYPE:AUDIO).  TYPE:MODE1_RAW and TYPE:AUDIO
#                 in the CHD.  Compressed with cdlz,cdzl,cdfl (what chdman does by default).

import os
import shutil
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))

DATA_SECTORS = 300  # Data track of both images.
AUDIO1_PREGAP_SECTORS = 75  # Pre-gap of track 2.  In the binary (INDEX 00).
AUDIO1_SECTORS = 100
AUDIO2_PREGAP_SECTORS = 150  # Pre-gap of track 3.  Not in the binary (PREGAP).
AUDIO2_SECTORS = 100


def user_data(lba):
	"""2048 bytes of user data of a data sector.  Byte i is (lba+i) & 0xff."""
	return bytes((lba + i) & 0xFF for i in range(2048))


def raw_sector(lba):
	"""2352-byte MODE1 sector.  Sync, header, user data, then zero EDC/ECC."""
	sync = b"\x00" + b"\xff" * 10 + b"\x00"
	msf = lba + 150
	header = bytes([to_bcd(msf // (60 * 75)), to_bcd((msf // 75) % 60), to_bcd(msf % 75), 0x01])
	return sync + header + user_data(lba) + bytes(288)


def audio_sector(lba):
	"""2352-byte audio sector.  588 little-endian stereo samples counting up from the sector number."""
	samples = bytearray()
	for i in range(588 * 2):
		samples += struct.pack("<h", ((lba * 1000 + i) & 0x7FFF))
	return bytes(samples)


def to_bcd(n):
	return ((n // 10) << 4) | (n % 10)


def run_chdman(cue, chd, compression):
	if os.path.exists(chd):
		os.remove(chd)
	subprocess.run(
		["chdman", "createcd", "-i", cue, "-o", chd, "-c", compression],
		check=True, stdout=subprocess.DEVNULL)


def make_2048(tmp):
	bin_name = os.path.join(tmp, "fake2048.bin")
	cue_name = os.path.join(tmp, "fake2048.cue")
	with open(bin_name, "wb") as f:
		for lba in range(DATA_SECTORS):
			f.write(user_data(lba))
	with open(cue_name, "w") as f:
		f.write('FILE "fake2048.bin" BINARY\n')
		f.write("  TRACK 01 MODE1/2048\n")
		f.write("    INDEX 01 00:00:00\n")
	run_chdman(cue_name, os.path.join(HERE, "fake2048.chd"), "cdzs")


def make_mixed(tmp):
	bin_name = os.path.join(tmp, "fakemixed.bin")
	cue_name = os.path.join(tmp, "fakemixed.cue")

	# Disc layout.  Every sector but the pre-gap of track 3 is in the binary, and the content of
	# each sector is a function of its disc LBA, so that the test can tell sectors apart.
	track2_index00 = DATA_SECTORS
	track2_index01 = track2_index00 + AUDIO1_PREGAP_SECTORS
	track3_index00 = track2_index01 + AUDIO1_SECTORS
	track3_index01 = track3_index00 + AUDIO2_PREGAP_SECTORS
	disc_sectors = track3_index01 + AUDIO2_SECTORS

	with open(bin_name, "wb") as f:
		for lba in range(0, DATA_SECTORS):
			f.write(raw_sector(lba))
		for lba in range(track2_index00, track3_index00):
			f.write(audio_sector(lba))
		for lba in range(track3_index01, disc_sectors):
			f.write(audio_sector(lba))

	def msf(lba):
		return "%02d:%02d:%02d" % (lba // (60 * 75), (lba // 75) % 60, lba % 75)

	# The times of a CUE file are positions in the binary, and the pre-gap of track 3 is not in
	# the binary.  So the disc LBA of track 3 is its time here plus that pre-gap.
	with open(cue_name, "w") as f:
		f.write('FILE "fakemixed.bin" BINARY\n')
		f.write("  TRACK 01 MODE1/2352\n")
		f.write("    INDEX 01 00:00:00\n")
		f.write("  TRACK 02 AUDIO\n")
		f.write("    INDEX 00 %s\n" % msf(track2_index00))
		f.write("    INDEX 01 %s\n" % msf(track2_index01))
		f.write("  TRACK 03 AUDIO\n")
		f.write("    PREGAP %s\n" % msf(AUDIO2_PREGAP_SECTORS))
		f.write("    INDEX 01 %s\n" % msf(track3_index00))
	run_chdman(cue_name, os.path.join(HERE, "fakemixed.chd"), "cdlz,cdzl,cdfl")


def main():
	if shutil.which("chdman") is None:
		print("chdman not found in the path.", file=sys.stderr)
		return 1

	tmp = os.path.join(HERE, "tmp")
	os.makedirs(tmp, exist_ok=True)
	try:
		make_2048(tmp)
		make_mixed(tmp)
	finally:
		shutil.rmtree(tmp)
	return 0


if __name__ == "__main__":
	sys.exit(main())
