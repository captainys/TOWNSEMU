import os
import subprocess
import shutil
import sys
import sys

THISFILE=os.path.realpath(__file__)
THISDIR=os.path.dirname(THISFILE)
TSUGARUDIR=os.path.join(THISDIR,"..")

BUILDDIR=os.path.join(TSUGARUDIR,"build")

BUILDDIR_TEST386=os.path.join(TSUGARUDIR,"test386vm","build")



def CMakeBuild():
	return ["cmake","--build",".","--config","-Release","--parallel"]



def CTest():
	return ["ctest","-C","Release"]



def Build():
	proc=[]
	os.chdir(BUILDDIR_TEST386)
	proc.append(subprocess.Popen(CMakeBuild()))
	os.chdir(BUILDDIR)
	proc.append(subprocess.Popen(CMakeBuild()))

	for p in proc:
		p.communicate()

	for p in proc:
		if 0!=p.returncode:
			print("Build Failed.")
			quit()

	print("Build Succeeded.")



def Test():
	proc=[]
	os.chdir(BUILDDIR_TEST386)
	proc.append(subprocess.Popen(CTest()))
	os.chdir(BUILDDIR)
	proc.append(subprocess.Popen(CTest()))

	for p in proc:
		p.communicate()

	for p in proc:
		if 0!=p.returncode:
			print("Build Failed.")
			quit()

	print("Test Succeeded.")



def main(argv):
	Build()
	Test()



if __name__=="__main__":
	main(sys.argv[1:])
