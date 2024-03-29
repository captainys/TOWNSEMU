#include <cctype>
#include "vgmrecorder.h"

void VGMRecorder::CleanUp(void)
{
	log.clear();
	memWrite.clear();
}

void VGMRecorder::WriteRegister(uint64_t VMTime,unsigned char target,unsigned int reg,unsigned char value)
{
	RegWriteLog newLog;
	newLog.VMTimeInNanosec=VMTime;
	newLog.target=target;
	newLog.reg=reg;
	newLog.value=value;
	log.push_back(newLog);
}

void VGMRecorder::WritePCMMemory(uint64_t VMTime,unsigned char target,unsigned int address,unsigned char value)
{
	if(0<log.size() && MEM_RF5C68==log.back().target && log.back().reg<memWrite.size())
	{
		auto &lastMemWriteChunk=memWrite[log.back().reg];
		if(address==lastMemWriteChunk.address+lastMemWriteChunk.data.size())
		{
			lastMemWriteChunk.data.push_back(value);
			return;
		}
	}

	RegWriteLog newLog;
	newLog.VMTimeInNanosec=VMTime;
	newLog.target=target;
	newLog.reg=(unsigned int)memWrite.size();
	newLog.value=0;
	log.push_back(newLog);

	MemoryWriteChunk chunk;
	chunk.address=address;
	chunk.data.push_back(value);
	memWrite.push_back(chunk);
}

void VGMRecorder::WritePCMMemoryBlock(uint64_t VMTime,unsigned char target,unsigned int address,unsigned int size,const unsigned char data[])
{
	RegWriteLog newLog;
	newLog.VMTimeInNanosec=VMTime;
	newLog.target=target;
	newLog.reg=(unsigned int)memWrite.size();
	newLog.value=0;
	log.push_back(newLog);

	MemoryWriteChunk chunk;
	chunk.address=address;
	for(unsigned int i=0; i<size; ++i)
	{
		chunk.data.push_back(data[i]);
	}
	memWrite.push_back(chunk);
}

