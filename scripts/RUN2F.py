import os
import subprocess
import shutil
import sys
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



def TsugaruExe():
	f=os.path.join(BUILDDIR,"main_cui/Tsugaru_CUI"+ExeExtension())
	if os.path.isfile(f):
		return f
	f=os.path.join(BUILDDIR,"main_cui/Release/Tsugaru_CUI"+ExeExtension())
	if os.path.isfile(f):
		return f
	throw



def Run(argv):
	subprocess.Popen([
		TsugaruExe(),
		ROMDIR,
		"-SYM",
		os.path.join(THISDIR,"../symtables/RUN"+TOWNSTYPE+".txt"),
		"-HD0",
		os.path.join(DISKDIR,"hddimage.bin"),
		"-CMOS",
		os.path.join(THISDIR,"../testdata/CMOS.bin"),
		"-DONTAUTOSAVECMOS",
		#"-DEBUG",
		#"-PAUSE",
	]+argv).wait()



if __name__=="__main__":
	build.Run()
	Run(sys.argv[1:])
