#ifndef MEMCARD_IS_INCLUDED
#define MEMCARD_IS_INCLUDED
/* { */



#include <vector>
#include <string>
#include <cstdint>

#include "townsdef.h"

class ICMemoryCard
{
public:
	enum
	{
		MEMCARD_TYPE_NONE,
		MEMCARD_TYPE_OLD,
		MEMCARD_TYPE_JEIDA4
	};

	unsigned int type=MEMCARD_TYPE_OLD;
	std::string fName;
	std::vector <unsigned char> data;
	mutable bool modified=false;
	bool readOnly=false;
	uint64_t lastModified=0;

	bool LoadRawImage(std::string fName);
	void SetFileName(std::string fName);
	bool SaveRawImage(void) const;
	bool SaveRawImageIfModified(void) const;
};

/* } */
#endif
