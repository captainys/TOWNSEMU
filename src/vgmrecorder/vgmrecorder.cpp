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
