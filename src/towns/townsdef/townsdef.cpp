/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include "townsdef.h"



unsigned int TownsStrToKeyComb(std::string str)
{
	if("NONE"==str || "none"==str)
	{
		return BOOT_KEYCOMB_NONE;
	}
	if("CD"==str || "cd"==str)
	{
		return BOOT_KEYCOMB_CD;
	}
	if("F0"==str || "f0"==str)
	{
		return BOOT_KEYCOMB_F0;
	}
	if("F1"==str || "f1"==str)
	{
		return BOOT_KEYCOMB_F1;
	}
	if("F2"==str || "f2"==str)
	{
		return BOOT_KEYCOMB_F2;
	}
	if("F3"==str || "f3"==str)
	{
		return BOOT_KEYCOMB_F3;
	}
	if("H0"==str || "h0"==str)
	{
		return BOOT_KEYCOMB_H0;
	}
	if("H1"==str || "h1"==str)
	{
		return BOOT_KEYCOMB_H1;
	}
	if("H2"==str || "h2"==str)
	{
		return BOOT_KEYCOMB_H2;
	}
	if("H3"==str || "h3"==str)
	{
		return BOOT_KEYCOMB_H3;
	}
	if("H4"==str || "h4"==str)
	{
		return BOOT_KEYCOMB_H4;
	}
	if("ICM"==str || "icm"==str)
	{
		return BOOT_KEYCOMB_ICM;
	}
	if("DEBUG"==str || "debug"==str)
	{
		return BOOT_KEYCOMB_DEBUG;
	}
	if("PADA"==str || "pada"==str)
	{
		return BOOT_KEYCOMB_PAD_A;
	}
	if("PADB"==str || "padb"==str)
	{
		return BOOT_KEYCOMB_PAD_B;
	}
	return BOOT_KEYCOMB_NONE;
}

std::string TownsKeyCombToStr(unsigned int keycomb)
{
	switch(keycomb)
	{
	case BOOT_KEYCOMB_NONE:
		break;
	case BOOT_KEYCOMB_CD:
		return "CD";
	case BOOT_KEYCOMB_F0:
		return "F0";
	case BOOT_KEYCOMB_F1:
		return "F1";
	case BOOT_KEYCOMB_F2:
		return "F2";
	case BOOT_KEYCOMB_F3:
		return "F3";
	case BOOT_KEYCOMB_H0:
		return "H0";
	case BOOT_KEYCOMB_H1:
		return "H1";
	case BOOT_KEYCOMB_H2:
		return "H2";
	case BOOT_KEYCOMB_H3:
		return "H3";
	case BOOT_KEYCOMB_H4:
		return "H4";
	case BOOT_KEYCOMB_ICM:
		return "ICM";
	case BOOT_KEYCOMB_DEBUG:
		return "DEBUG";
	case BOOT_KEYCOMB_PAD_A:
		return "PADA";
	case BOOT_KEYCOMB_PAD_B:
		return "PADB";
	}
	return "NONE";
}

unsigned int TownsStrToGamePortEmu(std::string str)
{
	if("NONE"==str || "none"==str)
	{
		return TOWNS_GAMEPORTEMU_NONE;
	}
	if("MOUSE"==str || "mouse"==str)
	{
		return TOWNS_GAMEPORTEMU_MOUSE;
	}
	if("KEY"==str || "key"==str)
	{
		return TOWNS_GAMEPORTEMU_KEYBOARD;
	}
	if("PHYS0"==str || "phys0"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL0;
	}
	if("PHYS1"==str || "phys1"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL1;
	}
	if("PHYS2"==str || "phys2"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL2;
	}
	if("PHYS3"==str || "phys3"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL3;
	}
	if("PHYS4"==str || "phys4"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL4;
	}
	if("PHYS5"==str || "phys5"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL5;
	}
	if("PHYS6"==str || "phys6"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL6;
	}
	if("PHYS7"==str || "phys7"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL7;
	}
	if("ANA0"==str || "ana0"==str)
	{
		return TOWNS_GAMEPORTEMU_ANALOG0;
	}
	if("ANA1"==str || "ana1"==str)
	{
		return TOWNS_GAMEPORTEMU_ANALOG1;
	}
	if("ANA2"==str || "ana2"==str)
	{
		return TOWNS_GAMEPORTEMU_ANALOG2;
	}
	if("ANA3"==str || "ana3"==str)
	{
		return TOWNS_GAMEPORTEMU_ANALOG3;
	}
	if("ANA4"==str || "ana4"==str)
	{
		return TOWNS_GAMEPORTEMU_ANALOG4;
	}
	if("ANA5"==str || "ana5"==str)
	{
		return TOWNS_GAMEPORTEMU_ANALOG5;
	}
	if("ANA6"==str || "ana6"==str)
	{
		return TOWNS_GAMEPORTEMU_ANALOG6;
	}
	if("ANA7"==str || "ana7"==str)
	{
		return TOWNS_GAMEPORTEMU_ANALOG7;
	}
	if("PHYS0CYB"==str || "phys0cyb"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL0_AS_CYBERSTICK;
	}
	if("PHYS1CYB"==str || "phys1cyb"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL1_AS_CYBERSTICK;
	}
	if("PHYS2CYB"==str || "phys2cyb"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL2_AS_CYBERSTICK;
	}
	if("PHYS3CYB"==str || "phys3cyb"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL3_AS_CYBERSTICK;
	}
	if("PHYS4CYB"==str || "phys4cyb"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL4_AS_CYBERSTICK;
	}
	if("PHYS5CYB"==str || "phys5cyb"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL5_AS_CYBERSTICK;
	}
	if("PHYS6CYB"==str || "phys6cyb"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL6_AS_CYBERSTICK;
	}
	if("PHYS7CYB"==str || "phys7cyb"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL7_AS_CYBERSTICK;
	}
	return TOWNS_GAMEPORTEMU_NONE;
}

