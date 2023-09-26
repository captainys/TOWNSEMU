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
		AY38910,
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

	bool enabled=false;
	std::vector <RegWriteLog> log;
	std::vector <MemoryWriteChunk> memWrite;

	unsigned int YM2612clock=2000000; // For FM TOWNS YM2612
	unsigned int YM2203clock=1228800; // For FM77AV YM2203
	unsigned int RF5C68clock=8000000; // Actually I don't know.  I know the sampling frequency is 20725Hz.
	unsigned int AY8910clock=1228800; // For FM-7 AY3-8910


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

	std::vector <unsigned char> Encode(void);

	void WriteUint(unsigned char *dst,unsigned int data) const;
};


/* } */
#endif
