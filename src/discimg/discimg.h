/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef DISCIMG_IS_INCLUDED
#define DISCIMG_IS_INCLUDED
/* { */

#include <vector>
#include <string>

class DiscImage
{
public:
	enum
	{
		ERROR_NOERROR,
		ERROR_UNSUPPORTED,
		ERROR_CANNOT_OPEN,
		ERROR_NOT_YET_SUPPORTED,
		ERROR_SECTOR_SIZE,
		ERROR_TOO_FEW_ARGS,
		ERROR_UNSUPPORTED_TRACK_TYPE,
		ERROR_SECTOR_LENGTH_NOT_GIVEN,
		ERROR_TRACK_INFO_WITHOTU_TRACK,
		ERROR_INCOMPLETE_MSF,
		ERROR_FIRST_TRACK_NOT_STARTING_AT_00_00_00,
		ERROR_BINARY_FILE_NOT_FOUND,
		ERROR_BINARY_SIZE_NOT_SECTOR_TIMES_INTEGER,
	};
	enum
	{
		HSG_BASE=150,
		MODE1_BYTES_PER_SECTOR=2048,
		RAW_BYTES_PER_SECTOR=2352,
	};
	enum
	{
		FILETYPE_NONE,
		FILETYPE_ISO,
		FILETYPE_CUE
	};
	enum
	{
		TRACK_UNKNOWNTYPE,
		TRACK_MODE1_DATA,
		TRACK_MODE2_DATA,
		TRACK_AUDIO,
	};
	class MinSecFrm
	{
	public:
		short min=0,sec=0,frm=0;
		static MinSecFrm Zero(void);
		static MinSecFrm TwoSeconds(void);

		inline void Set(short m,short s,short f)
		{
			min=m;
			sec=s;
			frm=f;
		}


		inline unsigned int Encode(void) const  // Different from HSG
		{
			return (min<<16)|(sec<<8)|frm;
		}

		inline void Increment(void)
		{
			++this->frm;
			if(75<=this->frm)
			{
				this->frm-=75;
				++this->sec;
			}
			if(60<=this->sec)
			{
				this->sec-=60;
				++this->min;
			}
		}
		inline void Decrement(void)
		{
			--this->frm;
			if(this->frm<0)
			{
				this->frm+=75;
				--this->sec;
			}
			if(this->sec<0)
			{
				this->sec+=60;
				--this->min;
			}
		}

		inline void Add(const MinSecFrm incoming)
		{
			this->frm+=incoming.frm;
			if(75<=this->frm)
			{
				this->frm-=75;
				++this->sec;
			}
			this->sec+=incoming.sec;
			if(60<=this->sec)
			{
				this->sec-=60;
				++this->min;
			}
			this->min+=incoming.min;
		}
		inline MinSecFrm &operator+=(const MinSecFrm incoming)
		{
			Add(incoming);
			return *this;
		}
		inline MinSecFrm operator+(const MinSecFrm incoming)
		{
			auto ret=*this;
			ret.Add(incoming);
			return ret;
		}


		inline void Sub(const MinSecFrm incoming)
		{
			this->frm-=incoming.frm;
			if(this->frm<0)
			{
				this->frm+=75;
				--this->sec;
			}
			this->sec-=incoming.sec;
			if(this->sec<0)
			{
				this->sec+=60;
				--this->min;
			}
			this->min-=incoming.min;
		}
		inline MinSecFrm &operator-=(const MinSecFrm incoming)
		{
			Sub(incoming);
			return *this;
		}
		inline MinSecFrm operator-(const MinSecFrm incoming)
		{
			auto ret=*this;
			ret.Sub(incoming);
			return ret;
		}

