#include <device.h>
#include "crtcbase.h"

void TownsDeviceHasLayer::AnalogPalette::Set16(unsigned int page,unsigned int component,unsigned char v)
{
	v=v&0xF0;
	v|=(v>>4);
	plt16[page][codeLatch&0x0F].v[component]=v;
}
void TownsDeviceHasLayer::AnalogPalette::Set256(unsigned int component,unsigned char v)
{
	plt256[codeLatch].v[component]=v;
}
void TownsDeviceHasLayer::AnalogPalette::SetRed(unsigned char v,unsigned int PLT)
{
	switch(PLT)
	{
	case 0: // 16-color paletter Layer 0
		Set16(0,0,v);
		break;
	case 2: // 16-color paletter Layer 1
		Set16(1,0,v);
		break;
	case 1: // 256-color paletter
	case 3: // 256-color paletter
		Set256(0,v);
		break;
	}
}
void TownsDeviceHasLayer::AnalogPalette::SetGreen(unsigned char v,unsigned int PLT)
{
	switch(PLT)
	{
	case 0: // 16-color paletter Layer 0
		Set16(0,1,v);
		break;
	case 2: // 16-color paletter Layer 1
		Set16(1,1,v);
		break;
	case 1: // 256-color paletter
	case 3: // 256-color paletter
		Set256(1,v);
		break;
	}
}
void TownsDeviceHasLayer::AnalogPalette::SetBlue(unsigned char v,unsigned int PLT)
{
	switch(PLT)
	{
	case 0: // 16-color paletter Layer 0
		Set16(0,2,v);
		break;
	case 2: // 16-color paletter Layer 1
		Set16(1,2,v);
		break;
	case 1: // 256-color paletter
	case 3: // 256-color paletter
		Set256(2,v);
		break;
	}
}

unsigned char TownsDeviceHasLayer::AnalogPalette::Get16(unsigned int page,unsigned int component) const
{
	return plt16[page][codeLatch&0x0F][component]&0xF0;
}
unsigned char TownsDeviceHasLayer::AnalogPalette::Get256(unsigned int component) const
{
	return plt256[codeLatch][component];
}
unsigned char TownsDeviceHasLayer::AnalogPalette::GetRed(unsigned int PLT) const
{
	switch(PLT)
	{
	case 0: // 16-color paletter Layer 0
		return Get16(0,0);
	case 2: // 16-color paletter Layer 1
		return Get16(1,0);
	case 1: // 256-color paletter
	case 3: // 256-color paletter
		return Get256(0);
	}
	return 0;
}
unsigned char TownsDeviceHasLayer::AnalogPalette::GetGreen(unsigned int PLT) const
{
	switch(PLT)
	{
	case 0: // 16-color paletter Layer 0
		return Get16(0,1);
	case 2: // 16-color paletter Layer 1
		return Get16(1,1);
	case 1: // 256-color paletter
	case 3: // 256-color paletter
		return Get256(1);
	}
	return 0;
}
unsigned char TownsDeviceHasLayer::AnalogPalette::GetBlue(unsigned int PLT) const
{
	switch(PLT)
	{
	case 0: // 16-color paletter Layer 0
		return Get16(0,2);
	case 2: // 16-color paletter Layer 1
		return Get16(1,2);
	case 1: // 256-color paletter
	case 3: // 256-color paletter
		return Get256(2);
	}
	return 0;
}

void TownsDeviceHasLayer::AnalogPalette::Serialize(std::vector <unsigned char> &data) const
{
	DeviceUtil::PushUint32(data,codeLatch);
	for(int i=0; i<2; ++i)
	{
		for(int j=0; j<16; ++j)
		{
			uint32_t col;
			col=(plt16[i][j][2]<<16)|(plt16[i][j][1]<<8)|plt16[i][j][0];
			DeviceUtil::PushUint32(data,col);
		}
	}
	for(auto p : plt256)
	{
		uint32_t col;
		col=(p[2]<<16)|(p[1]<<8)|p[0];
		DeviceUtil::PushUint32(data,col);
	}
}
void TownsDeviceHasLayer::AnalogPalette::Deserialize(const unsigned char *&data)
{
	codeLatch=DeviceUtil::ReadUint32(data);
	for(int i=0; i<2; ++i)
	{
		for(int j=0; j<16; ++j)
		{
			uint32_t col=DeviceUtil::ReadUint32(data);
			plt16[i][j][2]=(col>>16)&255;
			plt16[i][j][1]=(col>>8)&255;
			plt16[i][j][0]=col&255;
		}
	}
	for(auto &p : plt256)
	{
		uint32_t col=DeviceUtil::ReadUint32(data);
		p[2]=(col>>16)&255;
		p[1]=(col>>8)&255;
		p[0]=col&255;
	}
}
void TownsDeviceHasLayer::AnalogPalette::Reset(void)
{
	codeLatch=0;
	for(int i=0; i<2; ++i)
	{
		plt16[i][ 0].Set(  0,  0,  0,255);
		plt16[i][ 1].Set(  0,  0,128,255);
		plt16[i][ 2].Set(128,  0,  0,255);
		plt16[i][ 3].Set(128,  0,128,255);
		plt16[i][ 4].Set(  0,128,  0,255);
		plt16[i][ 5].Set(  0,128,128,255);
		plt16[i][ 6].Set(128,128,  0,255);
		plt16[i][ 7].Set(128,128,128,255);
		plt16[i][ 8].Set(  0,  0,  0,255);
		plt16[i][ 9].Set(  0,  0,255,255);
		plt16[i][10].Set(255,  0,  0,255);
		plt16[i][11].Set(255,  0,255,255);
		plt16[i][12].Set(  0,255,  0,255);
		plt16[i][13].Set(  0,255,255,255);
		plt16[i][14].Set(255,255,  0,255);
		plt16[i][15].Set(255,255,255,255);
	}

	for(int i=0; i<256; ++i)
	{
		plt256[i].Set(255,255,255,255);
	}
}