std::string TownsGamePortEmuToStr(unsigned int emu)
{
	switch(emu)
	{
	case TOWNS_GAMEPORTEMU_NONE:
		return "NONE";
	case TOWNS_GAMEPORTEMU_MOUSE:
		return "MOUSE";
	case TOWNS_GAMEPORTEMU_KEYBOARD:
		return "KEY";
	case TOWNS_GAMEPORTEMU_PHYSICAL0:
		return "PHYS0";
	case TOWNS_GAMEPORTEMU_PHYSICAL1:
		return "PHYS1";
	case TOWNS_GAMEPORTEMU_PHYSICAL2:
		return "PHYS2";
	case TOWNS_GAMEPORTEMU_PHYSICAL3:
		return "PHYS3";
	case TOWNS_GAMEPORTEMU_PHYSICAL4:
		return "PHYS4";
	case TOWNS_GAMEPORTEMU_PHYSICAL5:
		return "PHYS5";
	case TOWNS_GAMEPORTEMU_PHYSICAL6:
		return "PHYS6";
	case TOWNS_GAMEPORTEMU_PHYSICAL7:
		return "PHYS7";
	case TOWNS_GAMEPORTEMU_ANALOG0:
		return "ANA0";
	case TOWNS_GAMEPORTEMU_ANALOG1:
		return "ANA1";
	case TOWNS_GAMEPORTEMU_ANALOG2:
		return "ANA2";
	case TOWNS_GAMEPORTEMU_ANALOG3:
		return "ANA3";
	case TOWNS_GAMEPORTEMU_ANALOG4:
		return "ANA4";
	case TOWNS_GAMEPORTEMU_ANALOG5:
		return "ANA5";
	case TOWNS_GAMEPORTEMU_ANALOG6:
		return "ANA6";
	case TOWNS_GAMEPORTEMU_ANALOG7:
		return "ANA7";
	case TOWNS_GAMEPORTEMU_PHYSICAL0_AS_CYBERSTICK:
		return "PHYS0CYB";
	case TOWNS_GAMEPORTEMU_PHYSICAL1_AS_CYBERSTICK:
		return "PHYS1CYB";
	case TOWNS_GAMEPORTEMU_PHYSICAL2_AS_CYBERSTICK:
		return "PHYS2CYB";
	case TOWNS_GAMEPORTEMU_PHYSICAL3_AS_CYBERSTICK:
		return "PHYS3CYB";
	case TOWNS_GAMEPORTEMU_PHYSICAL4_AS_CYBERSTICK:
		return "PHYS4CYB";
	case TOWNS_GAMEPORTEMU_PHYSICAL5_AS_CYBERSTICK:
		return "PHYS5CYB";
	case TOWNS_GAMEPORTEMU_PHYSICAL6_AS_CYBERSTICK:
		return "PHYS6CYB";
	case TOWNS_GAMEPORTEMU_PHYSICAL7_AS_CYBERSTICK:
		return "PHYS7CYB";
	}
	return "NONE";
}

unsigned int TownsStrToApp(std::string str)
{
	if("WINGCOMMANDER1"==str || "WC1"==str)
	{
		return TOWNS_APPSPECIFIC_WINGCOMMANDER1;
	}
	if("SUPERDAISEN"==str)
	{
		return TOWNS_APPSPECIFIC_SUPERDAISEN;
	}
	return TOWNS_APPSPECIFIC_NONE;
}
std::string TownsAppToStr(unsigned int i)
{
	switch(i)
	{
	default:
		return "NONE";
	case TOWNS_APPSPECIFIC_WINGCOMMANDER1:
		return "WINGCOMMANDER1";
	case TOWNS_APPSPECIFIC_SUPERDAISEN:
		return "SUPERDAISEN";
	}
}
