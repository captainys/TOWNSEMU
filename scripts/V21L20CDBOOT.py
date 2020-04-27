import os
import subprocess
import shutil

import build

TOWNSTYPE="MX"

THISFILE=os.path.realpath(__file__)
THISDIR=os.path.dirname(THISFILE)
BUILDDIR=os.path.join(THISDIR,"..","build")
SRCDIR=os.path.join(THISDIR,"..","src")
ROMDIR=os.path.join(THISDIR,"..","..","TOWNSEMU_TEST","ROM_"+TOWNSTYPE)
DISKDIR=os.path.join(THISDIR,"..","..","TOWNSEMU_TEST","DISKIMG")



def Run():
	os.chdir(BUILDDIR)
	subprocess.Popen([
		"./main_cui/main_cui.exe",
		ROMDIR,
		"-FD0",
		os.path.join(DISKDIR,"V2.1L20.bin"),
		"-FD1",
		os.path.join(DISKDIR,"BDrive.bin"),
		"-CD",
		"D:/ISOImageSubset/TownsOSV2.1L20.cue",
		"-SYM",
		"../symtables/V2.1L20CDBOOT_"+TOWNSTYPE+".txt",
		"-DEBUG",
		"-PAUSE",
	]).wait()



if __name__=="__main__":
	build.Run()
	Run()
