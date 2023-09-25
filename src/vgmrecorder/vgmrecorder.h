#ifndef VGMRECORDER_IS_INCLUDED
#define VGMRECORDER_IS_INCLUDED
/* { */

#include <stdint.h>
#include <vector>

class VGMRecorder
{
public:
	enum
	{
		YM2612_CH0,
		YM2612_CH3,
		YM2203,
		RF5C68,
		RF5C68_MEM
	};
	class RegWriteLog
	{
	public:
		uint64_t VMTimeInNanosec;
		unsigned char target;
		unsigned int reg;  // Index to memory-write chunk if target is RF5C68_MEM
		unsigned char value;
	};
	class MemoryWriteChunk
	{
	public:
		unsigned int address;
		std::vector <unsigned char> data;
	};

	std::vector <RegWriteLog> log;
	std::vector <MemoryWriteChunk> memWrite;

	/*!
	*/
	void CleanUp(void);

	/*! Log register write.
	*/
	void WriteRegister(uint64_t VMTime,unsigned char target,unsigned int reg,unsigned char value);
	/*! Log write to PCM memory.  Target needs to be RF5C68_MEM.
	*/
	void WritePCMMemory(uint64_t VMTime,unsigned char target,unsigned int address,unsigned char value);

	/*! Target needs to be RF5C68_MEM.
	*/
	void WritePCMMemoryBlock(uint64_t VMTime,unsigned char target,unsigned int address,unsigned int size,unsigned char data[]);
};


/* } */
#endif
