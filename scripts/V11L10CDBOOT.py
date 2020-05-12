import os
import subprocess
import shutil
import sys

import build

TOWNSTYPE="MX"

THISFILE=os.path.realpath(__file__)
THISDIR=os.path.dirname(THISFILE)
BUILDDIR=os.path.join(THISDIR,"..","build")
SRCDIR=os.path.join(THISDIR,"..","src")
ROMDIR=os.path.join(THISDIR,"..","..","TOWNSEMU_TEST","ROM_"+TOWNSTYPE)
DISKDIR=os.path.join(THISDIR,"..","..","TOWNSEMU_TEST","DISKIMG")



def Run(argv):
	os.chdir(BUILDDIR)
	subprocess.Popen([
		"./main_cui/main_cui.exe",
		ROMDIR,
		#"-FREQ",
		#"16",
		"-FD0",
		os.path.join(DISKDIR,"V2.1L20.bin"),
		"-FD1",
		os.path.join(DISKDIR,"BDrive.bin"),
		"-CD",
		"D:/ISOImageSubset/TOSV1.1L10.iso",
		"-SYM",
		"../symtables/V1.1L10CDBOOT_"+TOWNSTYPE+".txt",
		"-DEBUG",
		"-PAUSE",
	]+argv).wait()



if __name__=="__main__":
	build.Run()
	Run(sys.argv[1:])
