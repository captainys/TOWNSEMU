#undef YS_LITTLE_ENDIAN

#include <iostream>
#include "cpputil.h"



bool TestUint32(uint32_t x)
{
	auto lowWord=cpputil::LowWord(x);
	auto highWord=cpputil::HighWord(x);

	if(lowWord!=(x&0xFFFF))
	{
		std::cout << cpputil::Uitox(x) << std::endl;
		return false;
	}
	if(highWord!=((x>>16)&0xFFFF))
	{
		std::cout << cpputil::Uitox(x) << std::endl;
		return false;
	}
	if(cpputil::WordPairToUnsigned32(lowWord,highWord)!=x)
	{
		std::cout << cpputil::Uitox(x) << std::endl;
		return false;
	}
	return true;
}

bool TestUint64(uint64_t x)
{
	auto lowDword=cpputil::LowDword(x);
	auto highDword=cpputil::HighDword(x);
	if(lowDword!=(x&0xFFFFFFFF))
	{
		std::cout << x << std::endl;
		return false;
	}
	if(highDword!=((x>>32)&0xFFFFFFFF))
	{
		std::cout << x << std::endl;
		return false;
	}
	if(cpputil::DwordPairToUnsigned64(lowDword,highDword)!=x)
	{
		std::cout << x << std::endl;
		return false;
	}
	return true;
}

bool TestInt64(int64_t x)
{
	auto lowDword=cpputil::LowDword(x);
	auto highDword=cpputil::HighDword(x);
	if(x!=cpputil::DwordPairToSigned64(lowDword,highDword))
	{
		return false;
	}
	return true;
}

int main(void)
{
	auto u16=cpputil::MakeUnsignedWord(0x11,0x22);
	if(u16!=0x2211)
	{
		std::cout << "U16" << std::endl;
		return 1;
	}

	auto u32=cpputil::MakeUnsignedDword(0x11,0x22,0x33,0x44);
	if(u32!=0x44332211)
	{
		std::cout << "U32" << std::endl;
		return 1;
	}


	{
		uint32_t testcases[]=
		{
			0x80008000,
			0x8FFF8FFF,
			0xABCD0123,
		};
		for(auto x : testcases)
		{
			if(true!=TestUint32(x))
			{
				return 1;
			}
		}
	}
	{
		uint64_t testcases[]=
		{
			0x8000000080000000LL,
			0x8FFFFFFF8FFFFFFFLL,
			0xABCDABCD01230123LL,
		};
		for(auto x : testcases)
		{
			if(true!=TestUint64(x))
			{
				return 1;
			}
		}
	}
	{
		int64_t testcases[]=
		{
			-0x1234567812345678LL,
			 0x1234567812345678LL,
			-0x7FFFFFFFFFFFFFFFLL,
			 0x7FFFFFFFFFFFFFFFLL,
		};
		for(auto x : testcases)
		{
			if(true!=TestInt64(x))
			{
				return 1;
			}
		}
	}

	std::cout << "Pass" << std::endl;

	return 0;
}
