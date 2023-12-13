#ifndef SJIS_TO_UTF8_H_IS_INCLUDED
#define SJIS_TO_UTF8_H_IS_INCLUDED

#include <unordered_map>
#include <array>
#include <string>
#include <stdint.h>

class ShiftJIS_UTF8
{
protected:
	std::unordered_map <unsigned short,std::array <unsigned char,3> > _SJIStoUTF8;
	std::unordered_map <uint32_t,unsigned short> _UTF8toSJIS;
public:
	ShiftJIS_UTF8();
	std::string SJIStoUTF8(std::string from) const;
	std::string UTF8toSJIS(std::string from) const;
};

#endif
