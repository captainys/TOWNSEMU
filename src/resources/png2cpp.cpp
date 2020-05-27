#include "yspng.h"
#include "cpputil.h"

#include <iostream>
#include <fstream>
#include <string>

// 16x16 icons to C++ source.

std::string files[]=
{
	"CD_BUSY",
	"CD_IDLE",
	"FD_BUSY",
	"FD_IDLE",
	"HDD_BUSY",
	"HDD_IDLE",
};

int main(void)
{
	// Check all of them are 16x16
	for(auto fName : files)
	{
		fName+=".png";
		std::cout << "Checking:"<<fName<<std::endl;

		YsRawPngDecoder png;
		png.Decode(fName.c_str());
		if(png.wid!=16 || png.hei!=16)
		{
			std::cout << fName << " is not 16x16." << std::endl;
			return 1;
		}
	}

	// Save as .cpp
	std::ofstream ofs;
	ofs.open("../towns/outside_world/icons.cpp");
	for(auto fName : files)
	{
		YsRawPngDecoder png;
		fName+=".png";
		png.Decode(fName.c_str());

		ofs << "const unsigned char " << fName << "[]={" << std::endl;

		for(int i=0; i<png.wid*png.hei; ++i)
		{
			ofs<<"0x"<<cpputil::Ubtox(png.rgba[i*4  ])<<",";
			ofs<<"0x"<<cpputil::Ubtox(png.rgba[i*4+1])<<",";
			ofs<<"0x"<<cpputil::Ubtox(png.rgba[i*4+2])<<",";
			ofs<<"0x"<<cpputil::Ubtox(png.rgba[i*4+3])<<",";
			if((i+1)%8==0 || i+1==png.wid*png.hei)
			{
				ofs<<std::endl;
			}
		}

		ofs << "};" << std::endl;
	
	}
	ofs.close();

	return 0;
}
