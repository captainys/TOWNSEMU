#include <stdio.h>

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
		return 1;
	}
	if(NULL==fgets(buf,255,fp))
	{
		fprintf(stderr,"Cannot read a line from Q:/TEXT.TXT\n");
		return 1;
	}
	if(0!=strncmp(buf,"FM Towns Emulator TSUGARU.",26))
	{
		fprintf(stderr,"Not getting file content from Q:/TEXT.TXT\n");
		return 1;
	}
	fclose(fp);

	fp=fopen("Q:/BINARY.BIN","rb");
	if(NULL==fp)
	{
		fprintf(stderr,"Cannot Open Q:/BINARY.BIN\n");
		return 1;
	}
	if(33!=fread(ubuf,1,256,fp))
	{
		fprintf(stderr,"Cannot read 33 bytes from Q:/BINARY.BIN\n");
		return 1;
	}
	for(int i=0; i<33; ++i)
	{
		if(binaryData[i]!=ubuf[i])
		{
			fprintf(stderr,"Not getting file content from Q:/BINARY.BIN\n");
			return 1;
		}
	}
	fclose(fp);

	return 0;
}
