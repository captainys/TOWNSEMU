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
		REG_YM2612_CH0,
		REG_YM2612_CH3,
		REG_YM2203,
		REG_AY38910,
		REG_RF5C68,
		MEM_RF5C68
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

	std::string trackName="unspecified";
	std::string gameName="unspecified";
	std::string systemName="unspecified";
	std::string composer="unspecified";
	std::string releaseDate="unspecified";
	std::string whoConverted="unspecified";
	std::string notes;

	unsigned int YM2612clock=8000000; // For FM TOWNS YM2612.  I thought it was 2MHz, 3x prescaler.  Why 8MHz works?
	unsigned int YM2203clock=1228800; // For FM77AV YM2203
	unsigned int RF5C68clock=8000000; // Actually I don't know.  I know the sampling frequency is 20725Hz.
	unsigned int AY8910clock=1228800; // For FM-7 AY3-8910


	/*!
	*/
	void CleanUp(void);

	/*!
	*/
	void CaptureYM2612InitialCondition(uint64_t VMTime,const class YM2612 &ym2612);

	/*!
	*/
	void CaptureRF5C68InitialCondition(uint64_t VMTime,const class RF5C68 &ym2612);

	/*! Log register write.
	*/
	void WriteRegister(uint64_t VMTime,unsigned char target,unsigned int reg,unsigned char value);
	/*! Log write to PCM memory.  Target needs to be RF5C68_MEM.
	*/
	void WritePCMMemory(uint64_t VMTime,unsigned char target,unsigned int address,unsigned char value);

	/*! Target needs to be RF5C68_MEM.
	*/
	void WritePCMMemoryBlock(uint64_t VMTime,unsigned char target,unsigned int address,unsigned int size,const unsigned char data[]);

	std::vector <unsigned char> Encode(void) const;

	void WriteUint(unsigned char *dst,unsigned int data) const;

	/*!
	*/
	void TrimUnusedDevices(void);

	/*!
	*/
	void TrimNoSoundSegments(void);

	/*!
	*/
	std::vector <unsigned char> GenerateGD3Tag(void) const;
	void AddStringToGD3Tag(std::vector <unsigned char> &gd3,std::string str) const;
};


/* } */
#endif
