import os
import subprocess
import shutil

import build

THISFILE=os.path.realpath(__file__)
THISDIR=os.path.dirname(THISFILE)
BUILDDIR=os.path.join(THISDIR,"..","build")
SRCDIR=os.path.join(THISDIR,"..","src")
ROMDIR=os.path.join(THISDIR,"..","..","TOWNSEMU_TEST","ROM_MX")
DISKDIR=os.path.join(THISDIR,"..","..","TOWNSEMU_TEST","DISKIMG")



def Run():
	os.chdir(BUILDDIR)
	subprocess.Popen([
		"./main_cui/main_cui.exe",
		ROMDIR,
		"-FD0",
		os.path.join(DISKDIR,"testset02.bin"),
		"-SYM",
		"../symtables/TESTSET02.txt",
		"-UNITTEST",
	]).wait()



if __name__=="__main__":
	build.Run()
	Run()
