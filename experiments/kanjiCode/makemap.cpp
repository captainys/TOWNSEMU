#include <iostream>
#include <fstream>
#include <cstdio>



/*
FM Towns's fonts are stored in a weird order.
Not sure what exactly it is.
But, this is the function that converts JIS code to the Towns's own code based on the reverse engineering of the font ROM.
Multiply 32 bytes to get to the font pattern from the beginning of the font ROM.

Also in the font ROM:
  +0x3D000  8x8 ANK Font (8 bytes times 256)
  +0x3D800  8x16 ANK Font (16 bytes times 256)
*/
inline unsigned int FontROMCode(unsigned int JISCode)
{
	auto JISCodeHigh=(JISCode>>8)&255;
	auto JISCodeLow=JISCode&255;
	if(JISCodeHigh<0x28)
	{
		// 32x8 Blocks
		unsigned int BLK=(JISCodeLow-0x20)>>5;
		unsigned int x=JISCodeLow&0x1F;
		unsigned int y=JISCodeHigh&7;
		if(BLK==1)
		{
			BLK=2;
		}
		else if(BLK==2)
		{
			BLK=1;
		}
		return BLK*32*8+y*32+x;
	}
	else
	{
		// 32x16 Blocks;
		unsigned int BlkX=(JISCodeLow-0x20)>>5;
		unsigned int BlkY=(JISCodeHigh-0x30)>>4;
		unsigned int BLK=BlkY*3+BlkX;
		unsigned int x=JISCodeLow&0x1F;
		unsigned int y=JISCodeHigh&0x0F;
		return 0x400+BLK*32*16+y*32+x;
	}
}

unsigned int xtoi(const char txt[])
{
	unsigned int i=0;
	for(;;)
	{
		unsigned int c;
		if('0'<=*txt && *txt<='9')
		{
			c=(*txt)-'0';
		}
		else if('A'<=*txt && *txt<='F')
		{
			c=(*txt)-'A'+10;
		}
		else if('a'<=*txt && *txt<='f')
		{
			c=(*txt)-'a'+10;
		}
		else
		{
			break;
		}
		i*=16;
		i+=c;
		++txt;
	}
	return i;
}

// 32 00 31 00 32 00 31 00 00 30 0A 00
// ^^^^^^^^^^^^^^^^^^^^^^^       ^^^^^
// JIS Code Hexadecimal          Linebreak
//                         ^^^^^
//                     UNICODE UTF16 (Little Endian)

int main(void)
{
	std::ifstream ifp("UTF16.txt",std::ios::binary);
	std::ofstream ofp("map.cpp");
	if(true==ifp.is_open())
	{
		ofp << "unsigned int codeTable[][3]={" << std::endl;
		ofp << "// {JISCode,UNICODE,TownsCode}" << std::endl;
		while(true!=ifp.eof())
		{
			char buf[12];
			ifp.read(buf,12);

			char JISCodeTxt[5]=
			{
				buf[0],
				buf[2],
				buf[4],
				buf[6],
				0
			};
			auto JISCode=xtoi(JISCodeTxt);

			unsigned int highByte=buf[9];
			unsigned int lowByte=buf[8];
			highByte&=255;
			lowByte&=255;
			unsigned int unicode=(highByte<<8)|lowByte;
			unicode&=0xffff;

			unsigned int townsCode=FontROMCode(JISCode)&0xffff;

			if(' '==unicode || (0x2D00<=JISCode && JISCode<0x3000) || 0x7500<=JISCode)
			{
				continue;
			}
			if(0x2000<=townsCode)
			{
				continue;
			}

			char str[80];
			sprintf(str,"{0x%04x,0x%04x,0x%04x},",JISCode,unicode,townsCode);
			ofp << str << std::endl;
		}
		ofp << "};" << std::endl;
	}
	return 0;
}
