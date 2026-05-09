#include <iostream>
#include "virtual_network.h"


////////////////////////////////////////////////////////////

void VirtualNetwork::DHCPOption::CleanUp(void)
{
	msgType=0;
	hostName="";
	paramReqList.clear();
	leaseTimeReq=0xffff;
}

bool VirtualNetwork::DHCPOption::Decode(size_t len,const uint8_t data[])
{
	CleanUp();
	while(0<len)
	{
		if(len<2+data[1])
		{
			std::cout << "DHCP Option Overflow.\n";
			return false;
		}

		switch(data[0])
		{
		case DHCP_OPTION_HOSTNAME: //0x0C,
			for(int i=0; i<data[1]; ++i)
			{
				hostName.push_back(data[2+i]);
			}
			break;
		case DHCP_OPTION_MESSAGE_TYPE: //0x35,
			msgType=data[2];
			break;
		case DHCP_OPTION_PARAM_REQ_LIST: //0x37,
			for(int i=0; i<data[1]; ++i)
			{
				paramReqList.push_back(data[2+i]);
			}
			break;
		case DHCP_OPTION_LEASE_TIME_REQ: //0x33,
			leaseTimeReq=GetDwordBE(data+2);
			break;
		case DHCP_OPTION_END: //0xFF
			return true;
		default:
			std::cout << "Undefined DHCP option:" << cpputil::Ubtox(data[0]) <<"\n";
			return false;
		}

		data+=(2+data[1]);
		len-=(2+data[1]);
	}
	return true;
}

////////////////////////////////////////////////////////////

uint64_t VirtualNetwork::GetMAC(const uint8_t data[])
{
	uint64_t i;
	i=data[0];
	i<<=8;
	i|=data[1];
	i<<=8;
	i|=data[2];
	i<<=8;
	i|=data[3];
	i<<=8;
	i|=data[4];
	i<<=8;
	i|=data[5];
	return i;
}

uint16_t VirtualNetwork::GetWordBE(const uint8_t data[])
{
	return (data[0]<<8)|data[1];
}

uint32_t VirtualNetwork::GetDwordBE(const uint8_t data[])
{
	return (data[0]<<24)|(data[1]<<16)|(data[2]<<8)|data[3];
}

void VirtualNetwork::PutMAC(uint8_t data[],uint64_t MAC)
{
	data[5]=MAC;
	MAC>>=8;
	data[4]=MAC;
	MAC>>=8;
	data[3]=MAC;
	MAC>>=8;
	data[2]=MAC;
	MAC>>=8;
	data[1]=MAC;
	MAC>>=8;
	data[0]=MAC;
}
void VirtualNetwork::PutWordBE(uint8_t data[],uint16_t i)
{
	data[1]=i;
	data[0]=i>>8;
}
void VirtualNetwork::PutDwordBE(uint8_t data[],uint32_t i)
{
	data[3]=i;
	data[2]=i>>8;
	data[1]=i>>16;
	data[0]=i>>24;
}

void VirtualNetwork::TransmitPacket(size_t len,const uint8_t data[],PacketReceiver *recv)
{
	if(len<36)
	{
		if(true==monitorTX)
		{
			std::cout << "Too short a packet.\n";
		}
		return;
	}

	EthernetHeader ether;
	IPHeader ip;


	size_t totalLen=cpputil::GetWord(data);

	data+=2;
	len-=2;

	// The Ethernet (Layer 2) Header
	ether.dstMAC=GetMAC(data);
	ether.srcMAC=GetMAC(data+6);
	ether.type=data[12];

	if(true==monitorTX)
	{
		std::cout << "Net TX Packet.\n";
		std::cout << "Src MAC " << cpputil::U64tox(ether.srcMAC) << "\n";
		std::cout << "Dst MAC " << cpputil::U64tox(ether.dstMAC) << "\n";
	}

	// Is data[13] unused?

	data+=14;
	len-=14;

	// IP (Layer 3) Header
	ip.version_headerLen=data[0];
	ip.QoS=data[1];
	ip.len=GetWordBE(data+2);
	ip.fragID=GetWordBE(data+4);
	ip.flagOrFragOffset=GetWordBE(data+6);
	ip.TTL=data[8];
	ip.protocol=data[9];  // 06H for TCP  11H for UDP
	ip.checkSum=GetWordBE(data+10);
	ip.srcIP=GetDwordBE(data+12);
	ip.dstIP=GetDwordBE(data+16);

	if(true==monitorTX)
	{
		std::cout << "SRC IP:" << cpputil::Uitox(ip.srcIP) << "DST IP:" << cpputil::Uitox(ip.dstIP) << "\n";;
	}

	data+=20;
	len-=20;

	if(0x06==ip.protocol) // TCP
	{
		if(true==monitorTX)
		{
			std::cout << "TCP\n";
		}
	}
	else if(0x11==ip.protocol) // UDP
	{
		UDPHeader udp;

		udp.srcPort=GetWordBE(data);
		udp.dstPort=GetWordBE(data+2);
		udp.len=GetWordBE(data+4);
		udp.checkSum=GetWordBE(data+6);
		udp.messageType=data[8];
		udp.netType=data[9];
		udp.MAClen=data[10];
		udp.hop=data[11];
		udp.transactionID=GetDwordBE(data+12);
		// From +16 to +36 are vague.  Prob Seconds Elapsed(word), BOOTP flags(word), ClientIP(DW), MyIP(DW), ServerIP(DW), RelayIP(DW).
		udp.MAC=GetMAC(data+36);

		if(true==monitorTX)
		{
			std::cout << "UDP SRCPort: " << cpputil::Ustox(udp.srcPort) << " DSTPort:" << cpputil::Ustox(udp.dstPort) << "\n";
		}

		if(0xFFFFFFFF==ip.dstIP && DHCP_CLIENT_PORT==udp.srcPort && DHCP_SERVER_PORT==udp.dstPort) // Broadcast and DHCP port
		{
			// Must be DHCP
			uint32_t magicNumber=GetDwordBE(data+0xF4);
			if(0x63825363==magicNumber)
			{
				if(true==monitorTX)
				{
					std::cout << "DHCP Packet\n";
				}
				ProcessUDP_DHCP_Packet(ether,ip,udp,len,data,recv);
			}
		}
	}
}

