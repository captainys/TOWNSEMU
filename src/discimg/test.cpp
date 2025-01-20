#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <discimg.h>



class CommandParameterInfo
{
public:
	std::vector <std::string> images;
	std::string TOCSample;

	bool Recognize(int ac,char *av[]);
	void PrintHelp(void) const;
};
bool CommandParameterInfo::Recognize(int ac,char *av[])
{
	for(int i=1; i<ac; ++i)
	{
		std::string opt=av[i];
		if("-help"==opt || "-h"==opt || "-?"==opt)
		{
			PrintHelp();
			return false;
		}
		else if("-cd"==opt && i+1<ac)
		{
			images.push_back(av[i+1]);
			++i;
		}
		else if("-sample"==opt && i+1<ac)
		{
			TOCSample=av[i+1];
			++i;
		}
		else
		{
			std::cout << "Unknown option or missing arguments.\n";
			return false;
		}
	}
	return true;
}
void CommandParameterInfo::PrintHelp(void) const
{
	std::cout << "Options:\n";
	std::cout << "-help,-h,-?\n";
	std::cout << "  Print this help.\n";
	std::cout << "-cd filename.cue/mds\n";
	std::cout << "  Specify a cd image.\n";
	std::cout << "-sample filename.bin\n";
	std::cout << "  TOC Sample from real FM TOWNS.\n";
}

int main(int ac,char *av[])
{
	CommandParameterInfo cpi;
	if(true!=cpi.Recognize(ac,av))
	{
		return 1;
	}

	std::vector <DiscImage> discs;
	discs.resize(cpi.images.size());
	for(int i=0; i<cpi.images.size(); ++i)
	{
		if(DiscImage::ERROR_NOERROR!=discs[i].Open(cpi.images[i]))
		{
			std::cout << "Cannot open " << cpi.images[i] << "\n";
			return 1;
		}
	}

	std::vector <unsigned char> TOCSample;
	if(""!=cpi.TOCSample)
	{
		std::ifstream ifp(cpi.TOCSample,std::ios::binary);
		ifp.seekg(0,std::ios::end);
		auto size=ifp.tellg();
		ifp.seekg(0,std::ios::beg);
		TOCSample.resize(size);
		ifp.read((char *)TOCSample.data(),size);
	}


	for(int i=1; i<discs.size(); ++i)
	{
		std::cout << "Comparing:\n";
		std::cout << "  " << cpi.images[0] << "\n";
		std::cout << "and:\n";
		std::cout << "  " << cpi.images[i] << "\n";

		if(discs[0].GetNumSectors()!=discs[i].GetNumSectors())
		{
			std::cout << "Number of sectors of do not match.\n";
			std::cout << discs[0].GetNumSectors() << "!=" << discs[i].GetNumSectors() << "\n";
			return 1;
		}

		auto &tracks0=discs[0].GetTracks();
		auto &tracks1=discs[i].GetTracks();

		if(tracks0.size()!=tracks1.size())
		{
			std::cout << "Number of tracks of do not match.\n";
			return 1;
		}

		for(int j=0; j<tracks0.size(); ++j)
		{
			if(tracks0[j].start!=tracks1[j].start)
			{
				std::cout << "Track " << j << " start time do not match.\n";
				std::cout << "  " << tracks0[j].start.min << ":";
				std::cout << "  " << tracks0[j].start.sec << ":";
				std::cout << "  " << tracks0[j].start.frm << "!=";
				std::cout << "  " << tracks1[j].start.min << ":";
				std::cout << "  " << tracks1[j].start.sec << ":";
				std::cout << "  " << tracks1[j].start.frm << "\n";
				return 1;
			}
		}
	}

	std::cout << "Comparison finished with no error.\n";

	return 0;
}
