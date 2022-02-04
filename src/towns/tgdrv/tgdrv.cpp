/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>
#include "tgdrv.h"
#include "towns.h"

void TownsTgDrv::State::PowerOn(void)
{
}
void TownsTgDrv::State::Reset(void)
{
}

TownsTgDrv::TownsTgDrv(class FMTowns *townsPtr) : Device(townsPtr)
{
	this->townsPtr=townsPtr;
}
/* virtual */ void TownsTgDrv::PowerOn(void)
{
	state.PowerOn();
}
/* virtual */ void TownsTgDrv::Reset(void)
{
	state.Reset();
}
/* virtual */ void TownsTgDrv::IOWriteByte(unsigned int ioport,unsigned int data)
{
	switch(ioport)
	{
	case TOWNSIO_VM_TGDRV:
		switch(data)
		{
		case TOWNS_VM_TGDRV_INSTALL://     0x01,
			std::cout << "Installing Tsugaru Drive." << std::endl;
			townsPtr->cpu.SetCF(false);
			break;
		case TOWNS_VM_TGDRV_INT2FH://      0x02,
			std::cout << "INT 2FH Intercept." << std::endl;
			// Set PF if not my drive.
			// Clear PF if my drive.
			townsPtr->cpu.SetPF(true);
			break;
		}
		break;
	}
}
/* virtual */ unsigned int TownsTgDrv::IOReadByte(unsigned int ioport)
{
	switch(ioport)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
		break;
	}
	return 0xff;
}

/* virtual */ uint32_t TownsTgDrv::SerializeVersion(void) const
{
	return 0;
}
/* virtual */ void TownsTgDrv::SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const
{
}
/* virtual */ bool TownsTgDrv::SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version)
{
	return true;
}
