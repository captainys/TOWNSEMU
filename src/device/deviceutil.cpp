#include <vector>
#include <string>
#include "deviceutil.h"

void DeviceUtil::PushInt64(std::vector <unsigned char> &buf,int64_t data)
{
	auto len=buf.size();
	buf.resize(buf.size()+8);
	buf[len  ]= data     &0xff;
	buf[len+1]=(data>> 8)&0xff;
	buf[len+2]=(data>>16)&0xff;
	buf[len+3]=(data>>24)&0xff;
	buf[len+4]=(data>>32)&0xff;
	buf[len+5]=(data>>40)&0xff;
	buf[len+6]=(data>>48)&0xff;
	buf[len+7]=(data>>56)&0xff;
}
void DeviceUtil::PushUint64(std::vector <unsigned char> &buf,uint64_t data)
{
	auto len=buf.size();
	buf.resize(buf.size()+8);
	buf[len  ]= data     &0xff;
	buf[len+1]=(data>> 8)&0xff;
	buf[len+2]=(data>>16)&0xff;
	buf[len+3]=(data>>24)&0xff;
	buf[len+4]=(data>>32)&0xff;
	buf[len+5]=(data>>40)&0xff;
	buf[len+6]=(data>>48)&0xff;
	buf[len+7]=(data>>56)&0xff;
}
void DeviceUtil::PushInt32(std::vector <unsigned char> &buf,int32_t data)
{
	auto len=buf.size();
	buf.resize(buf.size()+4);
	buf[len  ]= data     &0xff;
	buf[len+1]=(data>> 8)&0xff;
	buf[len+2]=(data>>16)&0xff;
	buf[len+3]=(data>>24)&0xff;
}
void DeviceUtil::PushUint32(std::vector <unsigned char> &buf,uint32_t data)
{
	auto len=buf.size();
	buf.resize(buf.size()+4);
	buf[len  ]= data     &0xff;
	buf[len+1]=(data>> 8)&0xff;
	buf[len+2]=(data>>16)&0xff;
	buf[len+3]=(data>>24)&0xff;
}
void DeviceUtil::PushInt16(std::vector <unsigned char> &buf,int16_t data)
{
	auto len=buf.size();
	buf.resize(buf.size()+2);
	buf[len  ]= data     &0xff;
	buf[len+1]=(data>> 8)&0xff;
}
void DeviceUtil::PushUint16(std::vector <unsigned char> &buf,uint16_t data)
{
	auto len=buf.size();
	buf.resize(buf.size()+2);
	buf[len  ]= data     &0xff;
	buf[len+1]=(data>> 8)&0xff;
}
void DeviceUtil::PushBool(std::vector <unsigned char> &buf,bool flag)
{
	auto len=buf.size();
	buf.resize(buf.size()+1);
	buf[len  ]= flag;
}

void DeviceUtil::PushString(std::vector <unsigned char> &buf,std::string str)
{
	PushUint32(buf,str.size()+1);
	for(auto c : str)
	{
		buf.push_back(c);
	}
	buf.push_back(0);
}
void DeviceUtil::PushUcharArray(std::vector <unsigned char> &buf,const std::vector <unsigned char> &data)
{
	PushUint32(buf,(uint32_t)data.size());
	buf.insert(buf.end(),data.begin(),data.end());
}
void DeviceUtil::PushUcharArray(std::vector <unsigned char> &buf,uint64_t len,const unsigned char data[])
{
	for(uint64_t i=0; i<len; ++i)
	{
		buf.push_back(data[i]);
	}
}

uint64_t DeviceUtil::ReadUint64(const unsigned char *&data)
{
	uint64_t v;
	v= ((uint64_t)data[0])|
	  (((uint64_t)data[1])<<8)|
	  (((uint64_t)data[2])<<16)|
	  (((uint64_t)data[3])<<24)|
	  (((uint64_t)data[4])<<32)|
	  (((uint64_t)data[5])<<40)|
	  (((uint64_t)data[6])<<48)|
	  (((uint64_t)data[7])<<56);
	data+=8;
	return v;
}
int64_t DeviceUtil::ReadInt64(const unsigned char *&data)
{
	int64_t v;
	v= ((uint64_t)data[0])|
	  (((uint64_t)data[1])<<8)|
	  (((uint64_t)data[2])<<16)|
	  (((uint64_t)data[3])<<24)|
	  (((uint64_t)data[4])<<32)|
	  (((uint64_t)data[5])<<40)|
	  (((uint64_t)data[6])<<48)|
	  (((uint64_t)data[7])<<56);
	data+=8;
	return v;
}
uint32_t DeviceUtil::ReadUint32(const unsigned char *&data)
{
	uint32_t v;
	v= data[0]|
	  (data[1]<<8)|
	  (data[2]<<16)|
	  (data[3]<<24);
	data+=4;
	return v;
}
int32_t DeviceUtil::ReadInt32(const unsigned char *&data)
{
	int32_t v;
	v= data[0]|
	  (data[1]<<8)|
	  (data[2]<<16)|
	  (data[3]<<24);
	data+=4;
	return v;
}
uint16_t DeviceUtil::ReadUint16(const unsigned char *&data)
{
	uint16_t v;
	v= data[0]|
	  (data[1]<<8);
	data+=2;
	return v;
}
int16_t DeviceUtil::ReadInt16(const unsigned char *&data)
{
	int16_t v;
	v= data[0]|
	  (data[1]<<8);
	data+=2;
	return v;
}
bool DeviceUtil::ReadBool(const unsigned char *&data)
{
	bool v=(0!=*data);
	++data;
	return v;
}
std::string DeviceUtil::ReadString(const unsigned char *&data)
{
	std::string str;
	auto len=ReadUint32(data);

	for(int64_t i=0; i<len; ++i)
	{
		if(0!=*data)
		{
			str.push_back((char)*data);
		}
		++data;
	}

	return str;
}
std::vector <unsigned char> DeviceUtil::ReadUcharArray(const unsigned char *&data)
{
	auto len=ReadUint32(data);
	std::vector <unsigned char> buf;

	for(int64_t i=0; i<len; ++i)
	{
		buf.push_back(*data);
		++data;
	}

	return buf;
}
void DeviceUtil::ReadUcharArray(const unsigned char *&data,uint64_t len,unsigned char buf[])
{
	for(uint64_t i=0; i<len; ++i)
	{
		buf[i]=*data;
		++data;
	}
}
