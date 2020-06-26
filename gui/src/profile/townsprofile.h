#ifndef TOWNSPROFILE_IS_INCLUDED
#define TOWNSPROFILE_IS_INCLUDED
/* { */

#include <vector>
#include <string>

class TownsProfile
{
public:
	enum
	{
		NUM_STANDBY_FDIMG=4,
		MAX_NUM_SCSI_DEVICE=7,
	};
	std::string ROMDir;
	std::string CDImgFile;
	std::string FDImgFile[2][NUM_STANDBY_FDIMG];
	std::string SCSIImgFile[MAX_NUM_SCSI_DEVICE];
	unsigned int bootKeyComb=BOOT_KEYCOMB_NONE;

	void CleanUp(void);
	std::vector <std::string> Serialize(void) const;
	bool Deserialize(const std::vector <std::string> &text);
	std::vector <std::string> MakeArgv(void) const;
};

/* } */
#endif
