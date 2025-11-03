#ifndef VGMRECORDER_IS_INCLUDED
#define VGMRECORDER_IS_INCLUDED
/* { */

#include <stdint.h>
#include <string>
#include <vector>

class VGMRecorder
{
public:
	enum
	{
		VGM_HEADER_LENGTH=256,

		VGM_OFFSET_EOF=4,
		VGM_OFFSET_VERSION=8,
		VGM_OFFSET_GD3_OFFSET=0x14,
		VGM_OFFSET_TOTAL_NUM_SAMPLES=0x18,
		VGM_OFFSET_RATE=0x24,
		VGM_OFFSET_YM2612CLK=0x2C,
		VGM_OFFSET_STREAMOFFSET=0x34,
		VGM_OFFSET_RF5C68CLK=0x40,
		VGM_OFFSET_YM2203CLK=0x44,
		VGM_OFFSET_YM2608CLK=0x48,
		VGM_OFFSET_AY8910CLK=0x74,
		VGM_OFFSET_AY8910TYPE=0x78,
		VGM_OFFSET_AY8910FLAGS=0x79,
		VGM_OFFSET_AY_IN_YM2203_FLAGS=0x7A,
		VGM_OFFSET_EXTHEADER_OFFSET=0xBC,

		VGM_CMD_YM2612_CH0=0x52,
		VGM_CMD_YM2612_CH3=0x53,
		VGM_CMD_YM2203=0x55,
		VGM_CMD_YM2203_2=0xA5, // All chips of the YM-familiy that use command 0x5n use 0xAn for the second chip.
		VGM_CMD_YM2608_PORT0=0x56,   // Let 3rd YM2203 pretend to be YM2608 port 0
		VGM_CMD_YM2608_PORT1=0x57,
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
	enum
	{
		YM2612_REG_KEY_ON_OFF=0x28,
		YM2203_REG_KEY_ON_OFF=0x28,
		RF5C68_REG_CH_ON_OFF=8,
	};
	enum
	{
		// Should be in the order stored in GD3.  GD3_TRACKNAME_ENGLISH must be zero.
		GD3_TRACKNAME_ENGLISH,
		GD3_TRACKNAME_INTERNATIONAL,
		GD3_GAMENAME_ENGLISH,
		GD3_GAMENAME_INTERNATIONAL,
		GD3_SYSTEMNAME_ENGLISH,
		GD3_SYSTEMNAME_INTERNATIONAL,
		GD3_COMPOSER_ENGLISH,
		GD3_COMPOSER_INTERNATIONAL,
		GD3_RELEASE_DATE,
		GD3_WHO_CONVERTED,
		GD3_NOTES,
	GD3_UNDEFINED,
	};
	enum
	{
		REG_YM2612_CH0,
		REG_YM2612_CH3,
		REG_YM2203,
		REG_AY8910,
		REG_RF5C68,
		MEM_RF5C68,
		REG_YM2203_2,
		REG_YM2203_3,
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

	std::string trackName;
	std::string gameName;
	std::string systemName;
	std::string composer;
	std::string releaseDate;
	std::string whoConverted;
	std::string notes;

	unsigned int YM2612clock=8000000; // For FM TOWNS YM2612.  I thought it was 2MHz, 3x prescaler.  Why 8MHz works?
	unsigned int YM2203clock=1228800; // For FM77AV YM2203
	unsigned int RF5C68clock=8000000; // Actually I don't know.  I know the sampling frequency is 20725Hz.
	unsigned int AY8910clock=1228800; // For FM-7 AY3-8910
	unsigned int YM2608clock=0;       // For FM77AV Third YM2203 (pretend to be 2608)

	uint32_t secondYM2203flag=0;


	/*!
	*/
	void CleanUp(void);

	/*!
	*/
	void CaptureYM2612InitialCondition(uint64_t VMTime,const class YM2612 &ym2612);

	/*!
	*/
	void CaptureRF5C68InitialCondition(uint64_t VMTime,const class RF5C68 &ym2612);

	/*! FM77AV Emulator Mutsu uses YM2612 class for the FM part of YM2203C, and AY38910 class for the PSG part.
	*/
	void CaptureYM2203InitialCondition(uint64_t VMTime,const class YM2612 &ym2612,const unsigned char ay8910part[16]);

	/*!
	*/
	void CaptureAY8910InitialCondition(uint64_t VMTime,const unsigned char ay8910part[16]);

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

	static void WriteUint(unsigned char *dst,unsigned int data);

	static unsigned int ReadUint(const unsigned char *src);

	/*!
	*/
	void TrimUnusedDevices(void);

	/*!
	*/
	void TrimNoSoundSegments(void);


	std::vector <unsigned char> GenerateExtraHeaderForYM2203CVolumeProblem(void) const;


	/*!
	*/
	std::vector <unsigned char> GenerateGD3Tag(void) const;
	static void AddStringToGD3Tag(std::vector <unsigned char> &gd3,std::string str);

	/*!
	*/
	static std::vector <unsigned char> MakeEmptyGD3Tag(void);

	/*!
	*/
	static void UpdateGD3Size(std::vector <unsigned char> &gd3);

	/*!
	*/
	static std::vector <unsigned char> GetGD3Tag(const std::vector <unsigned char> &vgmBinary);

	/*!
	*/
	static std::vector <std::string> ExtractGD3Tags(const std::vector <unsigned char> &GD3);


	static bool RemoveGD3Tag(std::vector <unsigned char> &vgm);
	static bool AddGD3Tag(std::vector <unsigned char> &vgm,const std::vector <unsigned char> &GD3);

	static bool ClearTagItem(std::vector <unsigned char> &GD3,unsigned int tagId);
	static bool InsertTagItem(std::vector <unsigned char> &GD3,unsigned int tagId,std::string value);

	static unsigned int StrToTagId(std::string str);
	static std::string TagIdToStr(unsigned int tagId);
};


/* } */
#endif
