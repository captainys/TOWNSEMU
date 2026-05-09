#ifndef VIRTUAL_NETWORK_H_IS_INCLUDED
#define VIRTUAL_NETWORK_H_IS_INCLUDED

#include "cpputil.h"

class VirtualNetwork
{
public:
	class PacketReceiver
	{
	public:
		// Virtual Network -> Adapter
		virtual void ReceivePacket(size_t len,const uint8_t data[])=0;
	};

	// Adapter -> Virtual Network
	void TransmitPacket(size_t len,const uint8_t data[]);

	// Polling.
	void Poll(PacketReceiver *recv);
};


#endif
