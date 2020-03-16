/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <stdio.h>

extern void TEST_FAILED(void);

int main(void)
{
	FILE *fp;
	char buf[256];
	unsigned char ubuf[256];
	const unsigned char binaryData[]=
	{
		0x0B,0x00,0xFF,0x01,0xFF,0x02,0xFF,0x03,0xFF,0x04,0xFF,0x05,0xFF,0x06,0xFF,0x07,
		0xFF,0x08,0xFF,0x09,0xFF,0x0A,0xFF,0x0B,0xFF,0x0C,0xFF,0x0D,0xFF,0x0E,0xFF,0x0F,
		0xFF
	};

	fp=fopen("Q:/TEXT.TXT","r");
	if(NULL==fp)
	{
		fprintf(stderr,"Cannot Open Q:/TEXT.TXT\n");
		TEST_FAILED();
		return 1;
	}
	if(NULL==fgets(buf,255,fp))
	{
		fprintf(stderr,"Cannot read a line from Q:/TEXT.TXT\n");
		TEST_FAILED();
		return 1;
	}
	if(0!=strncmp(buf,"FM Towns Emulator TSUGARU.",26))
	{
		fprintf(stderr,"Not getting file content from Q:/TEXT.TXT\n");
		TEST_FAILED();
		return 1;
	}
	fclose(fp);

	fp=fopen("Q:/BINARY.BIN","rb");
	if(NULL==fp)
	{
		fprintf(stderr,"Cannot Open Q:/BINARY.BIN\n");
		TEST_FAILED();
		return 1;
	}
	if(33!=fread(ubuf,1,256,fp))
	{
		fprintf(stderr,"Cannot read 33 bytes from Q:/BINARY.BIN\n");
		TEST_FAILED();
		return 1;
	}
	for(int i=0; i<33; ++i)
	{
		if(binaryData[i]!=ubuf[i])
		{
			fprintf(stderr,"Not getting file content from Q:/BINARY.BIN\n");
			TEST_FAILED();
			return 1;
		}
	}
	fclose(fp);

	return 0;
}
