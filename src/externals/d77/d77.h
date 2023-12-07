/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef D77_IS_INCLUDED
#define D77_IS_INCLUDED
/* { */

/*
Found in http://fullmotionvideo.free.fr/phpBB3/viewtopic.php?f=4&t=261

D88 (D68/D77/D98) File Structure
================================
-         All values are stored in little-endian format.


Image File Structure:
=====================

          Disk 1 Header
          Disk 1 Data
            Track 0
              Sector 1 Header
              Sector 1 Data
              ...
            Track 1
              Sector 1 Header
              ...
            ...
          Disk 2 Header
          Disk 2 Data
          Disk 3 Header
          ...


Disk Header:
============
Size:     688 or 672 bytes.   

Offset    Size             Meaning
------------------------------------------------------------------------------------
00h       16(CHAR)         Disk name / comment.
10h       1                00h (ASCII comment terminator).
11h       9                Reserved (00h).
1Ah       1                Write protect flag. 00h = normal. 10h = write protected. *1
1Bh       1                Media flag. 00h = 2D, 10h = 2DD, 20h = 2HD. *2
1Ch       4(DWORD)         Disk size (includes header size).
20h       4(DWORD) * 164   Track table. Offset of each track from disk start. *3
                           0 if track is unformatted or not used. *4

*1        Any non-zero value should be treated as write protected.
*2        The tool "1DDITT" also defines the flags 30h for 1D and 40h for 1DD disks.
*3        Older tools only allowed up to 160 entries. -> header size is 672 bytes.
*4        At least one of the many D88 tools is known to set all unused entries at
          the end of the table to the end of the disk.

-         To get the maximum number of tracks check the first non-zero value in the table.
          It must be either 688 or 672.
-         If the disk is unformatted only the first value in the table is set (688 or 672)
          and the disk ends after the table.


Disk Data Area:
===============
Offset:   2A0h or 2B0h.
Size:     variable.

Track:
------
-         Each track starts at the offset (relative to start of disk) found in the
          track table.
-         Tracks don't have any header data.

Sector:
-------
Size:     10h (header) + variable data size

Offset    Size          Meaning
------------------------------------------------------------------------------------------
00h       1             C (cylinder)
01h       1             H (head).
02h       1             R (record/sector ID).
03h       1             N (sector size). Bytes in sector = 128 << N.
04h       2(WORD)       Number of sectors in track. *1
06h       1             Density flag. 00h = double density. 40h = single density.
07h       1             Deleted data (DDAM) flag. 0 = normal, 10h = deleted.
08h       1             FDC status code. 00h = normal, B0h = Data CRC error, etc.
09h       5             Reserved (00h). *2
0Eh       2(WORD)       Actual data size following the sector header in bytes.
---                     End of sector header.
10h       (variable)    Sector data.

*1        If this is not the same for all sectors within a certain track the disk is damaged.
*2        Neko Project II stores values used for 1.44MB floppy disk emulation here.

-         Actual data length might not match N field value.
-         Sectors appear in the order in which they were read.
-         Sector headers without data are possible.
-         Sector header fields should be checked carefully since damaged image files are
          all too common.
*/


#include <array>
#include <vector>
#include <cstdint>
#include <string>

class D77File
{
public:
	enum
	{
		DAMN_BIG_NUMBER=0x7ffffff,

		MB8877_STATUS_DELETED_DATA=0x20,
		MB8877_STATUS_CRC=0x08,
		MB8877_STATUS_RECORD_NOT_FOUND=0x10,

		D77_MEDIATYPE_2D=0,
		D77_MEDIATYPE_2DD=0x10,
		D77_MEDIATYPE_2HD=0x20,
		D77_MEDIATYPE_1D=0x30,
		D77_MEDIATYPE_1DD=0x40,

		D77_DENSITY_FM=0x40,
		D77_DENSITY_MFM=0,

		D77_DATAMARK_NORMAL=0,
		D77_DATAMARK_DELETED=0x10,