std::vector <unsigned char> VGMRecorder::Encode(void) const
{
	std::vector <unsigned char> vgm;
	std::vector <unsigned char> extHeader;

	if(0<YM2203clock)
	{
		extHeader=GenerateExtraHeaderForYM2203CVolumeProblem();
	}

	vgm.resize(256);
	for(auto &d : vgm)
	{
		d=0;
	}

	vgm[0]=0x56; // "Vgm " signature.
	vgm[1]=0x67;
	vgm[2]=0x6d;
	vgm[3]=0x20;

	WriteUint(vgm.data()+VGM_OFFSET_VERSION,0x0171); // Version

	WriteUint(vgm.data()+VGM_OFFSET_RATE,VGM_RATE_NTSC); // Rate

	WriteUint(vgm.data()+VGM_OFFSET_YM2612CLK,YM2612clock);

	WriteUint(vgm.data()+VGM_OFFSET_STREAMOFFSET,(VGM_HEADER_LENGTH+extHeader.size()-VGM_OFFSET_STREAMOFFSET)); // Relative offset to VGM data stream.

	WriteUint(vgm.data()+VGM_OFFSET_RF5C68CLK,RF5C68clock);

	WriteUint(vgm.data()+VGM_OFFSET_YM2203CLK,YM2203clock);

	WriteUint(vgm.data()+VGM_OFFSET_AY8910CLK,AY8910clock);
	vgm[VGM_OFFSET_AY8910TYPE]=0; // AY8910
	vgm[VGM_OFFSET_AY8910FLAGS]=(0<AY8910clock ? 1 : 0); // What's this?
	vgm[VGM_OFFSET_AY_IN_YM2203_FLAGS]=(0<YM2203clock ? 1 : 0);  // What's this?


	if(0<extHeader.size())
	{
		WriteUint(vgm.data()+VGM_OFFSET_EXTHEADER_OFFSET,vgm.size()-VGM_OFFSET_EXTHEADER_OFFSET);
		vgm.insert(vgm.end(),extHeader.begin(),extHeader.end());
	}


	unsigned int nSamples=0;
	if(0<log.size())
	{
		// Is *44100/1000000000 to get number of samples.
		// Same as 441/10000000
		uint64_t prevPtr=log[0].VMTimeInNanosec*441/10000000; // In number of samples.
		for(auto L : log)
		{
			uint64_t ptr=L.VMTimeInNanosec*441/10000000; // In number of samples.
			if(prevPtr<ptr)
			{
				auto dt=ptr-prevPtr;
				nSamples+=dt;
				while(65535<dt)
				{
					vgm.push_back(VGM_CMD_WAIT);
					vgm.push_back(0xff);
					vgm.push_back(0xff);
					dt-=65535;
				}
				if(0<dt)
				{
					vgm.push_back(VGM_CMD_WAIT);
					vgm.push_back(dt&0xff);
					vgm.push_back((dt>>8)&0xff);
				}
				prevPtr=ptr;
			}

			switch(L.target)
			{
			case REG_YM2612_CH0:
				vgm.push_back(VGM_CMD_YM2612_CH0);
				vgm.push_back(L.reg);
				vgm.push_back(L.value);
				break;
			case REG_YM2612_CH3:
				vgm.push_back(VGM_CMD_YM2612_CH3);
				vgm.push_back(L.reg);
				vgm.push_back(L.value);
				break;
			case REG_YM2203:
				vgm.push_back(VGM_CMD_YM2203);
				vgm.push_back(L.reg);
				vgm.push_back(L.value);
				break;
			case REG_AY8910:
				vgm.push_back(VGM_CMD_AY8910);
				vgm.push_back(L.reg);
				vgm.push_back(L.value);
				break;
			case REG_RF5C68:
				vgm.push_back(VGM_CMD_RF5C68);
				vgm.push_back(L.reg);
				vgm.push_back(L.value);
				break;
			case MEM_RF5C68:
				vgm.push_back(VGM_CMD_DATA_BLOCK);
				vgm.push_back(VGM_CMD_END);
				vgm.push_back(VGM_DATABLOCK_RF5C68_RAM);
				{
					auto &memBlock=this->memWrite[L.reg];
					unsigned int size=memBlock.data.size()+2;

					// 32-bit block size
					vgm.push_back(size&0xFF);
					vgm.push_back((size>>8)&0xFF);
					vgm.push_back(0);
					vgm.push_back(0);

					vgm.push_back(memBlock.address&0xFF);
					vgm.push_back((memBlock.address>>8)&0xFF);

					vgm.insert(vgm.end(),memBlock.data.begin(),memBlock.data.end());
				}
				break;
			}
		}
	}

	vgm.push_back(VGM_CMD_END);

	WriteUint(vgm.data()+VGM_OFFSET_TOTAL_NUM_SAMPLES,nSamples);
	WriteUint(vgm.data()+VGM_OFFSET_EOF,vgm.size()-4);


	AddGD3Tag(vgm,GenerateGD3Tag());

	return vgm;
}

void VGMRecorder::WriteUint(unsigned char *dst,unsigned int data)
{
	dst[0]=data&0xff;
	dst[1]=(data>>8)&0xff;
	dst[2]=(data>>16)&0xff;
	dst[3]=(data>>24)&0xff;
}

unsigned int VGMRecorder::ReadUint(const unsigned char *src)
{
	unsigned int dat=0;
	dat=((unsigned int)src[0]) |
	    ((unsigned int)src[1]<<8) |
	    ((unsigned int)src[2]<<16) |
	    ((unsigned int)src[3]<<24);
	return dat;
}

