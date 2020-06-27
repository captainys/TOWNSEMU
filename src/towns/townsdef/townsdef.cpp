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
	if("PAD_A"==str || "pad_a"==str)
	{
		return BOOT_KEYCOMB_PAD_A;
	}
	if("PAD_B"==str || "pad_b"==str)
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
		return "PAD_A";
	case BOOT_KEYCOMB_PAD_B:
		return "PAD_B";
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
	if("KEYBOARD"==str || "keyboard"==str)
	{
		return TOWNS_GAMEPORTEMU_KEYBOARD;
	}
	if("PHYSICAL0"==str || "physical0"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL0;
	}
	if("PHYSICAL1"==str || "physical1"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL1;
	}
	if("PHYSICAL2"==str || "physical2"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL2;
	}
	if("PHYSICAL3"==str || "physical3"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL3;
	}
	if("PHYSICAL4"==str || "physical4"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL4;
	}
	if("PHYSICAL5"==str || "physical5"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL5;
	}
	if("PHYSICAL6"==str || "physical6"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL6;
	}
	if("PHYSICAL7"==str || "physical7"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL7;
	}
	if("ANALOG0"==str || "analog0"==str)
	{
		return TOWNS_GAMEPORTEMU_ANALOG0;
	}
	if("ANALOG1"==str || "analog1"==str)
	{
		return TOWNS_GAMEPORTEMU_ANALOG1;
	}
	if("ANALOG2"==str || "analog2"==str)
	{
		return TOWNS_GAMEPORTEMU_ANALOG2;
	}
	if("ANALOG3"==str || "analog3"==str)
	{
		return TOWNS_GAMEPORTEMU_ANALOG3;
	}
	if("ANALOG4"==str || "analog4"==str)
	{
		return TOWNS_GAMEPORTEMU_ANALOG4;
	}
	if("ANALOG5"==str || "analog5"==str)
	{
		return TOWNS_GAMEPORTEMU_ANALOG5;
	}
	if("ANALOG6"==str || "analog6"==str)
	{
		return TOWNS_GAMEPORTEMU_ANALOG6;
	}
	if("ANALOG7"==str || "analog7"==str)
	{
		return TOWNS_GAMEPORTEMU_ANALOG7;
	}
	if("PHYSICAL0_AS_CYBERSTICK"==str || "physical0_as_cyberstick"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL0_AS_CYBERSTICK;
	}
	if("PHYSICAL1_AS_CYBERSTICK"==str || "physical1_as_cyberstick"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL1_AS_CYBERSTICK;
	}
	if("PHYSICAL2_AS_CYBERSTICK"==str || "physical2_as_cyberstick"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL2_AS_CYBERSTICK;
	}
	if("PHYSICAL3_AS_CYBERSTICK"==str || "physical3_as_cyberstick"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL3_AS_CYBERSTICK;
	}
	if("PHYSICAL4_AS_CYBERSTICK"==str || "physical4_as_cyberstick"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL4_AS_CYBERSTICK;
	}
	if("PHYSICAL5_AS_CYBERSTICK"==str || "physical5_as_cyberstick"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL5_AS_CYBERSTICK;
	}
	if("PHYSICAL6_AS_CYBERSTICK"==str || "physical6_as_cyberstick"==str)
	{
		return TOWNS_GAMEPORTEMU_PHYSICAL6_AS_CYBERSTICK;
	}
	if("PHYSICAL7_AS_CYBERSTICK"==str || "physical7_as_cyberstick"==str)
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
		return "KEYBOARD";
	case TOWNS_GAMEPORTEMU_PHYSICAL0:
		return "PHYSICAL0";
	case TOWNS_GAMEPORTEMU_PHYSICAL1:
		return "PHYSICAL1";
	case TOWNS_GAMEPORTEMU_PHYSICAL2:
		return "PHYSICAL2";
	case TOWNS_GAMEPORTEMU_PHYSICAL3:
		return "PHYSICAL3";
	case TOWNS_GAMEPORTEMU_PHYSICAL4:
		return "PHYSICAL4";
	case TOWNS_GAMEPORTEMU_PHYSICAL5:
		return "PHYSICAL5";
	case TOWNS_GAMEPORTEMU_PHYSICAL6:
		return "PHYSICAL6";
	case TOWNS_GAMEPORTEMU_PHYSICAL7:
		return "PHYSICAL7";
	case TOWNS_GAMEPORTEMU_ANALOG0:
		return "ANALOG0";
	case TOWNS_GAMEPORTEMU_ANALOG1:
		return "ANALOG1";
	case TOWNS_GAMEPORTEMU_ANALOG2:
		return "ANALOG2";
	case TOWNS_GAMEPORTEMU_ANALOG3:
		return "ANALOG3";
	case TOWNS_GAMEPORTEMU_ANALOG4:
		return "ANALOG4";
	case TOWNS_GAMEPORTEMU_ANALOG5:
		return "ANALOG5";
	case TOWNS_GAMEPORTEMU_ANALOG6:
		return "ANALOG6";
	case TOWNS_GAMEPORTEMU_ANALOG7:
		return "ANALOG7";
	case TOWNS_GAMEPORTEMU_PHYSICAL0_AS_CYBERSTICK:
		return "PHYSICAL0_AS_CYBERSTICK";
	case TOWNS_GAMEPORTEMU_PHYSICAL1_AS_CYBERSTICK:
		return "PHYSICAL1_AS_CYBERSTICK";
	case TOWNS_GAMEPORTEMU_PHYSICAL2_AS_CYBERSTICK:
		return "PHYSICAL2_AS_CYBERSTICK";
	case TOWNS_GAMEPORTEMU_PHYSICAL3_AS_CYBERSTICK:
		return "PHYSICAL3_AS_CYBERSTICK";
	case TOWNS_GAMEPORTEMU_PHYSICAL4_AS_CYBERSTICK:
		return "PHYSICAL4_AS_CYBERSTICK";
	case TOWNS_GAMEPORTEMU_PHYSICAL5_AS_CYBERSTICK:
		return "PHYSICAL5_AS_CYBERSTICK";
	case TOWNS_GAMEPORTEMU_PHYSICAL6_AS_CYBERSTICK:
		return "PHYSICAL6_AS_CYBERSTICK";
	case TOWNS_GAMEPORTEMU_PHYSICAL7_AS_CYBERSTICK:
		return "PHYSICAL7_AS_CYBERSTICK";
	}
	return "NONE";
}
