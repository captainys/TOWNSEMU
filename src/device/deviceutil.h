#ifndef DEVICEUTIL_IS_INCLUDED
#define DEVICEUTIL_IS_INCLUDED
/* { */

#include <vector>
#include <string>

class DeviceUtil
{
protected:
	static void PushInt64(std::vector <unsigned char> &buf,int64_t data);
	static void PushUint64(std::vector <unsigned char> &buf,uint64_t data);
	static void PushInt32(std::vector <unsigned char> &buf,int32_t data);
	static void PushUint32(std::vector <unsigned char> &buf,uint32_t data);
	static void PushInt16(std::vector <unsigned char> &buf,int16_t data);
	static void PushUint16(std::vector <unsigned char> &buf,uint16_t data);
	static void PushBool(std::vector <unsigned char> &buf,bool flag);
	static void PushString(std::vector <unsigned char> &buf,std::string str);
	static void PushUcharArray(std::vector <unsigned char> &buf,const std::vector <unsigned char> &data);
	static void PushUcharArray(std::vector <unsigned char> &buf,uint64_t len,const unsigned char data[]);

	static uint64_t ReadUint64(const unsigned char *&data);
	static int64_t ReadInt64(const unsigned char *&data);
	static uint32_t ReadUint32(const unsigned char *&data);
	static int32_t ReadInt32(const unsigned char *&data);
	static uint16_t ReadUint16(const unsigned char *&data);
	static int16_t ReadInt16(const unsigned char *&data);
	static bool ReadBool(const unsigned char *&data);
	static std::string ReadString(const unsigned char *&data);
	static std::vector <unsigned char> ReadUcharArray(const unsigned char *&data);
	static void ReadUcharArray(const unsigned char *&data,uint64_t len,unsigned char buf[]);
};

/* } */
#endif