void VGMRecorder::TrimUnusedDevices(void)
{
	bool useYM2612=false,useYM2203=false,useRF5C68=false,useAY8910=false;
	for(auto L : log)
	{
		switch(L.target)
		{
		case REG_YM2612_CH0:
		case REG_YM2612_CH3:
			if(YM2612_REG_KEY_ON_OFF==L.reg && 0!=(L.value&0xF0))
			{
				useYM2612=true;
			}
			break;
		case REG_YM2203:
			if(YM2612_REG_KEY_ON_OFF==L.reg && 0!=(L.value&0xF0))
			{
				useYM2203=true;
			}
			if((8==L.reg || 9==L.reg || 10==L.reg) && 0!=(L.value&0x1F))
			{
				useYM2203=true;
			}
			break;
		case REG_AY8910:
			if((8==L.reg || 9==L.reg || 10==L.reg) && 0!=(L.value&0x1F))
			{
				useAY8910=true;
			}
			break;
		case REG_RF5C68:
			if(RF5C68_REG_CH_ON_OFF==L.reg && 0xFF!=(L.value&0xFF))
			{
				useRF5C68=true;
			}
			break;
		}
	}
	for(int i=log.size()-1; 0<=i; --i)
	{
		auto &L=log[i];
		if((true!=useYM2612 && (REG_YM2612_CH0==L.target || REG_YM2612_CH3==L.target)) ||
		   (true!=useYM2203 && (REG_YM2203==L.target)) ||
		   (true!=useAY8910 && (REG_AY8910==L.target)) ||
		   (true!=useRF5C68 &&  (REG_RF5C68==L.target || MEM_RF5C68==L.target)))
		{
			log.erase(log.begin()+i);
		}
	}

	if(true!=useYM2612)
	{
		YM2612clock=0;
	}
	if(true!=useYM2203)
	{
		YM2203clock=0;
	}
	if(true!=useAY8910)
	{
		AY8910clock=0;
	}
	if(true!=useRF5C68)
	{
		RF5C68clock=0;
	}
}

void VGMRecorder::TrimNoSoundSegments(void)
{
	size_t firstKeyOn=0;
	size_t lastKeyOff=log.size();

	bool prevKey=false;
	bool YM2612key[6]={false,false,false,false,false,false};
	bool YM2203key[6]={false,false,false,false,false,false};
	bool AY8910key[3]={false,false,false};
	bool RF5C68key[8]={false,false,false,false,false,false,false,false};

	for(size_t i=0; i<log.size(); ++i)
	{
		auto L=log[i];
		switch(L.target)
		{
		case REG_YM2612_CH0:
			if(YM2612_REG_KEY_ON_OFF==L.reg)
			{
				auto ch=(L.value&3);
				if(ch<3)
				{
					YM2612key[ch]=(0!=(L.value&0xF0));
				}
			}
			break;
		case REG_YM2612_CH3:
			if(YM2612_REG_KEY_ON_OFF==L.reg)
			{
				auto ch=(L.value&3);
				if(ch<3)
				{
					YM2612key[3+ch]=(0!=(L.value&0xF0));
				}
			}
			break;
		case REG_YM2203:
			if(YM2203_REG_KEY_ON_OFF==L.reg)
			{
				auto ch=(L.value&3);
				if(ch<3)
				{
					YM2203key[ch]=(0!=(L.value&0xF0));
				}
			}
			if(8==L.reg || 9==L.reg || 10==L.reg)
			{
				YM2203key[3+L.reg-8]=(0!=(L.value&0x1F));
			}
			break;
		case REG_AY8910:
			if(8==L.reg || 9==L.reg || 10==L.reg)
			{
				AY8910key[L.reg-8]=(0!=(L.value&0x1F));
			}
			break;
		case REG_RF5C68:
			if(RF5C68_REG_CH_ON_OFF==L.reg)
			{
				auto value=L.value;
				for(int i=0; i<8; ++i)
				{
					RF5C68key[i]=(0==(value&1));
					value>>=1;
				}
			}
			break;
		}

		bool key=false;
		for(auto b : YM2612key)
		{
			key=(key || b);
		}
		for(auto b : YM2203key)
		{
			key=(key || b);
		}
		for(auto b : AY8910key)
		{
			key=(key || b);
		}
		for(auto b : RF5C68key)
		{
			key=(key || b);
		}
		if(0==firstKeyOn && true==key)
		{
			firstKeyOn=i;
		}
		if(true==prevKey && true!=key)
		{
			lastKeyOff=i+1;
		}
		prevKey=key;
	}

	for(int i=firstKeyOn-1; 0<=i; --i)
	{
		log[i].VMTimeInNanosec=log[i+1].VMTimeInNanosec;
	}

	// If I do the following, I can trim off trailing silence,
	// but I may lose Release phase.
	// while(lastKeyOff<log.size())
	// {
	// 	log.pop_back();
	// }
}

