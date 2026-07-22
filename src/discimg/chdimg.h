/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef CHDIMG_IS_INCLUDED
#define CHDIMG_IS_INCLUDED
/* { */

#include <vector>
#include <string>
#include <cstdint>
#include <mutex>

/*! Reader for MAME's Compressed Hunks of Data (.CHD) CD images, backed by libchdr.
    All compression types libchdr knows are usable, which includes CDZL (zlib), CDLZ (LZMA),
    CDFL (FLAC), and CDZS (Zstandard).

    A .CHD stores a CD as fixed-size frames of 2352 bytes of sector data plus 96 bytes of
    subcode, and it pads every track to a multiple of 4 frames.  Gaps that were not part of
    the source binary are not stored at all.  DiscImage on the other hand wants to see a
    flat binary like a BIN file.  Therefore this class flattens the disc: it presents a
    virtual binary in which every sector of the disc takes trackSectorLength bytes,
    tracks come in disc order, and gaps missing from the .CHD read back as zero.
    DiscImage then only needs the track table and Read.
*/
class CHDImage
{
public:
	enum
	{
		FRAME_SIZE=2448,        // Sector data (2352) plus subcode (96).
		SECTOR_DATA_SIZE=2352,
		TRACK_PADDING=4,        // Tracks are padded to a multiple of this many frames.
	};
	enum
	{
		TRACK_UNKNOWNTYPE,
		TRACK_MODE1_DATA,
		TRACK_MODE2_DATA,
		TRACK_AUDIO,
	};
	enum
	{
		CHDERROR_NOERROR,
		CHDERROR_CANNOT_OPEN,
		CHDERROR_NOT_A_CD,
		CHDERROR_INVALID_METADATA,
		CHDERROR_UNSUPPORTED_TRACK_TYPE,
	};

	class Track
	{
	public:
		unsigned int trackNum=0;
		unsigned int trackType=TRACK_UNKNOWNTYPE;
		unsigned int sectorLength=SECTOR_DATA_SIZE; // Bytes of sector data per frame.  2048, 2324, 2336, or 2352.

		/*! Sectors of the disc, in disc order.
		    firstLBA         Start of the pre-gap, which is where the track begins for the CD player.
		    index01LBA       Start of the track data proper.
		    dataStartLBA     First sector that is actually stored in the .CHD.
		    numFramesInCHD   Sectors stored in the .CHD, pre-gap included if the pre-gap is stored.
		    postGapFrames    Sectors of silence after the data.  Never stored in the .CHD.
		*/
		unsigned int firstLBA=0;
		unsigned int index01LBA=0;
		unsigned int dataStartLBA=0;
		unsigned int numFramesInCHD=0;
		unsigned int postGapFrames=0;

		uint64_t locationInImage=0;  // Byte offset of dataStartLBA in the flattened image.
		unsigned int chdFrame=0;     // Frame number in the .CHD of dataStartLBA.
	};

	CHDImage();
	~CHDImage();
	CHDImage(const CHDImage &)=delete;
	CHDImage &operator=(const CHDImage &)=delete;

	/*! Opens a .CHD file.  Returns one of the CHDERROR_* codes.
	*/
	unsigned int Open(const std::string &fName);
	void Close(void);
	bool IsOpen(void) const;

	inline const std::vector <Track> &GetTracks(void) const
	{
		return tracks;
	}
	/*! Number of sectors of the disc, gaps included.
	*/
	inline unsigned int GetNumSectors(void) const
	{
		return numSectors;
	}
	/*! Size in bytes of the flattened image that Read presents.
	*/
	inline uint64_t GetImageSize(void) const
	{
		return imageSize;
	}

	/*! Reads len bytes at offset of the flattened image.  Bytes that the .CHD does not
	    store (missing gaps, and anything past the end of the disc) read back as zero.
	    Audio sectors are byte-swapped to little-endian, since a .CHD keeps CD-DA
	    big-endian while a BIN file, and hence the rest of Tsugaru, expects little-endian.

	    Thread safe.  The CD-ROM drive reads sectors from the VM thread while the
	    AsyncWaveReader thread reads CD-DA from the same image.
	*/
	bool Read(unsigned char *dst,uint64_t offset,uint64_t len) const;

private:
	/*! One run of sectors of the flattened image that is either stored in the .CHD or not.
	*/
	class Region
	{
	public:
		uint64_t locationInImage=0;
		uint64_t numBytes=0;
		unsigned int sectorLength=0;
		unsigned int chdFrame=0; // First frame in the .CHD.  Meaningless if !inCHD.
		bool inCHD=false;
		bool audio=false;
	};

	unsigned int MakeTrackTable(void);
	const unsigned char *GetFrame(unsigned int frame) const;

	struct _chd_file *chd=nullptr;
	std::vector <Track> tracks;
	std::vector <Region> regions;
	unsigned int numSectors=0;
	uint64_t imageSize=0;
	unsigned int framesPerHunk=0;

	mutable std::mutex readLock;
	mutable std::vector <unsigned char> hunkBuf;
	mutable unsigned int hunkInBuf=~0;
};

/* } */
#endif
