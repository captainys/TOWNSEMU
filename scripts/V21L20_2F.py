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



def Run(argv):
	os.chdir(BUILDDIR)
	subprocess.Popen([
		"./main_cui/main_cui.exe",
		ROMDIR,
		"-FD0",
		os.path.join(DISKDIR,"V2.1L20.bin"),
		"-CD",
		os.path.join(THISDIR,"..","testdata","ISOIMG","TESTDISC.ISO"),
		"-SYM",
		"../symtables/V2.1L20_"+TOWNSTYPE+".txt",
		"-DEBUG",
		"-PAUSE",
	]+argv).wait()



if __name__=="__main__":
	build.Run()
	Run(sys.argv[1:])
