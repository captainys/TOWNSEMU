import os
import subprocess
import shutil
import sys

import build

THISFILE=os.path.realpath(__file__)
THISDIR=os.path.dirname(THISFILE)
BUILDDIR=os.path.join(THISDIR,"..","build")
SRCDIR=os.path.join(THISDIR,"..","src")
ROMDIR=os.path.join(THISDIR,"..","..","TOWNSEMU_TEST","ROM_2F")
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
		os.path.join(DISKDIR,"testset01.bin"),
		"-SYM",
		"../symtables/TESTSET01.txt",
		"-UNITTEST",
	]).wait()



if __name__=="__main__":
	build.Run()
	Run()
