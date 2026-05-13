#include <iostream>
#include <string>
#include "virtual_network.h"
#include "real_network.h"
#include "miscutil.h"


////////////////////////////////////////////////////////////

void VirtualNetwork::DHCPOption::CleanUp(void)
{
	msgType=0;
	hostName="";
	paramReqList.clear();
	leaseTimeReq=0xffff;
	requestedIP=0;
}

bool VirtualNetwork::DHCPOption::Decode(size_t len,const uint8_t data[])
{
	CleanUp();
	while(0<len && data[0]!=DHCP_OPTION_END)
	{
		std::cout << cpputil::Ubtox(data[0]) << " " << cpputil::Ubtox(data[1]) << " (" << len << ")\n";

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
		case DHCP_OPTION_REQUESTED_IP_ADDR: // 0x22,
			requestedIP=GetDwordBE(data+2);
			break;
		case DHCP_OPTION_LEASE_TIME_REQ: //0x33,
			leaseTimeReq=GetDwordBE(data+2);
			break;
		case DHCP_OPTION_MESSAGE_TYPE: //0x35,
			msgType=data[2];
			break;
		case DHCP_OPTION_SERVER_IP: //0x36,
			hostIP=GetDwordBE(data+2);
			break;
		case DHCP_OPTION_PARAM_REQ_LIST: //0x37,
			for(int i=0; i<data[1]; ++i)
			{
				paramReqList.push_back(data[2+i]);
			}
			break;
		case DHCP_OPTION_END: //0xFF
			return true;
		default:
			std::cout << "Undefined DHCP option:" << cpputil::Ubtox(data[0]) <<"\n";
			return false;
		}

		auto optlen=data[1];
		len-=(2+optlen);
		data+=(2+optlen);
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

uint16_t VirtualNetwork::CalcIPCheckSum(size_t len,const uint8_t DATA[])
{
	const uint8_t *data=DATA;
	uint32_t sum=0;
	while(2<=len)
	{
		uint16_t word=*(uint16_t *)data;

		sum+=word;
		if(0x10000<=sum)
		{
			++sum;
			sum&=0xFFFF;
		}

		data+=2;
		len-=2;
	}
	if(1==len)
	{
		sum+=*data;
		if(0x10000<=sum)
		{
			++sum;
			sum&=0xFFFF;
		}
	}

	return ((~sum) & 0xFFFF);
}

uint16_t VirtualNetwork::TestTCPCheckSum(size_t len,const uint8_t data[],uint32_t srcIP,uint32_t dstIP)
{
	auto hdr=DecodeTCPHeader(len,data);
	std::vector <uint8_t> allData;

	allData.resize(12);
	PutDwordBE(allData.data()  ,srcIP);
	PutDwordBE(allData.data()+4,dstIP);
	allData[8]=0;
	allData[9]=6; // TCP
	PutWordBE(allData.data()+10,len);

	allData.insert(allData.end(),data,data+len);

	return CalcIPCheckSum(allData.size(),allData.data());
}

VirtualNetwork::EthernetHeader VirtualNetwork::DecodeEthernetHeader(size_t len,const uint8_t data[])
{
	EthernetHeader hdr;
	hdr.dstMAC=GetMAC(data);
	hdr.srcMAC=GetMAC(data+6);
	hdr.type=GetWordBE(data+12);
	return hdr;
}

void VirtualNetwork::AddEthernetHeader(std::vector <uint8_t> &DATA,const EthernetHeader &hdr)
{
	size_t ptr=DATA.size();
	DATA.resize(ptr+ETHER_HEADER_SIZE);

	auto data=DATA.data()+ptr;
	PutMAC(data,hdr.dstMAC);
	PutMAC(data+6,hdr.srcMAC);
	PutWordBE(data+12,hdr.type);
}

void VirtualNetwork::AddIPHeader(std::vector <uint8_t> &data,const IPHeader &hdr)
{
	size_t pos=data.size();
	data.resize(pos+20);
	data[pos]=hdr.version_headerLen;
	data[pos+1]=hdr.QoS;
	PutWordBE(data.data()+pos+2,hdr.len);
	PutWordBE(data.data()+pos+4,hdr.fragID);
	PutWordBE(data.data()+pos+6,hdr.flagOrFragOffset);
	data[pos+8]=hdr.TTL;
	data[pos+9]=hdr.protocol;
	PutWordBE(data.data()+pos+10,0); // Checksum 0 tentatively.
	PutDwordBE(data.data()+pos+12,hdr.srcIP);
	PutDwordBE(data.data()+pos+16,hdr.dstIP);
}

void VirtualNetwork::RecalculateIPHeaderCheckSum(size_t len,uint8_t data[])
{
	for(uint32_t t=0; t<0x10000; ++t) // Stupid. But I'm tired after running 5K today.
	{
		data[10]=t&0xFF;
		data[11]=(t>>8);
		if(0==CalcIPCheckSum(20,data))
		{
			break;
		}
	}
}

VirtualNetwork::ARPHeader VirtualNetwork::DecodeARPHeader(size_t len,const uint8_t data[])
{
	ARPHeader hdr;
	hdr.hardware=GetWordBE(data);
	hdr.protocol=GetWordBE(data+2);
	hdr.hardware_size=data[4];
	hdr.protocol_size=data[5];
	hdr.messageType=GetWordBE(data+6);
	hdr.srcMAC=GetMAC(data+8);
	hdr.srcIP=GetDwordBE(data+14);
	hdr.dstMAC=GetMAC(data+18);
	hdr.dstIP=GetDwordBE(data+24);
	return hdr;
}

void VirtualNetwork::AddARPHeader(std::vector <uint8_t> &DATA,const ARPHeader &hdr)
{
	size_t ptr=DATA.size();
	DATA.resize(ptr+ARP_HEADER_SIZE);

	auto data=DATA.data()+ptr;
	PutWordBE(data   ,hdr.hardware);
	PutWordBE(data+2 ,hdr.protocol);
	data[4]=hdr.hardware_size;
	data[5]=hdr.protocol_size;
	PutWordBE(data+6,hdr.messageType);
	PutMAC(data+8,hdr.srcMAC);
	PutDwordBE(data+14,hdr.srcIP);
	PutMAC(data+18,hdr.dstMAC);
	PutDwordBE(data+24,hdr.dstIP);
}

VirtualNetwork::TCPHeader VirtualNetwork::DecodeTCPHeader(size_t len,const uint8_t data[])
{
	TCPHeader hdr;
	hdr.srcPort=GetWordBE(data);
	hdr.dstPort=GetWordBE(data+2);
	hdr.sequenceNum=GetDwordBE(data+4);
	hdr.ackNum=GetDwordBE(data+8);
	hdr.dataOffset_reservedBits=data[12];
	hdr.flags=data[13];
	hdr.windowSize=GetWordBE(data+14);
	hdr.checkSum=GetWordBE(data+16);
	hdr.urgentPointer=GetWordBE(data+18);
	memcpy(hdr.options,data+20,hdr.GetOptionLength());
	return hdr;
}

void VirtualNetwork::AddTCPHeader(std::vector <uint8_t> &data,TCPHeader &hdr)
{
	size_t totalLen=hdr.GetTotalLength();
	size_t pos=data.size();
	data.resize(pos+totalLen);

	PutWordBE (data.data()+pos  ,hdr.srcPort);
	PutWordBE (data.data()+pos+2,hdr.dstPort);
	PutDwordBE(data.data()+pos+4,hdr.sequenceNum);
	PutDwordBE(data.data()+pos+8,hdr.ackNum);
	data[pos+12]=hdr.dataOffset_reservedBits;
	data[pos+13]=hdr.flags;
	PutWordBE (data.data()+pos+14,hdr.windowSize);
	PutWordBE (data.data()+pos+16,0); // Put checksum 0 tentatively.
	PutWordBE (data.data()+pos+18,hdr.urgentPointer);
	memcpy(data.data()+pos+20,hdr.options,totalLen-20);
}

void VirtualNetwork::RecalculateTCPHeaderCheckSum(size_t len,uint8_t data[],uint32_t srcIP,uint32_t dstIP)
{
	for(uint32_t t=0; t<0x10000; ++t) // It's stupid, but I'm tired after teaching at Japanese school today.
	{
		data[16]=t&0xFF;
		data[17]=t>>8;
		if(0==TestTCPCheckSum(len,data,srcIP,dstIP))
		{
			break;
		}
	}
}

void VirtualNetwork::TransmitPacket(size_t len,const uint8_t data[],PacketReceiver *recv,RealNetwork *realNet)
{
	if(len<36)
	{
		if(true==monitorTX)
		{
			std::cout << "Too short a packet.\n";
		}
		return;
	}

	IPHeader ip;


	size_t totalLen=cpputil::GetWord(data);

	data+=2;
	len-=2;

	// The Ethernet (Layer 2) Header
	EthernetHeader ether=DecodeEthernetHeader(14,data);

	if(true==monitorTX)
	{
		std::cout << "Net TX Packet.\n";
		std::cout << "Src MAC " << cpputil::U64tox(ether.srcMAC) << "\n";
		std::cout << "Dst MAC " << cpputil::U64tox(ether.dstMAC) << "\n";
	}

	data+=14;
	len-=14;

	if(TYPE_IPV4==ether.type)
	{
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
			std::cout << "SRC IP:" << cpputil::Uitox(ip.srcIP) << "DST IP:" << cpputil::Uitox(ip.dstIP) << "\n";
			std::cout << "IPv4 tells that the length is " << ip.len << " bytes.\n";
			std::cout << "What sent from the VM is " << len << " bytes.\n";
		}

		if(len<ip.len)
		{
			return; // Broken packet?
		}
		len=ip.len; // Update to the length from the IP header.

		data+=20;
		len-=20;

		if(0x06==ip.protocol) // TCP
		{
			if(true==monitorTX)
			{
				std::cout << "TCP\n";
			}

			TCPHeader tcp=DecodeTCPHeader(len,data);

			if(monitorTX)
			{
				std::cout << "CheckSumTest:" << cpputil::Ustox(TestTCPCheckSum(len,data,ip.srcIP,ip.dstIP)) << "\n";
			}

			data+=tcp.GetTotalLength();
			len-=tcp.GetTotalLength();

			ProcessTCP_Packet(ether,ip,tcp,len,data,recv,realNet);
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
	else if(TYPE_ARP==ether.type)
	{
		ProcessARP_Packet(ether,len,data,recv);
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

	auto data=DATA.data();

	switch(opt.msgType)
	{
	case 1: // DHCP_MSG_DISCOVER
	case 3: // DHCP_MSG_REQUEST
		// Ether layer
		PutMAC(data  ,ether.srcMAC); // Source MAC is now destination.
		PutMAC(data+6,MAC_DHCP_SERVER);
		data[12]=8; // Ethernet
		data[13]=0;

		// IP layer
		data[14]=0x45; // version=4, header length=5 words (5*4=20 bytes).
		data[15]=0;    // QoS
		PutWordBE(data+16,0x148); // Length : Is the length always 0x148?
		PutWordBE(data+18,0); // Fragment ID
		PutWordBE(data+20,0); // Fragment Offset or Flags  (Looks like 0xC000 for flags, 0x3FFF for offset)
		data[22]=0x40; // TTL
		data[23]=0x11; // UDP
		PutWordBE(data+24,0); // Checksum to be filled.
		PutDwordBE(data+26,DHCP_SERVER_IP); // Src IP
		PutDwordBE(data+30,VM_DHCP_IP);     // Dst IP

		RecalculateIPHeaderCheckSum(20,data+14);

		// UDP
		PutWordBE(data+34,DHCP_SERVER_PORT);
		PutWordBE(data+36,DHCP_CLIENT_PORT);
		PutWordBE(data+38,0x134); // Length
		PutWordBE(data+40,0);     // UDP checksum is optional according to PD3586
		data[42]=2; // Is this same as DHCP_MSG_REPLY?
		data[43]=1; // net type
		data[44]=6; // 6-byte MAC address
		data[45]=0; // hop
		PutDwordBE(data+46,udp.transactionID);
		PutDwordBE(data+58,VM_DHCP_IP);
		PutDwordBE(data+62,DHCP_SERVER_IP);
		PutMAC(data+70,ether.srcMAC);

		// UDP Payload
		{
			uint8_t replyMsgType=(1==opt.msgType ? DHCP_MSG_REPLY : DHCP_MSG_ACK);

			uint32_t ptr=0x116;
			PutDwordBE(data+ptr,0x63825363);
			ptr+=4;
			data[ptr++]=DHCP_OPTION_MESSAGE_TYPE; // 0x35
			data[ptr++]=1;  // 1 byte
			data[ptr++]=replyMsgType;

			data[ptr++]=DHCP_OPTION_SERVER_IP;
			data[ptr++]=4; // 4 bytes
			PutDwordBE(data+ptr,DHCP_SERVER_IP);
			ptr+=4;

			data[ptr++]=DHCP_OPTION_LEASE_TIME_REQ;
			data[ptr++]=4;
			PutDwordBE(data+ptr,opt.leaseTimeReq);
			ptr+=4;

			data[ptr++]=DHCP_OPTION_SUBNET_MASK;
			data[ptr++]=4;
			PutDwordBE(data+ptr,0xFFFFFF00);
			ptr+=4;

			data[ptr++]=DHCP_OPTION_DEF_ROUTER;
			data[ptr++]=4;
			PutDwordBE(data+ptr,ROUTER_IP);
			ptr+=4;

			data[ptr++]=DHCP_OPTION_DNS_SERVER;
			data[ptr++]=4;
			PutDwordBE(data+ptr,DNS_IP);
			ptr+=4;

			data[ptr++]=DHCP_OPTION_END;

			DATA.resize(ptr);
		}

		break;
	case 2:
		std::cout << "Message Type 2 is not supposed to come from the client.\n";
		break;
	case 4:
		std::cout << "Unknown Message Type.\n";
		break;
	}

	return DATA;
}

void VirtualNetwork::ProcessARP_Packet(EthernetHeader ether,size_t len,const uint8_t data[],PacketReceiver *recv)
{
	ARPHeader arp=DecodeARPHeader(len,data);
	if(arp.dstIP==ROUTER_IP)
	{
		// Asking ROUTER IP?
		ether.dstMAC=ether.srcMAC;
		ether.srcMAC=MAC_ROUTER;

		arp.messageType=2; // Reply
		arp.srcMAC=MAC_ROUTER;
		arp.srcIP=ROUTER_IP;
		arp.dstMAC=MAC_ROUTER;
		arp.dstIP=ROUTER_IP;

		std::vector <uint8_t> data;
		AddEthernetHeader(data,ether);
		AddARPHeader(data,arp);
		recv->ReceivePacket(data.size(),data.data());
	}
	else if(VM_DHCP_IP==arp.dstIP && VM_DHCP_IP==arp.srcIP)
	{
		// Stay silent.  Do not reply.
	}
	else
	{
		// Neet to get from outside.
	}
}

void VirtualNetwork::ProcessTCP_Packet(EthernetHeader ether,IPHeader ip,TCPHeader tcp,size_t len,const uint8_t data[],PacketReceiver *recv,RealNetwork *realNet)
{
	if(tcp.flags&TCP_FLAG_SYN)
	{
		if(monitorTX)
		{
			std::cout << "SYN\n";
		}

		TCPConnection conn;
		conn.ethernetHdr=ether;
		conn.ipHdr=ip;
		conn.tcpHdr=tcp;
		conn.state=STATE_PENDING;
		TCPConn.push_back(conn);

		// Not really, but pretend the connection was successful.
		if(ip.dstIP==DHCP_SERVER_IP ||
		   ip.dstIP==ROUTER_IP ||
		   ip.dstIP==DNS_IP)
		{
			TCPConnectionEstablished(TCPConn.back(),recv);
		}
		else
		{
			uint8_t IP[4]=
			{
				ip.dstIP>>24,
				ip.dstIP>>16,
				ip.dstIP>>8,
				ip.dstIP
			};
			realNet->RequestTCPConnection(tcp.srcPort,IP,tcp.dstPort);
		}
	}
}

void  VirtualNetwork::TCPConnectionEstablished(TCPConnection &conn,PacketReceiver *recv)
{
	conn.state=STATE_ESTABLISHED;

	std::swap(conn.ethernetHdr.srcMAC,conn.ethernetHdr.dstMAC);
	std::swap(conn.ipHdr.srcIP,conn.ipHdr.dstIP);
	std::swap(conn.tcpHdr.srcPort,conn.tcpHdr.dstPort);

	auto ether=conn.ethernetHdr;
	auto ip=conn.ipHdr;
	auto tcp=conn.tcpHdr;

	tcp.flags|=TCP_FLAG_ACK;
	tcp.ackNum=tcp.sequenceNum+1;
	tcp.sequenceNum=sequenceNumSource++;

	std::vector <uint8_t> data;
	AddEthernetHeader(data,ether);

	for(auto str : miscutil::MakeDump(data.size(),data.data()))
	{
		std::cout << str << "\n";
	}
	std::cout << "--1--\n";

	size_t IPHeaderPos=data.size();
	AddIPHeader(data,ip);

	for(auto str : miscutil::MakeDump(data.size(),data.data()))
	{
		std::cout << str << "\n";
	}
	std::cout << "--2--\n";

	RecalculateIPHeaderCheckSum(20,data.data()+IPHeaderPos);

	for(auto str : miscutil::MakeDump(data.size(),data.data()))
	{
		std::cout << str << "\n";
	}
	std::cout << "--3--\n";

	size_t TCPHeaderPos=data.size();
	AddTCPHeader(data,tcp);
	for(auto str : miscutil::MakeDump(data.size(),data.data()))
	{
		std::cout << str << "\n";
	}
	std::cout << "--4--\n";
	RecalculateTCPHeaderCheckSum(data.size()-TCPHeaderPos,data.data()+TCPHeaderPos,ip.srcIP,ip.dstIP);

	for(auto str : miscutil::MakeDump(data.size(),data.data()))
	{
		std::cout << str << "\n";
	}
	std::cout << "--5--\n";

	recv->ReceivePacket(data.size(),data.data());
}

void VirtualNetwork::Polling(PacketReceiver *recv,class RealNetwork *realNet)
{
	std::lock_guard <std::mutex> lock(realNet->clientsLock);
	for(auto &cli : realNet->clients)
	{
		if(RealNetwork::STATE_JUST_CONNECTED==cli.state)
		{
			cli.state=RealNetwork::STATE_CONNECTED;

			if(true==monitorTCP)
			{
				std::cout << "Connection Established\n";
			}

			uint32_t IPv4=0;
			IPv4=cli.conn.IPv4Addr[0];
			IPv4<<=8;
			IPv4|=cli.conn.IPv4Addr[1];
			IPv4<<=8;
			IPv4|=cli.conn.IPv4Addr[2];
			IPv4<<=8;
			IPv4|=cli.conn.IPv4Addr[3];
			for(auto &virConn : TCPConn)
			{
				if(virConn.ipHdr.dstIP==IPv4 &&
				   virConn.tcpHdr.srcPort==cli.conn.VMPort)
				{
					TCPConnectionEstablished(virConn,recv);
				}
			}
		}
		else if(cli.state==RealNetwork::STATE_CONNECTED && 0<cli.recvBuf.size())
		{
			// Packet came in.
		}
	}
}
