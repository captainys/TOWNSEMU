import os
import subprocess
import shutil
import sys
import sys

import build
import buildgui

THISFILE=os.path.realpath(__file__)
THISDIR=os.path.dirname(THISFILE)
BUILDDIR=os.path.join(THISDIR,"..","build")
BUILDGUIDIR=os.path.join(THISDIR,"..","gui","build")
SRCDIR=os.path.join(THISDIR,"..","src")



def ExeExtension():
	if sys.platform.startswith('win'):
		return ".exe"
	else:
		return ""


def TsugaruCUIExe():
	relpath=os.path.join(BUILDDIR,"main_cui","Tsugaru_CUI"+ExeExtension())
	if os.path.isfile(relpath):
		return relpath;
	relpath=os.path.join(BUILDDIR,"main_cui","Release","Tsugaru_CUI"+ExeExtension())
	if os.path.isfile(relpath):
		return relpath;
	raise


def GUISubdir():
	subdir=os.path.join(BUILDGUIDIR,"main_gui")
	ful=os.path.join(subdir,"Tsugaru_GUI"+ExeExtension())
	if os.path.isfile(ful):
		return subdir;
	subdir=os.path.join(BUILDGUIDIR,"main_gui","Release")
	ful=os.path.join(subdir,"Tsugaru_GUI"+ExeExtension())
	if os.path.isfile(ful):
		return subdir;
	raise


def Run(argv):
	os.chdir(BUILDGUIDIR)
	subprocess.Popen([
		os.path.join(GUISubdir(),"Tsugaru_GUI"+ExeExtension())
	]+argv).wait()



if __name__=="__main__":
	build.Run()
	buildgui.Run()
	shutil.copyfile(
		TsugaruCUIExe(),
		os.path.join(GUISubdir(),"Tsugaru_CUI"+ExeExtension())
	)
	Run(sys.argv[1:])
