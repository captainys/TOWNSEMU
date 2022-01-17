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



def ExeExtension():
	if sys.platform.startswith('win'):
		return ".exe"
	else:
		return ""



def TsugaruExe():
	if os.path.isfile("./main_cui/Tsugaru_CUI"+ExeExtension()):
		return "./main_cui/Tsugaru_CUI"+ExeExtension()
	if os.path.isfile("./main_cui/Release/Tsugaru_CUI"+ExeExtension()):
		return "./main_cui/Release/Tsugaru_CUI"+ExeExtension()
	throw



def Run(argv):
	os.chdir(BUILDDIR)
	subprocess.Popen([
		TsugaruExe(),
		ROMDIR,
		"-FD0",
		os.path.join(DISKDIR,"V2.1L20.bin"),
		"-CD",
		"C:/D/TownsISO/FB386CV2.1L10.iso",
		"-SYM",
		"../symtables/FB386CV2.1L10_"+TOWNSTYPE+".txt",
		"-DEBUG",
		"-HD0",
		os.path.join(DISKDIR,"hddimage.bin"),
		"-CMOS",
		"../testdata/CMOS.bin",
		"-DONTAUTOSAVECMOS",
		# "-PAUSE",
		"-USEFPU",
	]+argv).wait()



if __name__=="__main__":
	build.Run()
	Run(sys.argv[1:])