std::vector <unsigned char> VGMRecorder::GenerateExtraHeaderForYM2203CVolumeProblem(void)
{
	// VGM Player seems to play SSG-part of YM2203 way too quiet,
	// or FM-part way too loud.  It seems to be necessary to balance it by extra header.
	std::vector <unsigned char> data;
	data.resize(32);
	for(auto &d : data)
	{
		d=0;
	}

	WriteUint(data.data()  ,12); // 12-byte
	WriteUint(data.data()+4,0);  // No clock-adjustment
	WriteUint(data.data()+8,4);  // 4-bytes from this is Volume-adjustment list.

	// In the future, if I support WHGPlay, I'll need to have 2 entries for the second YM2203
	data[12]=1;    // 1 entry
	data[13]=6;    // 6 for YM2203C (Looks like 0:SN76489, 1:YM2413, 2:YM2612, 3:YM2151, 4:Sega PCM, 5:RF5C68, and 6:YM2203)
	               // Looks like 0x86 for SSG-part of YM2203.
	data[14]=0;    // Flags
	data[15]=0x80; // Relative volume to 0x100.
	               // World Trade Federation!?  It sounds correct if I halved the relative volume.  100% off the real hardware?
	               // I adjusted by listening to the output from VGMPlay.
	               // To adjust it correctly, I should match with the real output from FM77AV.
	               // Which I should be able to do, but that's going to be too much work.
	               // It surprised me that VGM community left this YM2203 volume inaccuracy problem unresolved,
	               // leaving it to the VGM encoder.
	data[16]=0x80; // Bit15 to make it relative.

	return data;
}

std::vector <unsigned char> VGMRecorder::GenerateGD3Tag(void) const
{
	std::vector <unsigned char> data;

	// Identifier
	data.push_back('G');
	data.push_back('d');
	data.push_back('3');
	data.push_back(' ');

	// Version
	data.push_back(0);
	data.push_back(1);
	data.push_back(0);
	data.push_back(0);

	// Size (tentative)
	data.push_back(0);
	data.push_back(0);
	data.push_back(0);
	data.push_back(0);

	AddStringToGD3Tag(data,trackName);  // Track Name in English characters
	AddStringToGD3Tag(data,"");         // Track Name in non-English characters
	AddStringToGD3Tag(data,gameName);   // Game Name in English characters
	AddStringToGD3Tag(data,"");         // Game Name in non-English characters (What encoding should I use?)
	AddStringToGD3Tag(data,systemName); // System Name in English characters
	AddStringToGD3Tag(data,"");         // System Name in non-English characfters
	AddStringToGD3Tag(data,composer);   // Name of Original Track Author in English characters (Should it be the composer?)
	AddStringToGD3Tag(data,"");         // Name of Original Track Author in non-English characters
	AddStringToGD3Tag(data,releaseDate);
	AddStringToGD3Tag(data,whoConverted);
	AddStringToGD3Tag(data,notes);
	data.push_back(0);             // I'm not sure if the last empty string is needed, but the sample VGM I referred to does this.
	data.push_back(0);

	WriteUint(data.data()+8,(unsigned int)data.size()-12);

	return data;
}

