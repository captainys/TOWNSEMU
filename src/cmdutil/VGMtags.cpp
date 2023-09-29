#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>

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

bool WriteBinaryFile(std::string fileName,const std::vector <unsigned char> &data)
{
	std::ofstream ofp(fileName,std::ios::binary);
	if(true==ofp.is_open())
	{
		ofp.write((char *)data.data(),data.size());
		return true;
	}
	return false;
}


class CommandParameterInfo
{
public:
	class TagValue
	{
	public:
		unsigned int tagId;
		std::string value;
	};
	std::string iVgmFileName,oVgmFileName;
	std::vector <TagValue> tagValueSet;

	bool RecognizeCommandParameter(int argc,char *argv[]);
	void PrintHelp(void) const;
	bool NeedEdit(void) const;
};
bool CommandParameterInfo::RecognizeCommandParameter(int argc,char *argv[])
{
	if(argc<2)
	{
		std::cout << "Too few arguments." << std::endl;
		std::cout << "VMTag -h for help." << std::endl;
		return false;
	}
	iVgmFileName=argv[1];
	for(int i=1; i<argc; ++i)
	{
		std::string opt=argv[i];
		for(auto &c : opt)
		{
			c=std::toupper(c);
		}
		if("-HELP"==opt || "-H"==opt)
		{
			PrintHelp();
			return false;
		}
		else if(("-T"==opt || "-TAG"==opt) && i+2<argc)
		{
			std::string tag=argv[i+1];
			for(auto &c : tag)
			{
				c=std::toupper(c);
			}
			TagValue tv;
			tv.tagId=VGMRecorder::StrToTagId(tag);
			if(VGMRecorder::GD3_UNDEFINED==tv.tagId)
			{
				std::cout << "Undefined tag." << std::endl;
				return false;
			}
			tv.value=argv[i+2];
			tagValueSet.push_back(tv);
			i+=2;
		}
		else if(("-O"==opt || "-OUT"==opt) && i+1<argc)
		{
			oVgmFileName=argv[i+1];
			++i;
		}
		else
		{
			if(1==i)
			{
				iVgmFileName=argv[i];
			}
			else
			{
				std::cout << "Unrecognized or insufficient arguments." << std::endl;
				return false;
			}
		}
	}

	if(0<tagValueSet.size() && ""==oVgmFileName)
	{
		std::cout << "Output file name is not specified although -T tags are given." << std::endl;
		return false;
	}
	return true;
}
void CommandParameterInfo::PrintHelp(void) const
{
	std::cout << "Usage:" << std::endl;
	std::cout << "  VGMtag inputVgmFileName.vgm [options]" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "  -t TagType TagValue" << std::endl;
	std::cout << "  -tag TagType TagValue" << std::endl;
	std::cout << "    Set value to a tag.  -t and -tag have the same meaning." << std::endl;
	std::cout << "    TagType can be one of:" << std::endl;
	for(int id=0; id<VGMRecorder::GD3_UNDEFINED; ++id)
	{
		std::cout << "      " << VGMRecorder::TagIdToStr(id) << std::endl;
	}
	std::cout << "  -o outputVgmFileName.vgm" << std::endl;
	std::cout << "  -out outputVgmFileName.vgm" << std::endl;
	std::cout << "    Specify output .vgm file name." << std::endl;
	std::cout << "    -o and -out have the same meaning." << std::endl;
}
bool CommandParameterInfo::NeedEdit(void) const
{
	return 0<tagValueSet.size();
}

int main(int ac,char *av[])
{
	CommandParameterInfo cpi;
	if(true!=cpi.RecognizeCommandParameter(ac,av))
	{
		return 1;
	}

	auto data=ReadBinaryFile(cpi.iVgmFileName);

	auto GD3tag=VGMRecorder::GetGD3Tag(data);
	if(0<GD3tag.size())
	{
		auto tags=VGMRecorder::ExtractGD3Tags(GD3tag);
		int id=0;
		for(auto s : tags)
		{
			std::cout << VGMRecorder::TagIdToStr(id) << ":";
			std::cout << s << std::endl;
			++id;
		}

		if(true==cpi.NeedEdit())
		{
			if(true!=VGMRecorder::RemoveGD3Tag(data))
			{
				std::cout << "Cannot remove GD3 tag." << std::endl;
				std::cout << "Most likely GD3 tag is not at the end of the VGM file." << std::endl;
				return 1;
			}
		}
	}
	else
	{
		std::cout << "Input VGM does not have GD3 tag information." << std::endl;
		GD3tag=VGMRecorder::MakeEmptyGD3Tag();
	}

	for(auto tv : cpi.tagValueSet)
	{
		VGMRecorder::ClearTagItem(GD3tag,tv.tagId);
		VGMRecorder::InsertTagItem(GD3tag,tv.tagId,tv.value);
		VGMRecorder::UpdateGD3Size(GD3tag);
	}

	int id=0;
	for(auto s : VGMRecorder::ExtractGD3Tags(GD3tag))
	{
		std::cout << VGMRecorder::TagIdToStr(id) << ":";
		std::cout << s << std::endl;
		++id;
	}

	return 0;
}