		D77_SECTOR_STATUS_CRC=0xB0,
		D77_SECTOR_STATUS_RECORD_NOT_FOUND=0xF0,

		UNSTABLE_NONE=0x00,
		UNSTABLE_VERSION1=0x01,
		UNSTABLE_VERSION2=0x02,
	};

	static std::vector <std::string> QuickParser(const char str[]);
	static void Capitalize(std::string &s);

	static unsigned int DWordToUnsignedInt(const unsigned char ptr[]);
	static unsigned short WordToUnsignedShort(const unsigned char ptr[]);
	static void UnsignedIntToDWord(unsigned char ptr[],const unsigned int dat);
	static void UnsignedShortToWord(unsigned char ptr[],const unsigned int dat);

public:
	class D77Disk
	{
	public:
		class TrackLocation
		{
		public:
			int track;
			int side;
		};

		class HasUnstableByteFlags
		{
		public:
			std::vector <bool> unstableBytes;

			void EncodeRDDUnstableBytes(std::vector <unsigned char> &rdd) const
			{
				if(0<unstableBytes.size())
				{
					size_t numBytes=(unstableBytes.size()+7)/8;

					rdd.push_back(0x10);
					while(14!=(rdd.size()&15))
					{
						rdd.push_back(0);
					}

					rdd.push_back(numBytes&0xFF);
					rdd.push_back((numBytes>>8)&0xFF);

					for(size_t i=0; i<unstableBytes.size(); ++i)
					{
						if(0==(i&7))
						{
							rdd.push_back(0);
						}
						if(true==unstableBytes[i])
						{
							rdd.back()|=(1<<(i&7));
						}
					}
					while(0!=(rdd.size()&15))
					{
						rdd.push_back(0);
					}
				}
			}
		};

		class D77Sector : public HasUnstableByteFlags
		{
		public:
			unsigned char CHRN[4];
			unsigned short nSectorTrack;
			unsigned char density;
			unsigned char deletedData;
			unsigned char crcStatus;
			unsigned char reservedByte[5];
			std::vector <unsigned char> data;

			bool resampled=false;  // true if the sector was sampled multiple times for replicating unstable-byte or Corocoro protect.
			bool probLeafInTheForest=false;  // true if it is suspected to be one of leaf-in-the-forest protect (such as Thexder and Fire Crystal)

			unsigned int nanosecPerByte=0;

			inline unsigned char &C(void){return CHRN[0];}
			inline unsigned char C(void) const{return CHRN[0];}
			inline unsigned char &H(void){return CHRN[1];}
			inline unsigned char H(void) const{return CHRN[1];}
			inline unsigned char &R(void){return CHRN[2];}
			inline unsigned char R(void) const{return CHRN[2];}
			inline unsigned char &N(void){return CHRN[3];}
			inline unsigned char N(void) const{return CHRN[3];}// 128<<N()=size

			// Left for gradual transition.
			unsigned short sectorDataSize(void) const{return data.size();}

			D77Sector();
			~D77Sector();
			void CleanUp(void);
			/*! Make an empty sector.
			    This function leaves nSectorTrack zero.
			    Must be set before written to a track.
			*/
			bool Make(int trk,int sid,int secId,int secSize);

			/*! Returns sector data taking unstable-bytes into account;
			*/
			inline std::vector <unsigned char> GetData(void) const
			{
				if(data.size()!=unstableBytes.size())
				{
					return data;
				}
				else
				{
					auto copy=data;
					for(int i=0; i<copy.size(); ++i)
					{
						if(true==unstableBytes[i])
						{
							copy[i]=rand()&0x255;
						}
					}
					return copy;
				}
			}

			bool SameCHR(const D77Sector &s) const;
			bool SameCHRN(const D77Sector &s) const;
			bool SameCHRNandActualSize(const D77Sector &s) const;
		};
		class D77IDMark : public HasUnstableByteFlags
		{
		public:
			std::array <unsigned char,16> data;  // As stored in .RDD
			bool CRCError(void) const;
		};
		class D77Track : public HasUnstableByteFlags
		{
		friend class D77Disk;

