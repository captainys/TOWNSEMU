import os
import subprocess
import shutil

THISFILE=os.path.realpath(__file__)
THISDIR=os.path.dirname(THISFILE)
BUILDDIR=os.path.join(THISDIR,"..","build")
SRCDIR=os.path.join(THISDIR,"..","src")


def Run():
	if not os.path.isdir(BUILDDIR):
		os.makedirs(BUILDDIR)


	os.chdir(BUILDDIR)
	subprocess.Popen(
		[
			"cmake",
			SRCDIR,
			"-G",
			"NMake Makefiles",
			"-DCMAKE_BUILD_TYPE=Release"
		]).wait();



if __name__=="__main__":
	Run()