void VirtualNetwork::ProcessUDP_DHCP_Packet(
    EthernetHeader ether,IPHeader ip,UDPHeader udp,size_t len,const uint8_t data[],PacketReceiver *recv)
{
	DHCPOption opt;
	if(true==opt.Decode(len-0xF8,data+0xF8))
	{
		auto DHCPReturn=MakeDHCPReturnPacket(ether,ip,udp,opt);
		if(0<DHCPReturn.size())
		{
			recv->ReceivePacket(DHCPReturn.size(),DHCPReturn.data());
		}
	}
	else
	{
		if(true==monitorTX)
		{
			std::cout << "DHCP Option Error.\n";
		}
	}
}

std::vector <uint8_t> VirtualNetwork::MakeDHCPReturnPacket(EthernetHeader ether,IPHeader ip,UDPHeader udp,DHCPOption opt)
{
	std::vector <uint8_t> DATA;
	DATA.resize(512);

	auto data=DATA.data()+2;

	switch(opt.msgType)
	{
	case 1: // DHCP_MSG_DISCOVER
		// Ether layer
		PutMAC(data  ,ether.srcMAC); // Source MAC is now destination.
		PutMAC(data+6,MAC_DHCP_SERVER);
		data[12]=8; // Ethernet
		data[13]=0;

		// IP layer
		data[14]=0x45; // version=4, header length=5 words.
		data[15]=0;    // QoS
		PutWordBE(data+16,0x148); // Length : Is the length always 0x148?
		PutWordBE(data+18,0); // Fragment ID
		PutWordBE(data+20,0); // Fragment Offset or Flags
		data[22]=0x40; // TTL
		data[23]=0x11; // UDP
		PutWordBE(data+24,0); // Checksum to be filled.
		PutDwordBE(data+26,DHCP_SERVER_IP); // Src IP
		PutDwordBE(data+30,VM_DHCP_IP);     // Dst IP

		// UDP
		PutWordBE(data+34,DHCP_SERVER_PORT);
		PutWordBE(data+36,DHCP_CLIENT_PORT);
		PutWordBE(data+38,0x134); // Length
		PutWordBE(data+40,0);     // Checksum to be filled.
		data[42]=2; // Is this same as DHCP_MSG_REPLY?
		data[43]=1; // net type
		data[44]=6; // 6-byte MAC address
		data[45]=0; // hop
		PutDwordBE(data+46,udp.transactionID);
		PutDwordBE(data+58,VM_DHCP_IP);
		PutDwordBE(data+62,DHCP_SERVER_IP);
		PutMAC(data+70,ether.srcMAC);

		// UDP Payload
		PutDwordBE(data+0x118,0x63825363);
		data[0x11C]=DHCP_OPTION_MESSAGE_TYPE; // 0x35
		data[0x11D]=1;  // 1 byte
		data[0x11E]=DHCP_MSG_REPLY;

		data[0x11F]=DHCP_OPTION_DNS_SERVER;
		data[0x120]=4; // 4 bytes
		PutDwordBE(data+0x121,DHCP_SERVER_IP);

		data[0x125]=DHCP_OPTION_LEASE_TIME_REQ;
		data[0x126]=4;
		PutDwordBE(data+0x127,opt.leaseTimeReq);

		data[0x12B]=DHCP_OPTION_SUBNET_MASK;
		data[0x12C]=4;
		PutDwordBE(data+0x12D,0xFFFFFF00);

		data[0x131]=DHCP_OPTION_DEF_ROUTER;
		data[0x132]=4;
		PutDwordBE(data+0x133,ROUTER_IP);

		data[0x137]=DHCP_OPTION_DNS_SERVER;
		data[0x138]=4;
		PutDwordBE(data+0x139,DNS_IP);

		data[0x13D]=DHCP_OPTION_END;

		cpputil::PutWord(DATA.data(),0x13E);

		break;
	case 3: // DHCP_MSG_REQUEST
		break;
	case 4:
		std::cout << "Unknown Message Type.\n";
		break;
	}

	return DATA;
}
