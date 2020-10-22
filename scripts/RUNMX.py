import os
import subprocess
import shutil
import sys
import sys

import build

TOWNSTYPE="MX"

THISFILE=os.path.realpath(__file__)
THISDIR=os.path.dirname(THISFILE)
BUILDDIR=os.path.join(THISDIR,"..","build")
SRCDIR=os.path.join(THISDIR,"..","src")
ROMDIR=os.path.join(THISDIR,"..","..","TOWNSEMU_TEST","ROM_"+TOWNSTYPE)
DISKDIR=os.path.join(THISDIR,"..","..","TOWNSEMU_TEST","DISKIMG")
MEMCARDDIR=os.path.join(THISDIR,"..","..","TOWNSEMU_TEST","MEMCARD")



def ExeExtension():
	if sys.platform.startswith('win'):
		return ".exe"
	else:
		return ""


def Run(argv):
	os.chdir(BUILDDIR)
	subprocess.Popen([
		"./main_cui/Tsugaru_CUI"+ExeExtension(),
		ROMDIR,
		"-SYM",
		"../symtables/RUN"+TOWNSTYPE+".txt",
		"-HD0",
		os.path.join(DISKDIR,"hddimage.bin"),
		"-HD1",
		os.path.join(DISKDIR,"40MB.h1"),
		"-JEIDA4",
		os.path.join(MEMCARDDIR,"4MB.bin"),
		"-CMOS",
		"../testdata/CMOS.bin",
		"-DONTAUTOSAVECMOS",
		#"-HIGHRES",
		#"-DEBUG",
		#"-PAUSE",
	]+argv).wait()



if __name__=="__main__":
	build.Run()
	Run(sys.argv[1:])
