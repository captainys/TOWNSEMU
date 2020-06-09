import os
import subprocess
import shutil
import sys

import build

TOWNSTYPE="2F"

THISFILE=os.path.realpath(__file__)
THISDIR=os.path.dirname(THISFILE)
BUILDDIR=os.path.join(THISDIR,"..","build")
SRCDIR=os.path.join(THISDIR,"..","src")
ROMDIR=os.path.join(THISDIR,"..","..","TOWNSEMU_TEST","ROM_"+TOWNSTYPE)
DISKDIR=os.path.join(THISDIR,"..","..","TOWNSEMU_TEST","DISKIMG")



def ExeExtension():
	if sys.platform.startswith('win'):
		return ".exe"
	else:
		return ""


def Run():
	os.chdir(BUILDDIR)
	subprocess.Popen([
		"./main_cui/Tsugaru_CUI"+ExeExtension(),
		ROMDIR,
		"-FD0",
		os.path.join(DISKDIR,"V2.1L30.bin"),
		"-SYM",
		"../symtables/V2.1L30"+TOWNSTYPE+".txt",
		"-HD0",
		os.path.join(DISKDIR,"hddimage.bin"),
		"-HD0",
		os.path.join(DISKDIR,"hddimage.bin"),
		"-CMOS",
		"../testdata/CMOS.bin",
		"-DONTAUTOSAVECMOS",
		"-DEBUG",
		"-PAUSE",
	]).wait()



if __name__=="__main__":
	build.Run()
	Run()