		inline bool operator==(const MinSecFrm incoming) const
		{
			return this->Encode()==incoming.Encode();
		}
		inline bool operator!=(const MinSecFrm incoming) const
		{
			return this->Encode()!=incoming.Encode();
		}
		inline bool operator>(const MinSecFrm incoming) const
		{
			return this->Encode()>incoming.Encode();
		}
		inline bool operator<(const MinSecFrm incoming) const
		{
			return this->Encode()<incoming.Encode();
		}
		inline bool operator>=(const MinSecFrm incoming) const
		{
			return this->Encode()>=incoming.Encode();
		}
		inline bool operator<=(const MinSecFrm incoming) const
		{
			return this->Encode()<=incoming.Encode();
		}


		inline unsigned int ToHSG(void) const
		{
			return (min*60+sec)*75+frm;
		}
		inline void FromHSG(unsigned int HSG)
		{
			frm=HSG%75;
			HSG/=75;
			sec=HSG%60;
			HSG/=60;
			min=HSG;
		}
	};
	class Track
	{
	public:
		unsigned int trackType=TRACK_UNKNOWNTYPE;
		unsigned int sectorLength=2352; // Default 2352 bytes per sector.
		unsigned long long int locationInFile=0;
		mutable std::vector <unsigned char> dataCache;
		MinSecFrm start,end;  // end must be 1-frame before the next track or the disc length.
		MinSecFrm index00,preGap,postGap; // Wikipedia suggests index00 is preGap.

		inline Track()
		{
			CleanUp();
		}
		inline void CleanUp(void)
		{
			trackType=TRACK_UNKNOWNTYPE;
			sectorLength=2352;
			locationInFile=0;
			dataCache.clear();
			start=MinSecFrm::Zero();
			end=MinSecFrm::Zero();
			preGap=MinSecFrm::Zero();
			postGap=MinSecFrm::Zero();
			index00=MinSecFrm::Zero();
		}
	};
	unsigned int fileType=FILETYPE_NONE;
	unsigned long long int binFileSize=0;
	std::string fName,binFName;
	unsigned int num_sectors;
	std::vector <Track> tracks;



	DiscImage();
	static const char *ErrorCodeToText(unsigned int errCode);
	void CleanUp(void);
	unsigned int Open(const std::string &fName);
	unsigned int OpenCUE(const std::string &fName);
private:
	unsigned int OpenCUEPostProcess(void);

public:
	unsigned int OpenISO(const std::string &fName);

	/*! Returns the number of tracks.
	    Returns zero if image is not loaded.
	*/
	unsigned int GetNumTracks(void) const;

	/*! Returns theu number of sectors.
	*/
	unsigned int GetNumSectors(void) const;

	/*! Returns tracks.
	*/
	const std::vector <Track> &GetTracks(void) const;

	/*! Read data sectors MODE1 (2048 bytes/sector).
	    If it cannot be read (no data track, unsupported sector length, go beyond the data-track limit, etc.),
	    it returns zero byte.
	*/
	std::vector <unsigned char> ReadSectorMODE1(unsigned int HSG,unsigned int numSec) const;


	/*! Returns the track from MSF.
	    Returns -1 if it is not on any track.
	*/
	int GetTrackFromMSF(MinSecFrm MSF) const;


	/*! Returns the file location from track and MSF.
	    Track number starts with 1.
	*/
	unsigned long long int GetFileLocationFromTrackAndMSF(int track,MinSecFrm MSF) const;


	/*! Returns the 44KHz wave from start and end MSFs.
	    Can be as large as 700MB.
	*/
	std::vector <unsigned short> GetWave(MinSecFrm startMSF,MinSecFrm endMSF) const;



	/*! Convert a string MM:SS:FF to MinSecFrm structure.
	*/
	static bool StrToMSF(MinSecFrm &msf,const char str[]);

	static MinSecFrm HSGtoMSF(unsigned int HSG);
	static unsigned int MSFtoHSG(MinSecFrm MSF);
	static unsigned int BinToBCD(unsigned int bin);
	static unsigned int BCDToBin(unsigned int bin);
};



/* } */
#endif