		public:
			class SectorLocation : public TrackLocation
			{
			public:
				int sectorPos;
				int sector;
				int addr;
			};

			std::vector <D77Sector> sector;
			std::vector <unsigned char> trackImage;  // Result by track-dump command of MB8877.
			std::vector <D77IDMark> IDMark; // Result by Read-ID command of MB8877.
			unsigned char FDCStatusAfterTrackRead=0;

			D77Track();
			~D77Track();
			void CleanUp(void);
			void PrintInfo(void) const;
			void PrintDetailedInfo(void) const;
			std::vector <SectorLocation> Find(const std::vector <unsigned char> &pattern) const;
			void Replace(SectorLocation pos,const std::vector <unsigned char> &pattern);

			D77Sector *FindSector(int sectorId);
			const D77Sector *FindSector(int sectorId) const;

			/*! Writes a track from sector data.
			*/
			void WriteTrack(int nSec,const D77Disk::D77Sector sec[]);

			/*! Add a sector to the track.  secSize must be 128,256,512, or 1024.
			*/
			bool AddSector(int trk,int sid,int secId,int secSize);

			/*! Sets CHRN of the sector.
			    This secId is not a sector number, but a sequential index
			    in the track.
			*/
			bool SetSectorCHRN(int secId,int C,int H,int R,int N);

			/*! Returns the track number as desribed in the first sector.
			    It may be different from actual track number.
			*/
			int GetTrackNumber(void) const;

			/*! Returns the side as desribed in the first sector.
			    It may be different from actual side.
			*/
			int GetSide(void) const;


			/*! Check if the track is suspected to be leaf-in-the-forest protect.
			    This type copy-protect (only confirmed on Thexder and Fire Crystal for FM-7)
			    typically has multiple sectors with same CHR.
			*/
			bool SuspectedLeafInTheForest(void) const;


			/*! Identify unstable-byte protect.
			    IdentifyUnstableByte function checks if the track has multpile sectors,
			    but all same CHR for distinguishing multi sample and leaf-in-the-forest.
			    IdentifyUnstableByteRDD function relies on resampled flag of RDD.
			*/
			void IdentifyUnstableByte(void);
			void IdentifyUnstableByteRDD(void);
		private:
			void UnidentifyUnstableByteForContinuousData(void);

		public:
			/*! Returns a list of sectors in the track.
			    Member addr will all be zero.
			*/
			std::vector <SectorLocation> AllSector(void) const;
		};
		class D77Header
		{
		public:
			char diskName[17];
			unsigned char reservedByte[9];
			unsigned char writeProtected;
			unsigned char mediaType; // 0:2D  0x10:2DD  0x20:2HD  (0x30:1D  0x40:1DD?)
			long long int diskSize;  // Including the header
			void CleanUp(void);
		};
		D77Header header;
		std::string rddDiskName;

	private:
		std::vector <D77Track> track;
		bool modified;

	public:
		D77Disk(void);
		~D77Disk(void);
		void CleanUp(void);
		void PrintInfo(void) const;

	private:
		D77Track *FindEditableTrack(int trk,int side);
		D77Track *GetEditableTrack(int trk,int side);

	public:
		/*! Returns a pointer to a track whose first sector identifies the track
		    to be track=trk and head=side.  It may be different from physical track.
		*/
		const D77Track *FindTrack(int trk,int side) const;
		D77Track *FindTrack(int trk,int side);

		/*! Returns a track by the physical location.
		*/
		const D77Track *GetTrack(int trk,int side) const;
		D77Track *GetTrack(int trk,int side);

		std::vector <unsigned char> MakeD77Image(void) const;

		std::vector <unsigned char> MakeRDDImage(void) const;

		std::vector <unsigned char> MakeRawImage(void) const;


		/*! Return the modified-flag. */
		bool IsModified(void) const;

