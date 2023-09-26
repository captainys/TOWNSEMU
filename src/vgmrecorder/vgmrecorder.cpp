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
	if(0<log.size() && RF5C68_MEM==log.back().target && log.back().reg<memWrite.size())
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

void VGMRecorder::WritePCMMemoryBlock(uint64_t VMTime,unsigned char target,unsigned int address,unsigned int size,unsigned char data[])
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

std::vector <unsigned char> VGMRecorder::Encode(void)
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
		VGM_CMD_PCMRAMWRITE=0x68,
		VGM_CMD_END=0x66,
		VGM_CMD_AY8910=0xA0,
		VGM_CMD_RF5C68=0xB0,

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
				nSamples+=dt;
				prevPtr=ptr;
			}

			switch(L.target)
			{
			case YM2612_CH0:
				vgm.push_back(VGM_CMD_YM2612_CH0);
				vgm.push_back(L.reg);
				vgm.push_back(L.value);
				break;
			case YM2612_CH3:
				vgm.push_back(VGM_CMD_YM2612_CH3);
				vgm.push_back(L.reg);
				vgm.push_back(L.value);
				break;
			case YM2203:
				vgm.push_back(VGM_CMD_YM2203);
				vgm.push_back(L.reg);
				vgm.push_back(L.value);
				break;
			case AY38910:
				vgm.push_back(VGM_CMD_AY8910);
				vgm.push_back(L.reg);
				vgm.push_back(L.value);
				break;
			case RF5C68:
				//vgm.push_back(VGM_CMD_RF5C68);
				//vgm.push_back(L.reg);
				//vgm.push_back(L.value);
				break;
			case RF5C68_MEM:
				//vgm.push_back(VGM_CMD_PCMRAMWRITE);
				//vgm.push_back(0x66); // ?
				//vgm.push_back(0xC0); // RF5C68 RAM
				break;
			}
		}
	}

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
