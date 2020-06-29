import os
import subprocess
import shutil
import sys

THISFILE=os.path.realpath(__file__)
THISDIR=os.path.dirname(THISFILE)
BUILDDIR=os.path.join(THISDIR,"..","gui","build")
SRCDIR=os.path.join(THISDIR,"..","gui","src")
PUBLICSRCDIR=os.path.join(THISDIR,"..","gui","src","public")



def Run():
	os.chdir(PUBLICSRCDIR)
	proc=subprocess.Popen([
		"git","pull"
	])
	proc.communicate();
	if proc.returncode!=0:
		print("Build Error! (git pull)")
		quit()

	os.chdir(BUILDDIR)
	proc=subprocess.Popen([
		"cmake","--build",".","--config","Release","--target","Tsugaru_GUI"
	])
	proc.communicate();
	if proc.returncode!=0:
		print("Build Error!")
		quit()



if __name__=="__main__":
	Run()
