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
	};
	static const char *ErrorCodeToText(unsigned int errCode)
	{
		switch(errCode)
		{
		case ERROR_NOERROR:
			return "No error";
		case ERROR_UNSUPPORTED:
			return "Unsupported file format.";
		case ERROR_CANNOT_OPEN:
			return "Cannot open image file.";
		case ERROR_NOT_YET_SUPPORTED:
			return "File format not yet supported. (I'm working on it!)";
		case ERROR_SECTOR_SIZE:
			return "Binary size is not integer multiple of the sector length.";
		}
		return "Undefined error.";
	}
	enum
	{
		FILETYPE_NONE,
		FILETYPE_ISO,
		FILETYPE_CUE
	};
	enum
	{
		TRACK_MODE1_DATA,
		TRACK_MODE2_DATA,
		TRACK_AUDIO,
	};
	class MinSecFrm
	{
	public:
		short min=0,sec=0,frm=0;
		static MinSecFrm Zero(void);
	};
	class Track
	{
	public:
		unsigned int sectorLength=2352; // Default 2352 bytes per sector.
		std::vector <unsigned char> dataCache;
		MinSecFrm start,end;
		MinSecFrm postGap;
	};
	unsigned int fileType=FILETYPE_NONE;
	unsigned long long int binFileSize=0;
	std::string fName,binFName;
	unsigned int num_sectors;
	std::vector <Track> tracks;



	DiscImage();
	void CleanUp(void);
	unsigned int Open(const std::string &fName);
	unsigned int OpenCUE(const std::string &fName);
	unsigned int OpenISO(const std::string &fName);

	/*! Returns the number of tracks.
	    Returns zero if image is not loaded.
	*/
	unsigned int GetNumTracks(void) const;

	static MinSecFrm HSGtoMSF(unsigned int HSG);
	static unsigned int MSFtoHSG(MinSecFrm MSF);
};



/* } */
#endif
