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


def Run(argv):
	os.chdir(BUILDGUIDIR)
	subprocess.Popen([
		"./main_gui/Tsugaru_GUI"+ExeExtension()
	]+argv).wait()



if __name__=="__main__":
	build.Run()
	buildgui.Run()
	shutil.copyfile(
		os.path.join(BUILDDIR,"main_cui","Tsugaru_CUI"+ExeExtension()),
		os.path.join(BUILDGUIDIR,"main_gui","Tsugaru_CUI"+ExeExtension())
	)
	Run(sys.argv[1:])
