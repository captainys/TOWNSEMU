import sys
import os



license=[
"/* LICENSE>>\n",

"Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)\n",
"\n",
"Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:\n",
"\n",
"1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.\n",
"\n",
"2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.\n",
"\n",
"3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.\n",
"\n",
'THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n',
"\n",
"<< LICENSE */\n",
]




def ReadFile(fName):
	text=[]
	fp=open(fName,"r")
	for s in fp:
		text.append(s)
	return text



def WriteFile(fName,text):
	fp=open(fName,"w")
	for t in text:
		fp.write(t)



def RemoveLicenseLines(text):
	if text[0].startswith("/* LICENSE>>"):
		ripped=[]
		foundEnd=False
		for s in text:
			if True==foundEnd:
				ripped.append(s)
			if s.startswith("<< LICENSE */"):
				foundEnd=True
		if True==foundEnd:
			return ripped
	return text;



def CheckAndAddLicense(fName):
	text=ReadFile(fName)
	ripped=RemoveLicenseLines(text);
	WriteFile(fName,license+ripped)



def DoRecursive(dirName):
	for fName in os.listdir(dirName):
		if fName.startswith(".git"):
			continue

		ful=os.path.join(dirName,fName)
		if os.path.isdir(ful):
			DoRecursive(ful)

		FNAME=fName.upper()
		if FNAME.endswith(".C") or FNAME.endswith(".H") or FNAME.endswith(".CPP"):
			print("*"+ful)
			CheckAndAddLicense(ful)


def main(argv):
	DoRecursive(".")



if __name__=='__main__':
	main(sys.argv)