std::vector <unsigned char> VGMRecorder::MakeEmptyGD3Tag(void)
{
	std::vector <unsigned char> data;

	// Identifier
	data.push_back('G');
	data.push_back('d');
	data.push_back('3');
	data.push_back(' ');

	// Version
	data.push_back(0);
	data.push_back(1);
	data.push_back(0);
	data.push_back(0);

	// Size (tentative)
	data.push_back(0);
	data.push_back(0);
	data.push_back(0);
	data.push_back(0);

	AddStringToGD3Tag(data,""); // Track Name in English characters
	AddStringToGD3Tag(data,""); // Track Name in non-English characters
	AddStringToGD3Tag(data,""); // Game Name in English characters
	AddStringToGD3Tag(data,""); // Game Name in non-English characters (What encoding should I use?)
	AddStringToGD3Tag(data,""); // System Name in English characters
	AddStringToGD3Tag(data,""); // System Name in non-English characfters
	AddStringToGD3Tag(data,""); // Name of Original Track Author in English characters (Should it be the composer?)
	AddStringToGD3Tag(data,""); // Name of Original Track Author in non-English characters
	AddStringToGD3Tag(data,""); // Game Release Date
	AddStringToGD3Tag(data,""); // Who converted to VGM
	AddStringToGD3Tag(data,""); // Notes.
	data.push_back(0);          // I'm not sure if the last empty string is needed, but the sample VGM I referred to does this.
	data.push_back(0);

	WriteUint(data.data()+8,(unsigned int)data.size()-12);

	return data;
}

void VGMRecorder::UpdateGD3Size(std::vector <unsigned char> &gd3)
{
	if(12<=gd3.size())
	{
		WriteUint(gd3.data()+8,(unsigned int)gd3.size()-12);
	}
}

void VGMRecorder::AddStringToGD3Tag(std::vector <unsigned char> &gd3,std::string str)
{
	for(auto c : str)
	{
		gd3.push_back(c);
		gd3.push_back(0);
	}
	gd3.push_back(0);
	gd3.push_back(0);
}

std::vector <unsigned char> VGMRecorder::GetGD3Tag(const std::vector <unsigned char> &vgmBinary)
{
	std::vector <unsigned char> tag;
	if(VGM_OFFSET_GD3_OFFSET+4<=vgmBinary.size())
	{
		size_t GD3Offset=VGM_OFFSET_GD3_OFFSET+ReadUint(vgmBinary.data()+VGM_OFFSET_GD3_OFFSET);
		if(GD3Offset+12<=vgmBinary.size())
		{
			if(vgmBinary[GD3Offset  ]=='G' &&
			   vgmBinary[GD3Offset+1]=='d' &&
			   vgmBinary[GD3Offset+2]=='3' &&
			   vgmBinary[GD3Offset+3]==' ')
			{
				size_t GD3Size=ReadUint(vgmBinary.data()+GD3Offset+8);
				if(GD3Offset+GD3Size+12<=vgmBinary.size())
				{
					tag.insert(tag.end(),
					           vgmBinary.begin()+GD3Offset,
					           vgmBinary.begin()+GD3Offset+GD3Size+12);
				}
			}
		}
	}
	return tag;
}

std::vector <std::string> VGMRecorder::ExtractGD3Tags(const std::vector <unsigned char> &GD3)
{
	std::vector <std::string> tags;
	auto limit=std::min <unsigned int>(GD3.size(),12+ReadUint(GD3.data()+8));

	std::string current;
	for(size_t i=12; i<limit; i+=2)
	{
		if(0==GD3[i])
		{
			tags.push_back(current);
			current.clear();
		}
		else
		{
			current.push_back(GD3[i]);
		}
	}

	if(""!=current)
	{
		tags.push_back(current);
	}

	return tags;
}

bool VGMRecorder::RemoveGD3Tag(std::vector <unsigned char> &vgm)
{
	std::vector <unsigned char> tag;
	if(VGM_OFFSET_GD3_OFFSET+4<=vgm.size())
	{
		size_t GD3Offset=VGM_OFFSET_GD3_OFFSET+ReadUint(vgm.data()+VGM_OFFSET_GD3_OFFSET);
		if(GD3Offset+12<=vgm.size())
		{
			if(vgm[GD3Offset  ]=='G' &&
			   vgm[GD3Offset+1]=='d' &&
			   vgm[GD3Offset+2]=='3' &&
			   vgm[GD3Offset+3]==' ')
			{
				size_t GD3Size=ReadUint(vgm.data()+GD3Offset+8);
				if(vgm.size()<=GD3Offset+GD3Size+12)
				{
					// GD3 is at the end of file.  Can remove.
					vgm.resize(GD3Offset);
					WriteUint(vgm.data()+VGM_OFFSET_GD3_OFFSET,0);
					WriteUint(vgm.data()+VGM_OFFSET_EOF,vgm.size()-4);
					return true;
				}
			}
		}
	}
	return false;
}

