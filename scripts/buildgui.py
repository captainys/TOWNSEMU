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
	if True!=os.path.isdir(PUBLICSRCDIR):
		os.chdir(SRCDIR)
		proc=subprocess.Popen([
			"git","clone","https://github.com/captainys/public.git"
		])
		proc.communicate();
		if proc.returncode!=0:
			print("Build Error! (git clone)")
			quit()

	os.chdir(PUBLICSRCDIR)
	proc=subprocess.Popen([
		"git","pull"
	])
	proc.communicate();
	if proc.returncode!=0:
		print("Build Error! (git pull)")
		quit()

	if True!=os.path.isdir(BUILDDIR):
		os.mkdir(BUILDDIR)

	if True!=os.path.isfile(os.path.join(BUILDDIR,"CMakeCache.txt")):
		os.chdir(BUILDDIR)
		proc=subprocess.Popen([
			"cmake","../src","-DCMAKE_BUILD_TYPE=Release"
		])
		proc.communicate();
		if proc.returncode!=0:
			print("CMake Error!")
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
