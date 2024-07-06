#include "townstips.h"



std::vector <std::string> TownsTips::GetTips(void)
{
	std::vector <std::string> tips;

	tips.push_back(
		"Asuka 120% Burning Fest and Mad Stalker will freeze unless you\n"
		"unplug a mouse from the game ports in real FM TOWNS or Tsugaru.\n"
		"Make sure to unplug a mouse from the game ports when you run\n"
		"these game titles.\n"
		"In Tsugaru, go to Game-Port options and make sure no mouse is\n"
		"selected, or you can select ASUKA120 from application-specific\n"
		"augmentation.\n"
	);

	tips.push_back(
		"Brandish does not run on 486SX, 486, and Pentium TOWNS models.\n"
		"It first identifies if the CPU is 80386DX, if not Brandish\n"
		"assumes the CPU is 80386SX.  If the cpu is 80486SX or newer,\n"
		"Brandish wrongfully identifies the cpu as 80386SX and subsequently\n"
		"messes up.\n"
		"In Tsugaru, use 'Pretend 80386DX' option, or choose BRANDISH from\n"
		"application-specific augmentation.\n"
	);

	tips.push_back(
		"RUN386.EXE in Towns OS V2.1 L10 does not run with 80486 or newer CPU.\n"
		"Use 'Pretend 80386DX' option to run Towns OS V2.1 L10 on Tsugaru.\n"
		"Freeware Collection 4 Disc B boots up in this version Towns OS.\n"
	);

	tips.push_back(
		"Sherlock Holmes's Consulting Detective crashes if the CD is\n"
		"faster than 1X speed.  Please select 1X speed in the option if you\n"
		"want to run it.  (In real TOWNS, use the compatible mode.)\n"
		"If the CD is too fast, the read-ahead buffer fills up too fast\n"
		"and overflows.\n"
	);

	tips.push_back(
		"You might notice a dark horizontal line about 1/3 from the bottom\n"
		"of the Tsugaru window.  It is emulating what's called a damper-wire\n"
		"line.  Fujitsu selected the best-quality monitor available for FM TOWNS,\n"
		"which was SONY's Trinitron CRT.  The only hiccup was the trinitron CRT had \n"
		"a wire inside the tube called damper wire, which cast a shadow on the\n"
		"monitor.  However, it was a signature of a high-quality monitor in 1989.\n"
		"So, Tsugaru emulates it by default.  If you don't like to see the damper-\n"
		"wire line, please turn off 'Damper-Wire Line' option.\n"
	);

	tips.push_back(
		"Tsugaru supports Windows 3.1, Windows 95, Linux, and OSASK.  To use those\n"
		"operating systems, please check 'High-Fidelity Mode' option.  Many of the\n"
		"80386's protection features are unnecessary for running majority of\n"
		"Towns-OS games, so Tsugaru runs faster with this option turned off.\n"
		"However, other operating systems, namely pre-emptive multi-tasking\n"
		"operating systems, require those CPU features, which will be enabled\n"
		"with 'High-Fidelity Mode' option.\n"
	);

	tips.push_back(
		"When mouse cursor does not move, or mouse goes crazy, you can stabilize\n"
		"it by controlling mouse by keyboard arrow keys, or a USB game pad.\n"
		"Go to Game-Port options, and select appropriate mouse mode.\n"
	);

	return tips;
}

