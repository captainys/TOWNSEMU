#include <iostream>
#include <fstream>
#include "fssimplewindow.h"
#include "yssystemfont.h"


#include "map.cpp"


unsigned char fontROM[256*1024];

int main(int ac,char *av[])
{
	if(2<=ac)
	{
		std::ifstream ifp(av[1],std::ios::binary);
		if(true==ifp.is_open())
		{
			ifp.read((char *)fontROM,sizeof(fontROM));
		}
		else
		{
			std::cout << "Failed to open fontROM." << std::endl;
			return 1;
		}
	}

	int codePtr=0;
	const int codeMax=sizeof(codeTable)/sizeof(codeTable[0]);

	printf("JIS:%04x  UNICODE:%04x  Towns:%04x\n",codeTable[codePtr][0],codeTable[codePtr][1],codeTable[codePtr][2]);

	FsOpenWindow(0,0,800,600,1);

	YsSystemFontCache fontCache;
	fontCache.RequestDefaultFontWithHeight(16);

	for(;;)
	{
		FsPollDevice();
		auto key=FsInkey();
		if(FSKEY_ESC==key)
		{
			break;
		}

		const auto prev=codePtr;
		int delta=1;
		if(FsGetKeyState(FSKEY_SHIFT))
		{
			delta=256;
		}
		switch(key)
		{
		case FSKEY_UP:
			codePtr+=delta;
			if(codeMax<=codePtr)
			{
				codePtr=codeMax-1;
			}
			break;
		case FSKEY_DOWN:
			codePtr-=delta;
			if(codePtr<0)
			{
				codePtr=0;
			}
			break;
		}
		if(prev!=codePtr)
		{
			printf("JIS:%04x  UNICODE:%04x  Towns:%04x\n",codeTable[codePtr][0],codeTable[codePtr][1],codeTable[codePtr][2]);
		}

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		static unsigned char data[64];
		for(int i=0; i<64; ++i)
		{
			data[i]=0;
		}
		auto townsCode=codeTable[codePtr][2];
		for(int y=0; y<16; ++y)
		{
			data[(15-y)*4  ]=fontROM[townsCode*32+y*2];
			data[(15-y)*4+1]=fontROM[townsCode*32+y*2+1];
		}
		glRasterPos2i(0,16);
		glBitmap(16,16,0,0,0,0,data);

		wchar_t uni[2]={codeTable[codePtr][1],0};
		auto sysFontBmp=fontCache.MakeSingleBitBitmap(uni,YSTRUE);

		glRasterPos2i(32,16);
		glBitmap(sysFontBmp.Width(),sysFontBmp.Height(),0,0,0,0,sysFontBmp.Bitmap());

		FsSwapBuffers();
	}

	return 0;
}