bool VGMRecorder::AddGD3Tag(std::vector <unsigned char> &vgm,const std::vector <unsigned char> &GD3)
{
	if(0==ReadUint(vgm.data()+VGM_OFFSET_GD3_OFFSET))
	{
		auto gd3Offset=vgm.size()-VGM_OFFSET_GD3_OFFSET;
		vgm.insert(vgm.end(),GD3.begin(),GD3.end());
		WriteUint(vgm.data()+VGM_OFFSET_GD3_OFFSET,gd3Offset);
		WriteUint(vgm.data()+VGM_OFFSET_EOF,vgm.size()-4);
		return true;
	}
	else
	{
		// Tag already exists.
		return false;
	}
}

bool VGMRecorder::ClearTagItem(std::vector <unsigned char> &GD3,unsigned int tagId)
{
	size_t tagPtr=12;
	for(int i=0; i<tagId; ++i)
	{
		while(tagPtr<GD3.size() && 0!=GD3[tagPtr])
		{
			tagPtr+=2;
		}
		tagPtr+=2;
	}
	if(GD3.size()<=tagPtr)
	{
		return false;
	}

	while(tagPtr+1<GD3.size() && 0!=GD3[tagPtr])
	{
		GD3.erase(GD3.begin()+tagPtr);
		GD3.erase(GD3.begin()+tagPtr);
	}

	return true;
}

bool VGMRecorder::InsertTagItem(std::vector <unsigned char> &GD3,unsigned int tagId,std::string value)
{
	size_t tagPtr=12;
	for(int i=0; i<tagId; ++i)
	{
		while(tagPtr<GD3.size() && 0!=GD3[tagPtr])
		{
			tagPtr+=2;
		}
		tagPtr+=2;
	}
	if(GD3.size()<=tagPtr)
	{
		return false;
	}

	for(auto c : value)
	{
		GD3.insert(GD3.begin()+tagPtr,c);
		++tagPtr;
		GD3.insert(GD3.begin()+tagPtr,0);
		++tagPtr;
	}

	return true;
}

unsigned int VGMRecorder::StrToTagId(std::string str)
{
	for(auto &c : str)
	{
		c=std::toupper(c);
	}
	for(unsigned int i=0; i<GD3_UNDEFINED; ++i)
	{
		if(str==TagIdToStr(i))
		{
			return i;
		}
	}
	return GD3_UNDEFINED;
}
std::string VGMRecorder::TagIdToStr(unsigned int tagId)
{
	switch(tagId)
	{
	case GD3_TRACKNAME_ENGLISH:
		return "TRACKNAME";
	case GD3_TRACKNAME_INTERNATIONAL:
		return "TRACKNAME_INTL";
	case GD3_GAMENAME_ENGLISH:
		return "GAMENAME";
	case GD3_GAMENAME_INTERNATIONAL:
		return "GAMENAME_INTL";
	case GD3_SYSTEMNAME_ENGLISH:
		return "SYSTEMNAME";
	case GD3_SYSTEMNAME_INTERNATIONAL:
		return "SYSTEMNAME_INTL";
	case GD3_COMPOSER_ENGLISH:
		return "COMPOSER";
	case GD3_COMPOSER_INTERNATIONAL:
		return "COMPOSER_INTL";
	case GD3_RELEASE_DATE:
		return "RELEASEDATE";
	case GD3_WHO_CONVERTED:
		return "WHOCONVERTED";
	case GD3_NOTES:
		return "NOTES";
	}
	return "";
}