		/*! Clears the modified-flag. */
		void ClearModified(void);

		/*! Sets the modified-flag. */
		void SetModified(void);

		/*! Constructs a disk from a D77 image.
		    It clears the contents.  Also modified-flag will be cleared.
		*/
		bool SetD77Image(const unsigned char d77[],bool verboseMode);

		/*! Construct a disk from an RDD image.
		    It clears the contents.  Also modified-flag will be cleared.
		*/
		bool SetRDDImage(size_t &bytesUsed,size_t len,const unsigned char d77[],bool verboseMode);

		/*! Constructs a track from a bytes in a D77 image.
		*/
		D77Track MakeTrackData(const unsigned char trackPtr[],const unsigned char *lastPtr) const;

		/*! Creates a F-BASIC standard format disk.
		*/
		void CreateStandardFormatted(void);

		/*! Create an unformatted disk.
		    nTrack must be number-of-tracks times number-of-sides.
		    If 40 tracks 2 sides, nTrack must be 80.
		*/
		void CreateUnformatted(int nTrack,const char diskName[]);

		std::vector <unsigned char> ReadTrack(int trk,int sid) const;

		/*! Write sectors to a track.
		    This trk and side points to the physical location, not track and side numbers
		    stored in the sector.
		*/
		bool WriteTrack(int trk,int sid,int nSec,const D77Disk::D77Sector sec[]);

		/*! Write sectors to a track.
		    This trk and side points to the physical location, not track and side numbers
		    stored in the sector.
		    If the track does not exist, it will increase the number of tracks before writing.
		*/
		bool ForceWriteTrack(int trk,int sid,int nSec,const D77Disk::D77Sector sec[]);

		/*! Set Number of Tracks.
		    It assumes each track has two sides.
		    If nTrack==80, track 0 side 0 to track 79 side 1 will be available.
		*/
		void SetNumTrack(unsigned int nTrack);

		/*! Add a sector to a track.
		    This trk and side points to the physical location, not track and side numbers
		    stored in the sector.
		    Sector size (secSize) is in number of bytes.  128,256,512, or 1024.
		*/
		bool AddSector(int trk,int sid,int secId,int secSize);

		/*! Sets CHRN of the sector.
		    This secId is not a sector number, but a sequential index
		    in the track.
		*/
		bool SetSectorCHRN(int trk,int sid,int secId,int C,int H,int R,int N);

		/*! Sets CHRN of the sector.
		    This secId is not a sector number, but a sequential index
		    in the track.
		*/
		bool ReplaceSectorCHRN(int C0,int H0,int R0,int N0,int C,int H,int R,int N);

		/*! Return a list of tracks. */
		std::vector <TrackLocation> AllTrack(void) const;

		/*! Return value is actual number of bytes written.
		*/
		long long int WriteSector(int trk,int sid,int sec,long long int nByte,const unsigned char dat[]);

		/*! Renumber a sector. */
		bool RenumberSector(int trk,int sid,int secFrom,int secTo);

		/*! Renumber a sector. 
		    New size must be 128,256,512, or 1024*/
		bool ResizeSector(int trk,int sid,int sec,int newSize);

		/*! Set CRC error if true, or clear CRC error if false.
		*/
		bool SetCRCError(int trk,int sid,int sec,bool crcError);

		/*! Set DDM (Deleted Data Mark) if true, or clear DDM error if false.
		*/
		bool SetDDM(int trk,int sid,int sec,bool ddm);

		/*! Get CRC error. */
		bool GetCRCError(int trk,int sid,int sec) const;

		/*! Get Record Not Found Error. */
		bool GetRecordNotFound(int trk,int sid,int sec) const;

		/*! Get DDM */
		bool GetDDM(int trk,int sid,int sec) const;

		/*! Deletes duplicate sectors. */
		bool DeleteDuplicateSector(int trk,int sid);

		/*! Check duplicate sectors. */
		bool CheckDuplicateSector(int trk,int sid) const;

