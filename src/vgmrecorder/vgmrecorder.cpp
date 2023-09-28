#include "ym2612.h"
#include "rf5c68.h"
#include "vgmrecorder.h"

void VGMRecorder::CleanUp(void)
{
	log.clear();
	memWrite.clear();
}

void VGMRecorder::CaptureYM2612InitialCondition(uint64_t VMTime,const class YM2612 &ym2612)
{
	for(unsigned int reg=0x22; reg<=0x28; ++reg)
	{
		WriteRegister(VMTime,REG_YM2612_CH0,reg,ym2612.state.regSet[0][reg]);
	}
	for(unsigned int reg=0x30; reg<=0xB0; ++reg)
	{
		WriteRegister(VMTime,REG_YM2612_CH0,reg,ym2612.state.regSet[0][reg]);
	}
	for(unsigned int reg=0x30; reg<=0xB0; ++reg)
	{
		WriteRegister(VMTime,REG_YM2612_CH3,reg,ym2612.state.regSet[1][reg]);
	}
}
void VGMRecorder::CaptureRF5C68InitialCondition(uint64_t VMTime,const class RF5C68 &rf5c68)
{
	WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_CH_ON_OFF,0xFF);
	for(int bank=0; bank<0x10; ++bank)
	{
		unsigned char data=(true==rf5c68.state.playing ? 0x80 : 0);
		data|=bank;
		WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_CONTROL,data);
		WritePCMMemoryBlock(VMTime,MEM_RF5C68,0,0x1000,rf5c68.state.waveRAM.data()+(bank<<12));
	}

	for(int ch=0; ch<RF5C68::NUM_CHANNELS; ++ch)
	{
		unsigned char data=(true==rf5c68.state.playing ? 0x80 : 0);
		data|=0x40; // MOD=CB
		data|=ch;
		WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_CONTROL,data);
		WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_ENV,rf5c68.state.ch[ch].ENV);
		WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_PAN,rf5c68.state.ch[ch].PAN);
		WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_FDL,rf5c68.state.ch[ch].FD&0xFF);
		WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_FDH,(rf5c68.state.ch[ch].FD>>8)&0xFF);
		WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_LSL,rf5c68.state.ch[ch].LS&0xFF);
		WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_LSH,(rf5c68.state.ch[ch].LS>>8)&0xFF);
		WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_ST,rf5c68.state.ch[ch].ST);
	}
	WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_CH_ON_OFF,rf5c68.state.chOnOff);

	unsigned char data=(true==rf5c68.state.playing ? 0x80 : 0);
	data|=0x40; // MOD=CB
	data|=rf5c68.state.CB;
	WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_CONTROL,data);

	data=(true==rf5c68.state.playing ? 0x80 : 0);
	data|=rf5c68.state.Bank; // MOD=WB
	WriteRegister(VMTime,REG_RF5C68,RF5C68::REG_CONTROL,data);
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

	vgm.resize(256);
	for(auto &d : vgm)
	{
		d=0;
	}

	vgm[0]=0x56; // "Vgm " signature.
	vgm[1]=0x67;
	vgm[2]=0x6d;
	vgm[3]=0x20;

	enum
	{
		VGM_HEADER_LENGTH=256,

		VGM_OFFSET_EOF=4,
		VGM_OFFSET_VERSION=8,
		VGM_OFFSET_TOTAL_NUM_SAMPLES=0x18,
		VGM_OFFSET_RATE=0x24,
		VGM_OFFSET_YM2612CLK=0x2C,
		VGM_OFFSET_STREAMOFFSET=0x34,
		VGM_OFFSET_RF5C68CLK=0x40,
		VGM_OFFSET_YM2203CLK=0x44,
		VGM_OFFSET_AY8910CLK=0x74,
		VGM_OFFSET_AY8910TYPE=0x78,
		VGM_OFFSET_AY8910FLAGS=0x79,

		VGM_CMD_YM2612_CH0=0x52,
		VGM_CMD_YM2612_CH3=0x53,
		VGM_CMD_YM2203=0x55,
		VGM_CMD_WAIT=0x61,
		VGM_CMD_WAIT_735=0x62,
		VGM_CMD_WAIT_882=0x63,
		VGM_CMD_DATA_BLOCK=0x67,
		VGM_CMD_END=0x66,
		VGM_CMD_AY8910=0xA0,
		VGM_CMD_RF5C68=0xB0,

		VGM_DATABLOCK_RF5C68_RAM=0xC0,

		VGM_RATE_NTSC=60,
	};


	WriteUint(vgm.data()+VGM_OFFSET_VERSION,0x0171); // Version

	WriteUint(vgm.data()+VGM_OFFSET_RATE,VGM_RATE_NTSC); // Rate

	WriteUint(vgm.data()+VGM_OFFSET_YM2612CLK,YM2612clock);

	WriteUint(vgm.data()+VGM_OFFSET_STREAMOFFSET,(VGM_HEADER_LENGTH-VGM_OFFSET_STREAMOFFSET)); // Relative offset to VGM data stream.

	WriteUint(vgm.data()+VGM_OFFSET_RF5C68CLK,RF5C68clock);

	WriteUint(vgm.data()+VGM_OFFSET_YM2203CLK,YM2203clock);

	WriteUint(vgm.data()+VGM_OFFSET_AY8910CLK,AY8910clock);
	vgm[VGM_OFFSET_AY8910TYPE]=0; // AY8910
	vgm[VGM_OFFSET_AY8910FLAGS]=0; // What's this?


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
			case REG_AY38910:
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
	return vgm;
}

