#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "vgmrecorder.h"



// ....  I've been trying to get used to C++ file I/O.  I still feel it easier
// to do with C FILE functions.
std::vector <unsigned char> ReadBinaryFile(std::string fileName)
{
	std::vector <unsigned char> data;
	std::ifstream ifp(fileName,std::ios::binary);
	if(true==ifp.is_open())
	{
		ifp.seekg(0,std::ios::end);
		auto size=ifp.tellg();

		data.resize(size);

		ifp.seekg(std::ios::beg);
		ifp.read((char *)data.data(),size);
	}
	return data;
}



int main(int ac,char *av[])
{
	if(ac<2)
	{
		return 1;
	}

	auto data=ReadBinaryFile(av[1]);

	auto GD3tag=VGMRecorder::GetGD3Tag(data);
	if(0<GD3tag.size())
	{
		return 0;
	}
	else
	{
		std::cout << "Cannot extract GD3 tag." << std::endl;
	}

	return 0;
}