		/*! Deletes a sector that has a given id. */
		bool DeleteSectorWithId(int trk,int sid,int sectorId);

		/*! Deletes a sector. */
		bool DeleteSectorByIndex(int trk,int sid,int sectorIdx);

		/*! Replaces a byte string. */
		bool ReplaceData(const std::vector <unsigned char> &from,const std::vector <unsigned char> &to);

		/*! Reads from a sector.
		*/
		std::vector <unsigned char> ReadSector(int trk,int sid,int sec) const;

		/*! Get a pointer to a sector.
		*/
		const D77Sector *GetSector(int trk,int sid,int sec) const;
		D77Sector *GetSector(int trk,int sid,int sec);

		/*! Get a pointer to a sector.
		    It starts search from index posInTrack into the sector list of the track
		    and wrap around.  For emulating skew format and korokoro protect by multi-read.
		    If a sector is found, it increments the posInTrack to the one after the
		    found sector.
		    nStep will be how many sectors has been visited for finding the sector.
		*/
		const D77Sector *GetSectorFrom(int trk,int sid,int sec,unsigned int &posInTrack,unsigned int &nStep) const;
		D77Sector *GetSectorFrom(int trk,int sid,int sec,unsigned int &posInTrack,unsigned int &nStep);

		/*! Get a pointer to a sector by index, not sector number.
		*/
		const D77Sector *GetSectorByIndex(int trk,int sid,int sec) const;
		D77Sector *GetSectorByIndex(int trk,int sid,int sec);

		/*! Copies a track.
		*/
		bool CopyTrack(int dstTrk,int dstSide,int srcTrk,int srcSide);

		/*! Returns if the disk is write-protected.
		*/
		bool IsWriteProtected(void) const;

		/*! Sets write-protected flag. */
		void SetWriteProtected(void);

		/*! Clears write-protected flag. */
		void ClearWriteProtected(void);
	};

private:
	std::vector <D77Disk> disk;

public:
	D77File();
	~D77File();
	void CleanUp(void);

	void SetData(const std::vector <unsigned char> &byteData,bool verboseMode=true);
	void SetData(long long int nByte,const unsigned char byteData[],bool verboseMode=true);

	/*! Create from a RAW image.  Disk type is identified by the file size.
	      1474560 bytes -> 1440KB   512bytes/sector, 18sectors/track, 80tracks, 2sides
	      1261568 bytes -> 1232KB  1024bytes/sector,  8sectors/track, 77tracks, 2sides
		   737280 bytes ->  720KB   512bytes/sector,  9sectors/track, 80tracks, 2sides
		   655360 bytes ->  640KB   512bytes/sector,  8sectors/track, 80tracks, 2sides
		   327680 bytes ->  320KB   256bytes/sector, 16sectors/track, 40tracks, 2sides
	*/
	bool SetRawBinary(const std::vector <unsigned char> &byteData,bool verboseMode=true);
	bool SetRawBinary(long long int nByte,const unsigned char byteData[],bool verboseMode=true);

	/*! Create from .RDD disk image.
	*/
	bool SetRDDData(const std::vector <unsigned char> &byteData,bool verboseMode=true);

	/*! Create a standard format disk and return a disk Id.
	*/
	int CreateStandardFormatted(void);

	/*! Create an unformatted disk and return a disk Id.
	*/
	int CreateUnformatted(int nTrack,const char diskName[]);

	/*! Create a binary to be written to an image file.
	*/
	std::vector <unsigned char> MakeD77Image(void) const;

	/*! Create a binary to be written to an image file.
	*/
	std::vector <unsigned char> MakeRDDImage(void) const;

	/*!  Create a binary to be written to an image file.
	*/
	std::vector <unsigned char> MakeRawImage(void) const;

	bool IsModified(void) const;

	void PrintInfo(void) const;

	long long int GetNumDisk(void) const;

	D77Disk *GetDisk(int diskId);
	const D77Disk *GetDisk(int diskId) const;
};



/* } */
#endif