void VGMRecorder::WriteUint(unsigned char *dst,unsigned int data) const
{
	dst[0]=data&0xff;
	dst[1]=(data>>8)&0xff;
	dst[2]=(data>>16)&0xff;
	dst[3]=(data>>24)&0xff;
}

void VGMRecorder::TrimUnusedDevices(void)
{
	bool useYM2612=false,useYM2203=false,useRF5C68=false,useAY38910=false;
	for(auto L : log)
	{
		switch(L.target)
		{
		case REG_YM2612_CH0:
		case REG_YM2612_CH3:
			if(YM2612::REG_KEY_ON_OFF==L.reg && 0!=(L.value&0xF0))
			{
				useYM2612=true;
			}
			break;
		case REG_YM2203:
			if(YM2612::REG_KEY_ON_OFF==L.reg && 0!=(L.value&0xF0))
			{
				useYM2203=true;
			}
			if((8==L.reg || 9==L.reg || 10==L.reg) && 0!=(L.value&0x1F))
			{
				useYM2203=true;
			}
			break;
		case REG_AY38910:
			if((8==L.reg || 9==L.reg || 10==L.reg) && 0!=(L.value&0x1F))
			{
				useAY38910=true;
			}
			break;
		case REG_RF5C68:
			if(RF5C68::REG_CH_ON_OFF==L.reg && 0xFF!=(L.value&0xFF))
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
		   (true!=useAY38910 && (REG_AY38910==L.target)) ||
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
	if(true!=useAY38910)
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
			if(YM2612::REG_KEY_ON_OFF==L.reg)
			{
				auto ch=(L.value&3);
				if(ch<3)
				{
					YM2612key[ch]=(0!=(L.value&0xF0));
				}
			}
			break;
		case REG_YM2612_CH3:
			if(YM2612::REG_KEY_ON_OFF==L.reg)
			{
				auto ch=(L.value&3);
				if(ch<3)
				{
					YM2612key[3+ch]=(0!=(L.value&0xF0));
				}
			}
			break;
		case REG_YM2203:
			if(YM2612::REG_KEY_ON_OFF==L.reg)
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
		case REG_AY38910:
			if(8==L.reg || 9==L.reg || 10==L.reg)
			{
				AY8910key[L.reg-8]=(0!=(L.value&0x1F));
			}
			break;
		case REG_RF5C68:
			if(RF5C68::REG_CH_ON_OFF==L.reg)
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
