/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef DEVICE_IS_INCLUDED
#define DEVICE_IS_INCLUDED
/* { */

#include <vector>
#include <string>

#include "vmbase.h"

class Device
{
protected:
	VMBase *vmPtr;

public:
	const unsigned long long int TIME_NO_SCHEDULE=~0;
	int vmDeviceIndex=-1;  // Index to allDevices in the VM.
	int vmPrevTaskScheduledDeviceIndex=-1,vmNextTaskScheduledDeviceIndex=-1;

	class CommonState
	{
	public:
		unsigned long long int deviceTime;

		/*! Time next task needs to be performed.
		    If no task scheduled, scheduledTime=TIME_NO_SCHEDULE.
		    RunScheduledTask won't be called unless townsTime passes scheduleTime.
		    In RunScheduleTask function for specific device, time should be reset to TIME_NOSCHEDULE
		    if no more task needs to be done.
		    It is set to TIME_NO_SCHEDULE in the constructor.
		*/
		unsigned long long int scheduleTime;

		void PushState(std::vector <unsigned char> &data) const;
		bool ReadState(const unsigned char *&data);
	};

	CommonState commonState;

	Device(VMBase *);

	void Abort(const std::string &abortReason) const;

	virtual const char *DeviceName(void) const=0;

	/*! Default behavior of PowerOn is just call Reset.
	*/
	virtual void PowerOn(void);
	virtual void Reset(void);

	/*! Default behavior is to set deviceTime to TIME_NO_SCHEDULE.
	*/
	virtual void RunScheduledTask(unsigned long long int townsTime);

	virtual void IOWriteByte(unsigned int ioport,unsigned int data);
	virtual void IOWriteWord(unsigned int ioport,unsigned int data); // Default behavior calls IOWriteByte twice
	virtual void IOWriteDword(unsigned int ioport,unsigned int data); // Default behavior calls IOWriteByte 4 times

	virtual unsigned int IOReadByte(unsigned int ioport);
	virtual unsigned int IOReadWord(unsigned int ioport); // Default behavior calls IOReadByte twice
	virtual unsigned int IOReadDword(unsigned int ioport); // Default behavior calls IOWriteByte 4 times

	/*! Machine state format:
	State File Name is may be needed for serializing and de-serializing to search
	a disk/disc image from the state directory.

	+0  DW             DevSize   Total bytes of the device excluding this four bytes (0 means end)
	+4  DevSize bytes  DevData   Serialized device


	DevData
	+0  32 bytes       DevIdent  Device ID String
	+32 DW             Version
	+36 DW             Length in bytes of the common state
	+40                Common State
	+40+x              Data (Length depends on the device)
	*/
	const unsigned int deviceIdLength=32;
	std::vector <unsigned char> Serialize(std::string stateFName) const;
	bool Deserialize(const std::vector <unsigned char> &dat,std::string stateFName);
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

	/*! Version used for serialization.
	*/
	virtual uint32_t SerializeVersion(void) const{return ~0;};
	/*! Device-specific Serialization.
	*/
	virtual void SpecificSerialize(std::vector <unsigned char> &data,std::string stateFName) const{};
	/*! Device-specific De-serialization.
	*/
	virtual bool SpecificDeserialize(const unsigned char *&data,std::string stateFName,uint32_t version){return false;};
};

/* } */
#endif
