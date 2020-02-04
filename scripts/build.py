import os
import subprocess
import shutil

THISFILE=os.path.realpath(__file__)
THISDIR=os.path.dirname(THISFILE)
BUILDDIR=os.path.join(THISDIR,"..","build")
SRCDIR=os.path.join(THISDIR,"..","src")
ROMDIR=os.path.join(THISDIR,"..","testdata","ROM_MX")
DISKDIR=os.path.join(THISDIR,"..","testdata","DISKIMG")



def Run():
	os.chdir(BUILDDIR)
	subprocess.Popen([
		"nmake",
	]).wait()



if __name__=="__main__":
	Run()
