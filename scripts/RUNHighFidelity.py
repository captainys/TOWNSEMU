import os
import subprocess
import shutil
import sys
import sys

TOWNSTYPE="MX"

THISFILE=os.path.realpath(__file__)
THISDIR=os.path.dirname(THISFILE)
TSUGARUDIR=os.path.join(THISDIR,"..")

BUILDDIR=os.path.join(TSUGARUDIR,"build")
SRCDIR=os.path.join(TSUGARUDIR,"src")
ROMDIR=os.path.join(TSUGARUDIR,"..","TOWNSEMU_TEST","ROM_"+TOWNSTYPE)
DISKDIR=os.path.join(TSUGARUDIR,"..","TOWNSEMU_TEST","DISKIMG")
MEMCARDDIR=os.path.join(TSUGARUDIR,"..","TOWNSEMU_TEST","MEMCARD")



def ExeExtension():
	if sys.platform.startswith('win'):
		return ".exe"
	else:
		return ""



def TsugaruExe():
	fName=os.path.join(BUILDDIR,"main_cui","Tsugaru_CUI"+ExeExtension())
	if os.path.isfile(fName):
		return fName
	fName=os.path.join(BUILDDIR,"main_cui","Release","Tsugaru_CUI"+ExeExtension())
	if os.path.isfile(fName):
		return fName
	fName=os.path.join(BUILDDIR,"main_cui","Tsugaru_CUI.app","Contents","MacOS","Tsugaru_CUI"+ExeExtension())
	if os.path.isfile(fName):
		return fName
	throw



def Build():
	proc=subprocess.Popen([
		"cmake",
		"--build",
		BUILDDIR,
		"--config",
		"Release",
		"--parallel",])
	proc.communicate()
	if 0!=proc.returncode:
		print("Build Error!")
		quit()



def Run(argv):
	subprocess.Popen([
		TsugaruExe(),
		ROMDIR,
		"-HIGHFIDELITY",
		"-SYM",
		os.path.join(TSUGARUDIR,"symtables","RUN"+TOWNSTYPE+"_HF.txt"),
		"-HD0",
		os.path.join(DISKDIR,"hddimage.bin"),
		"-HD1",
		os.path.join(DISKDIR,"40MB.h1"),
		"-JEIDA4",
		os.path.join(MEMCARDDIR,"4MB.bin"),
		"-CMOS",
		os.path.join(TSUGARUDIR,"testdata","CMOS.bin"),
		"-DONTAUTOSAVECMOS",
		"-USEFPU",
		"-HIGHRES",
		"-DEBUG",
		#"-PAUSE",
	]+argv).wait()



if __name__=="__main__":
	Build()
	Run(sys.argv[1:])
