#include "townstips.h"



std::vector <std::string> TownsTips::GetTips(void)
{
	std::vector <std::string> tips;

	tips.push_back(
		"Asuka 120% Burning Fest and Mad Stalker will freeze unless you\n"
		"unplug a mouse from the game ports ixn real FM TOWNS or Tsugaru.\n"
		"Make sure to unplug a mouse from the game ports when you run\n"
		"these game titles.\n"
	);

	tips.push_back(
		"Brandish does not run on 486SX, 486, and Pentium TOWNS models.\n"
		"It first identifies if the CPU is 80386DX, if not Brandish\n"
		"assumes the CPU is 80386SX.  If the cpu is 80486SX or newer,\n"
		"Brandish wrongfully identifies the cpu as 80386SX and subsequently\n"
		"messes up.\n"
		"In Tsugaru, use 'Pretend 80386DX' option, or choose BRANDISh from\n"
		"application-specific augmentation.\n"
	);

	tips.push_back(
		"RUN386.EXE in Towns OS V2.1 L10 does not run with 80486 or newer CPU.\n"
		"Use 'Pretend 80386DX' option to run Towns OS V2.1 L10 on Tsugaru.\n"
	);

	tips.push_back(
		"You might notice a dark horizontal line about 1/3 from the bottom\n"
		"of the window of Tsugaru.  It is emulating what's called damper-wire\n"
		"line.  Fujitsu selected the best-quality monitor available for FM TOWNS,\n"
		"which was SONY's Trinitron CRT.  Only hiccup was the trinitron CRT had \n"
		"a wire inside the tube called damper wire, which casted a shadow on the\n"
		"monitor.  However, it was a signature of a high-quality monitor in 1989.\n"
		"So, Tsugaru emulates it by default.  If you don't like to see the damper-\n"
		"wire line, please turn off 'Damper-Wire Line' option.\n"
	);

	return tips;
}

