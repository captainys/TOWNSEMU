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
		case DHCP_OPTION_MAC_ADDRESS: // 0x3D
			{
				// Should be same as the Ethernet header's MAC address.
			}
			break;
		case DHCP_OPTION_END: //0xFF
			return true;
		default:
			std::cout << "Warning: Undefined DHCP option:" << cpputil::Ubtox(data[0]) <<"\n";
			break;
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

std::string VirtualNetwork::FormatIPAddress(uint32_t IP)
{
	std::string str;
	str=cpputil::Uitoa(IP>>24);
	str.push_back('.');
	str+=cpputil::Uitoa((IP>>16)&255);
	str.push_back('.');
	str+=cpputil::Uitoa((IP>>8)&255);
	str.push_back('.');
	str+=cpputil::Uitoa(IP&255);
	return str;
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

void VirtualNetwork::AddUDPHeader(std::vector <uint8_t> &data,const UDPHeader &udp)
{
	size_t ptr=data.size();
	data.resize(ptr+8);
	PutWordBE(data.data()+ptr,udp.srcPort);
	PutWordBE(data.data()+ptr+2,udp.dstPort);
	PutWordBE(data.data()+ptr+4,udp.len);
	PutWordBE(data.data()+ptr+6,udp.checkSum);
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

void VirtualNetwork::AddStatusText(std::vector <std::string> &text) const
{
	for(auto &conn : TCPConn)
	{
		std::string str;
		str="TCP VMPort:"+cpputil::Ustox(conn.tcpHdr.dstPort)+"H RemoteIP:"+cpputil::Uitox(conn.ipHdr.srcIP)+"H RemotePort:"+cpputil::Ustox(conn.tcpHdr.srcPort)+" ";
		switch(conn.state)
		{
		case STATE_PENDING:
			str+="PENDING";
			break;
		case STATE_ESTABLISHED:
			str+="EST";
			break;
		case STATE_CLOSING_FROM_ROUTER:
			str+="CLOSING_FROM_REMOTE";
			break;
		case STATE_FIN_SENT: // Closing from the remote, and FIN|ACK has been sent to the VM.
			str+="CLOSING_FROM_REMOTE";
			break;
		case STATE_CLOSED:
			str+="CLOSED";
			break;
		case STATE_FIN_RECEIVED: // VM initiated FIN.
			str+="CLOSING_FROM_VM";
			break;
		}

		str.push_back(' ');
		str+="TxBuf:"+cpputil::Uitox(conn.TxData.size())+"H bytes";
		text.push_back(str);
	}
	for(auto &req : DNSReq)
	{
		std::string str;
		str="DNS Req Hostname="+req.hostname;
		str.push_back(' ');
		switch(req.state)
		{
 		case DNS_REQUESTED:
			str+="REQUESTED";
			break;
		case DNS_FOUND:
			str+="FOUND";
			break;
		case DNS_NOT_FOUND:
			str+="NOT_FOUND";
			break;
		}
		text.push_back(str);
	}
}


void VirtualNetwork::TransmitPacket(size_t len,const uint8_t data[],PacketReceiver *recv,RealNetwork *realNet)
{
	if(len<34)
	{
		if(true==monitorTX)
		{
			std::cout << "Too short a packet.\n";
		}
		return;
	}

	IPHeader ip;

	// The Ethernet (Layer 2) Header
	EthernetHeader ether=DecodeEthernetHeader(14,data);

	if(true==monitorTX)
	{
		std::cout << "Net TX Packet (VM->Router) "  << "SrcMAC " << cpputil::U64tox(ether.srcMAC) << " " << "DstMAC " << cpputil::U64tox(ether.dstMAC) << "\n";
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
			std::cout << "SRC IP:" << cpputil::Uitox(ip.srcIP) << "DST IP:" << cpputil::Uitox(ip.dstIP) << " " << len << "bytes\n";
			if(ip.len!=len)
			{
				std::cout << "IPv4 tells that the length is " << ip.len << " bytes.\n";
				std::cout << "What sent from the VM is " << len << " bytes.\n";
			}
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
			TCPHeader tcp=DecodeTCPHeader(len,data);

			if(monitorTX)
			{
				std::cout << "TCP  CheckSumTest:" << cpputil::Ustox(TestTCPCheckSum(len,data,ip.srcIP,ip.dstIP)) << "\n";
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

			if(true==monitorTX)
			{
				std::cout << "UDP SRCPort: " << cpputil::Ustox(udp.srcPort) << " DSTPort:" << cpputil::Ustox(udp.dstPort) << "\n";
			}

			if(0xFFFFFFFF==ip.dstIP && DHCP_CLIENT_PORT==udp.srcPort && DHCP_SERVER_PORT==udp.dstPort) // Broadcast and DHCP port
			{
				DHCPInfo dhcp;
				dhcp.messageType=data[8];
				dhcp.netType=data[9];
				dhcp.MAClen=data[10];
				dhcp.hop=data[11];
				dhcp.transactionID=GetDwordBE(data+12);
				// From +16 to +36 are vague.  Prob Seconds Elapsed(word), BOOTP flags(word), ClientIP(DW), MyIP(DW), ServerIP(DW), RelayIP(DW).
				dhcp.MAC=GetMAC(data+36);

				// Must be DHCP
				uint32_t magicNumber=GetDwordBE(data+0xF4);
				if(0x63825363==magicNumber)
				{
					if(true==monitorTX)
					{
						std::cout << "DHCP Packet\n";
					}
					ProcessUDP_DHCP_Packet(ether,ip,udp,dhcp,len,data,recv);
				}
			}
			else if(DNS_IP==ip.dstIP && DNS_SERVER_PORT==udp.dstPort)
			{
				if(true==monitorDNS)
				{
					std::cout << "UDP Packet to DNS.\n";
				}
				len-=8;
				data+=8;
				ProcessUDP_DNS_Packet(ether,ip,udp,len,data,realNet);
			}
		}
	}
	else if(TYPE_ARP==ether.type)
	{
		ProcessARP_Packet(ether,len,data,recv);
	}
}

void VirtualNetwork::ProcessUDP_DHCP_Packet(
    EthernetHeader ether,IPHeader ip,UDPHeader udp,DHCPInfo dhcp,size_t len,const uint8_t data[],PacketReceiver *recv)
{
	DHCPOption opt;
	if(true==opt.Decode(len-0xF8,data+0xF8))
	{
		auto DHCPReturn=MakeDHCPReturnPacket(ether,ip,udp,dhcp,opt);
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

std::vector <uint8_t> VirtualNetwork::MakeDHCPReturnPacket(EthernetHeader ether,IPHeader ip,UDPHeader udp,DHCPInfo dhcp,DHCPOption opt)
{
	std::vector <uint8_t> DATA;
	DATA.resize(512);

	size_t IPHeaderPos=14;
	size_t UDPHeaderPos=34;
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
		IPHeaderPos=14;
		data[14]=0x45; // version=4, header length=5 words (5*4=20 bytes).
		data[15]=0;    // QoS
		PutWordBE(data+16,0); // Length : Tentative.  Should be the total length minus IPHeaderPos.
		PutWordBE(data+18,0); // Fragment ID
		PutWordBE(data+20,0); // Fragment Offset or Flags  (Looks like 0xC000 for flags, 0x3FFF for offset)
		data[22]=0x40; // TTL
		data[23]=0x11; // UDP
		PutWordBE(data+24,0); // Checksum to be filled.
		PutDwordBE(data+26,DHCP_SERVER_IP); // Src IP
		PutDwordBE(data+30,VM_DHCP_IP);     // Dst IP

		// UDP
		UDPHeaderPos=34;
		PutWordBE(data+34,DHCP_SERVER_PORT);
		PutWordBE(data+36,DHCP_CLIENT_PORT);
		PutWordBE(data+38,0); // Length: Tentative.
		PutWordBE(data+40,0);     // UDP checksum is optional according to PD3586
		data[42]=2; // Is this same as DHCP_MSG_REPLY?
		data[43]=1; // net type
		data[44]=6; // 6-byte MAC address
		data[45]=0; // hop
		PutDwordBE(data+46,dhcp.transactionID);
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


			PutWordBE(data+38,ptr-UDPHeaderPos); // UDP Length: Header + Payload

			size_t IPv4Len=ptr-IPHeaderPos;
			PutWordBE(data+16,IPv4Len);
			RecalculateIPHeaderCheckSum(20,data+IPHeaderPos); // Now I can calculate IP checksum.


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

void VirtualNetwork::ProcessUDP_DNS_Packet(EthernetHeader ether,IPHeader ip,UDPHeader udp,size_t len,const uint8_t data[],RealNetwork *realNet)
{
	std::string hostname;
	size_t ptr=12;
	while(ptr<len && 0!=data[ptr])
	{
		if(0<hostname.size())
		{
			hostname.push_back('.');
		}

		size_t count=data[ptr++];
		while(ptr<len && 0<count)
		{
			hostname.push_back(data[ptr++]);
			--count;
		}
	}

	if(true==monitorDNS)
	{
		std::cout << "DNS Inquiry:" << hostname << "\n";
	}

	realNet->RequestDNS(hostname);

	DNSRequest req;
	req.ethernetHdr=ether;
	req.ipHdr=ip;
	req.udpHdr=udp;
	req.udpPayload.insert(req.udpPayload.end(),data,data+len);
	req.hostname=hostname;
	DNSReq.push_back(req);
}

void VirtualNetwork::SendDNSReplyFoundToVM(DNSRequest &req,PacketReceiver *recv)
{
	auto ether=req.ethernetHdr;
	auto ip=req.ipHdr;
	auto udp=req.udpHdr;

	std::swap(ether.srcMAC,ether.dstMAC);
	std::swap(ip.srcIP,ip.dstIP);
	std::swap(udp.srcPort,udp.dstPort);

	udp.checkSum=0; // Decline to calculate checksum.

	std::vector <uint8_t> data;
	AddEthernetHeader(data,ether);

	size_t ipHeaderPos=data.size();
	AddIPHeader(data,ip);

	size_t udpHeaderPos=data.size();
	AddUDPHeader(data,udp);

	size_t udpPayloadPos=data.size();
	data.insert(data.end(),req.udpPayload.begin(),req.udpPayload.end());

	data[udpPayloadPos+2]=0x81; // Standard Response, No Error.
	data[udpPayloadPos+3]=0x80;

	data[udpPayloadPos+7]=1;  // One answer

	data.push_back(0xC0); // Pointer to the name in the original inquiry.
	data.push_back(0x0C);

	data.push_back(0); // Type A
	data.push_back(1);

	data.push_back(0); // Class IN
	data.push_back(1);

	data.push_back(0); // TTL
	data.push_back(0);
	data.push_back(0);
	data.push_back(60); // 60sec

	data.push_back(0); // Data length
	data.push_back(4);

	data.push_back(req.ipAddr[0]);
	data.push_back(req.ipAddr[1]);
	data.push_back(req.ipAddr[2]);
	data.push_back(req.ipAddr[3]);

	size_t ipLen=data.size()-ipHeaderPos;
	PutWordBE(data.data()+ipHeaderPos+2,ipLen);
	RecalculateIPHeaderCheckSum(ip.GetHeaderLength(),data.data()+ipHeaderPos);

	size_t udpLen=data.size()-udpHeaderPos;
	PutWordBE(data.data()+udpHeaderPos+4,udpLen);

	recv->ReceivePacket(data.size(),data.data());
}

void VirtualNetwork::SendDNSReplyNotFoundToVM(DNSRequest &req,PacketReceiver *recv)
{
	auto ether=req.ethernetHdr;
	auto ip=req.ipHdr;
	auto udp=req.udpHdr;

	udp.checkSum=0; // Decline to calculate checksum.

	std::swap(ether.srcMAC,ether.dstMAC);
	std::swap(ip.srcIP,ip.dstIP);
	std::swap(udp.srcPort,udp.dstPort);

	std::vector <uint8_t> data;
	AddEthernetHeader(data,ether);

	size_t ipHeaderPos=data.size();
	AddIPHeader(data,ip);

	size_t udpHeaderPos=data.size();
	AddUDPHeader(data,udp);

	size_t udpPayloadPos=data.size();
	data.insert(data.end(),req.udpPayload.begin(),req.udpPayload.end());

	data[udpPayloadPos+2]=0x81; // Standard Response
	data[udpPayloadPos+3]=0x83; // NXDOMAIN Error

	data[udpPayloadPos+7]=0;  // Zero answer

	size_t ipLen=data.size()-ipHeaderPos;
	PutWordBE(data.data()+ipHeaderPos+2,ipLen);
	RecalculateIPHeaderCheckSum(ip.GetHeaderLength(),data.data()+ipHeaderPos);

	size_t udpLen=data.size()-udpHeaderPos;
	PutWordBE(data.data()+udpHeaderPos+4,udpLen);

	recv->ReceivePacket(data.size(),data.data());
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
	TCPConnection *thisConn=nullptr;

	if(monitorTX)
	{
		std::cout << "TCP Packet VM->Router  Port:" << uint32_t(tcp.srcPort) << "->" << uint32_t(tcp.dstPort);
		std::cout << " IP:" << FormatIPAddress(ip.srcIP) << "->" << FormatIPAddress(ip.dstIP) << "\n";
	}

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

		// Swap src and dst to use it for packet back to the VM.
		std::swap(conn.ethernetHdr.srcMAC,conn.ethernetHdr.dstMAC);
		std::swap(conn.ipHdr.srcIP,conn.ipHdr.dstIP);
		std::swap(conn.tcpHdr.srcPort,conn.tcpHdr.dstPort);

		for(auto &existing : TCPConn)
		{
			if(ip.dstIP==existing.ipHdr.srcIP &&  // Src & Dst are flipped.
			   tcp.dstPort==existing.tcpHdr.srcPort &&
			   tcp.srcPort==existing.tcpHdr.dstPort)
			{
				// Port already exists.
				if(true==monitorTCP)
				{
					std::cout << "SYN requested for already-existing connection.\n";
				}
				existing=conn;
				thisConn=&existing;
				break;
			}
		}
		
		if(nullptr==thisConn)
		{
			TCPConn.push_back(conn);
			thisConn=&TCPConn.back();
		}

		// Not really, but pretend the connection was successful.
		if(ip.dstIP==DHCP_SERVER_IP ||
		   ip.dstIP==ROUTER_IP ||
		   ip.dstIP==DNS_IP)
		{
			TCPConnectionEstablished(*thisConn,recv);
		}
		else
		{
			uint8_t IP[4]=
			{
				uint8_t(ip.dstIP>>24),
				uint8_t(ip.dstIP>>16),
				uint8_t(ip.dstIP>>8),
				uint8_t(ip.dstIP)
			};
			realNet->RequestTCPConnection(tcp.srcPort,IP,tcp.dstPort);
		}
	}
	if(tcp.flags&TCP_FLAG_ACK) // ACK VM->Router
	{
		for(auto &conn : TCPConn)
		{
			if(conn.ipHdr.dstIP==ip.srcIP &&
			   conn.tcpHdr.srcPort==tcp.dstPort && // Remote (Outside) Port
			   conn.tcpHdr.dstPort==tcp.srcPort)   // Local (VM) Port
			{
				thisConn=&conn;
				if(true==monitorTCP)
				{
					std::cout << "Ack from VM  " << "  Seq#=" << cpputil::Uitox(tcp.sequenceNum) << "  Ack#=" << cpputil::Uitox(tcp.ackNum) << "\n";
					std::cout << "Current      " << "  Seq#=" << cpputil::Uitox(conn.tcpHdr.sequenceNum) << "  Ack#=" << cpputil::Uitox(conn.tcpHdr.ackNum) << "\n";
				}
				if(true==conn.waitingAck && conn.unacknowledgedSeq==tcp.ackNum)
				{
					if(true==monitorTCP)
					{
						std::cout << "VM Acknowledged " << cpputil::Uitox(tcp.ackNum) << "\n";
					}
					conn.waitingAck=false;
				}
				break;
			}
		}
	}
	if(tcp.flags&TCP_FLAG_PSH)
	{
		for(auto &conn : TCPConn)
		{
			if(conn.ipHdr.dstIP==ip.srcIP &&
			   conn.tcpHdr.srcPort==tcp.dstPort && // Remote (Outside) port
			   conn.tcpHdr.dstPort==tcp.srcPort)   // Local (VM) port
			{
				++conn.ipHdr.fragID; // Is it necessary?

				if(true==monitorTCP)
				{
					std::cout << "Acknowledging PSH from VM Incoming seq=" << cpputil::Uitox(tcp.sequenceNum) << " ack=" << cpputil::Uitox(tcp.ackNum) << "\n";
					std::cout << "                          Current  seq=" << cpputil::Uitox(conn.tcpHdr.sequenceNum) << " ack=" << cpputil::Uitox(conn.tcpHdr.ackNum) << "\n";
				}

				size_t dataLen=ip.len-ip.GetHeaderLength()-tcp.GetTotalLength();
				conn.tcpHdr.ackNum+=dataLen;

				{
					//Acknowledge PSH
					auto tcp=conn.tcpHdr;
					auto ip=conn.ipHdr;
					
					tcp.flags=TCP_FLAG_ACK;
					tcp.StripOptions();

					ip.len=ip.GetHeaderLength()+tcp.GetTotalLength();

					std::vector <uint8_t> data;
					AddEthernetHeader(data,conn.ethernetHdr);

					size_t IPHeaderPos=data.size();
					AddIPHeader(data,ip);
					RecalculateIPHeaderCheckSum(20,data.data()+IPHeaderPos);

					size_t TCPHeaderPos=data.size();
					AddTCPHeader(data,tcp);
					// No payload.
					RecalculateTCPHeaderCheckSum(data.size()-TCPHeaderPos,data.data()+TCPHeaderPos,ip.srcIP,ip.dstIP);

					recv->ReceivePacket(data.size(),data.data());

					// Apparently ACK packet with no data doesn't increment the sequence number.
				}

				conn.TxData.insert(conn.TxData.end(),data,data+dataLen);
			}
		}
	}
	if(TCP_FLAG_FIN&tcp.flags)
	{
		for(auto iter=TCPConn.begin(); TCPConn.end()!=iter;)
		{
			auto &conn=*iter;
			bool deleted=false;
			if(conn.ipHdr.dstIP==ip.srcIP &&
			   conn.tcpHdr.srcPort==tcp.dstPort && // Remote (Outside) port
			   conn.tcpHdr.dstPort==tcp.srcPort)   // Local (VM) port
			{
				if(STATE_FIN_SENT==conn.state) // Closing from the remote host
				{
					conn.tcpHdr.StripOptions(); // Just in case.
					conn.tcpHdr.flags=TCP_FLAG_ACK;
					conn.tcpHdr.ackNum=tcp.sequenceNum+1;
					conn.ipHdr.len=conn.ipHdr.GetHeaderLength()+conn.tcpHdr.GetTotalLength();

					std::vector <uint8_t> data;
					AddEthernetHeader(data,conn.ethernetHdr);

					size_t IPHeaderPos=data.size();
					AddIPHeader(data,conn.ipHdr);
					RecalculateIPHeaderCheckSum(20,data.data()+IPHeaderPos);

					size_t TCPHeaderPos=data.size();
					AddTCPHeader(data,conn.tcpHdr);
					RecalculateTCPHeaderCheckSum(data.size()-TCPHeaderPos,data.data()+TCPHeaderPos,ip.srcIP,ip.dstIP);

					recv->ReceivePacket(data.size(),data.data());

					iter=TCPConn.erase(iter); // conn.state=STATE_CLOSED;
					deleted=true;
				}
				else // Closing from the VM.
				{
					//Acknowledge FIN
					++conn.tcpHdr.ackNum;

					auto &tcp=conn.tcpHdr; // At this point forget incoming tcp- and ip-headers.
					auto &ip=conn.ipHdr;

					tcp.flags=TCP_FLAG_ACK;
					tcp.StripOptions();

					ip.len=ip.GetHeaderLength()+tcp.GetTotalLength();

					std::vector <uint8_t> data;
					AddEthernetHeader(data,conn.ethernetHdr);

					size_t IPHeaderPos=data.size();
					AddIPHeader(data,ip);
					RecalculateIPHeaderCheckSum(20,data.data()+IPHeaderPos);

					size_t TCPHeaderPos=data.size();
					AddTCPHeader(data,tcp);
					// No payload.
					RecalculateTCPHeaderCheckSum(data.size()-TCPHeaderPos,data.data()+TCPHeaderPos,ip.srcIP,ip.dstIP);

					recv->ReceivePacket(data.size(),data.data());

					conn.state=STATE_FIN_RECEIVED;
					if(true==monitorTCP)
					{
						std::cout << "FIN initiated from the VM.\n";
					}
				}
			}
			if(true!=deleted)
			{
				++iter;
			}
		}
	}
	if(TCP_FLAG_RST&tcp.flags)
	{
		std::cout << "TCP_FLAG_RST not handling now.\n";
	}
	if(TCP_FLAG_URG&tcp.flags)
	{
		std::cout << "TCP_FLAG_URG not handling now.\n";
	}
	if(TCP_FLAG_ECE&tcp.flags)
	{
		std::cout << "TCP_FLAG_ECE not handling now.\n";
	}
	if(TCP_FLAG_CWR&tcp.flags)
	{
		std::cout << "TCP_FLAG_CWR not handling now.\n";
	}


	size_t optPtr=0;
	bool optErr=false,optEnd=false;
	while(optPtr<tcp.GetOptionLength() && true!=optErr && true!=optEnd)
	{
		switch(tcp.options[optPtr])
		{
		case TCP_OPTION_END: //0,
			optEnd=true;
			++optPtr;
			break;
		case TCP_OPTION_NOP: //1,
			++optPtr;
			break;
		case TCP_OPTION_MSS: //2,
			if(optPtr+1<tcp.GetOptionLength())
			{
				if(4==tcp.options[optPtr+1] && nullptr!=thisConn)
				{
					thisConn->MSS=GetWordBE(tcp.options+optPtr+2);
					std::cout << "TCP Option MSS=" << thisConn->MSS << "\n";
				}
				else
				{
					optErr=true; // Something went wrong.
				}
				optPtr+=tcp.options[optPtr+1];
			}
			else
			{
				optErr=true;
			}
			break;
		case TCP_OPTION_WSCALE: //3,
			std::cout << "WSCALE option is present.  Should omit in the ACK|SYN packet.\n";
			if(optPtr+1<tcp.GetOptionLength())
			{
				optPtr+=tcp.options[optPtr+1];
			}
			else
			{
				optErr=true;
			}
			break;
		case TCP_OPTION_SACK_PERM: //4,
			if(optPtr+1<tcp.GetOptionLength())
			{
				optPtr+=tcp.options[optPtr+1];
			}
			else
			{
				optErr=true;
			}
			break;
		case TCP_OPTION_SACK: //5,
			if(optPtr+1<tcp.GetOptionLength())
			{
				optPtr+=tcp.options[optPtr+1];
			}
			else
			{
				optErr=true;
			}
			break;
		case TCP_OPTION_TIMESTAMP: //8,
			if(optPtr+1<tcp.GetOptionLength())
			{
				optPtr+=tcp.options[optPtr+1];
			}
			else
			{
				optErr=true;
			}
			break;
		case TCP_OPTION_TIMEOUT: //28,
			if(optPtr+1<tcp.GetOptionLength())
			{
				optPtr+=tcp.options[optPtr+1];
			}
			else
			{
				optErr=true;
			}
			break;
		case TCP_OPTION_AUTH_OPT: //29,
			if(optPtr+1<tcp.GetOptionLength())
			{
				optPtr+=tcp.options[optPtr+1];
			}
			else
			{
				optErr=true;
			}
			break;
		case TCP_OPTION_MPTCP: //30,
			if(optPtr+1<tcp.GetOptionLength())
			{
				optPtr+=tcp.options[optPtr+1];
			}
			else
			{
				optErr=true;
			}
			break;
		}
	}
}

void  VirtualNetwork::TCPConnectionEstablished(TCPConnection &conn,PacketReceiver *recv)
{
	conn.state=STATE_ESTABLISHED;

	conn.tcpHdr.flags|=TCP_FLAG_ACK;
	conn.tcpHdr.ackNum=conn.tcpHdr.sequenceNum+1;
	conn.tcpHdr.sequenceNum=sequenceNumSource++;

	auto ether=conn.ethernetHdr;
	auto ip=conn.ipHdr;
	auto tcp=conn.tcpHdr;

	tcp.NOPWindowScale();
	std::vector <uint8_t> data;
	AddEthernetHeader(data,ether);

	size_t IPHeaderPos=data.size();
	AddIPHeader(data,ip);

	RecalculateIPHeaderCheckSum(20,data.data()+IPHeaderPos);

	size_t TCPHeaderPos=data.size();
	AddTCPHeader(data,tcp);

	RecalculateTCPHeaderCheckSum(data.size()-TCPHeaderPos,data.data()+TCPHeaderPos,ip.srcIP,ip.dstIP);

	recv->ReceivePacket(data.size(),data.data());

	conn.tcpHdr.StripOptions();
	++conn.ipHdr.fragID; // Is it necessary?
	++conn.tcpHdr.sequenceNum;

	if(true==monitorTCP)
	{
		std::cout << "After Sending SYN|ACK to VM\n";
		std::cout << "  Seq#=" << cpputil::Uitox(conn.tcpHdr.sequenceNum) << "\n";
		std::cout << "  Ack#=" << cpputil::Uitox(conn.tcpHdr.ackNum) << "\n";
	}

	conn.waitingAck=true;
	conn.unacknowledgedSeq=conn.tcpHdr.sequenceNum;
}

void VirtualNetwork::ReceivedTCPData(TCPConnection &conn,size_t len,const uint8_t recvdata[],PacketReceiver *recv)
{
	conn.tcpHdr.dataOffset_reservedBits=0x50; // No options.
	conn.tcpHdr.flags=TCP_FLAG_PSH|TCP_FLAG_ACK;
	conn.tcpHdr.windowSize=TCP_WINDOW_SIZE;

	conn.ipHdr.len=20+20+len; // 20 bytes for IP header, 20 bytes TCP Header without Option, plus data length.

	std::vector <uint8_t> data;
	AddEthernetHeader(data,conn.ethernetHdr);

	size_t IPHeaderPos=data.size();
	AddIPHeader(data,conn.ipHdr);
	RecalculateIPHeaderCheckSum(20,data.data()+IPHeaderPos);

	size_t TCPHeaderPos=data.size();
	AddTCPHeader(data,conn.tcpHdr);

	data.insert(data.end(),recvdata,recvdata+len);

	RecalculateTCPHeaderCheckSum(data.size()-TCPHeaderPos,data.data()+TCPHeaderPos,conn.ipHdr.srcIP,conn.ipHdr.dstIP);

	if(true==monitorTCP)
	{
		std::cout << "TCP Packet Remote->VM  Port:" << uint16_t(conn.tcpHdr.srcPort) << "->" << uint16_t(conn.tcpHdr.dstPort);
		std::cout << " IP:" << FormatIPAddress(conn.ipHdr.srcIP) << "->" << FormatIPAddress(conn.ipHdr.dstIP) << "\n";
	}

	recv->ReceivePacket(data.size(),data.data());

	++conn.ipHdr.fragID; // Is it necessary?
	conn.tcpHdr.sequenceNum+=len;

	if(true==monitorTCP)
	{
		std::cout << "After Sending " << len << " bytes of data\n";
		std::cout << "  Seq#=" << cpputil::Uitox(conn.tcpHdr.sequenceNum) << "\n";
		std::cout << "  Ack#=" << cpputil::Uitox(conn.tcpHdr.ackNum) << "\n";
	}

	conn.waitingAck=true;
	conn.unacknowledgedSeq=conn.tcpHdr.sequenceNum;
}

void VirtualNetwork::TCPInitiateFIN(TCPConnection &conn,PacketReceiver *recv)
{
	conn.tcpHdr.flags=TCP_FLAG_ACK|TCP_FLAG_FIN;
	conn.tcpHdr.StripOptions(); // Just in case
	conn.ipHdr.len=conn.ipHdr.GetHeaderLength()+conn.tcpHdr.GetTotalLength();

	std::vector <uint8_t> data;

	AddEthernetHeader(data,conn.ethernetHdr);

	size_t IPHeaderPos=data.size();
	AddIPHeader(data,conn.ipHdr);
	RecalculateIPHeaderCheckSum(20,data.data()+IPHeaderPos);

	size_t TCPHeaderPos=data.size();
	AddTCPHeader(data,conn.tcpHdr);
	RecalculateTCPHeaderCheckSum(data.size()-TCPHeaderPos,data.data()+TCPHeaderPos,conn.ipHdr.srcIP,conn.ipHdr.dstIP);

	recv->ReceivePacket(data.size(),data.data());

	++conn.ipHdr.fragID; // Is it necessary?

	++conn.tcpHdr.sequenceNum;

	conn.state=STATE_FIN_SENT;
}

void VirtualNetwork::Polling(PacketReceiver *recv,class RealNetwork *realNet)
{
	{
		std::lock_guard <std::mutex> lock(realNet->clientsLock);
		// for(auto &cli : realNet->clients)
		// {
		// 	if(0<cli.recvBuf.size())
		// 	{
		// 		std::cout << "Recv Data Present for VMPort:" << cli.conn.VMPort << " dstPort:" << cli.conn.dstPort << "\n";
		// 		std::cout << "(" << recv->RxReady() << ")\n";
		// 	}
		// }

		for(auto &cli : realNet->clients)
		{
			if(RealNetwork::STATE_JUST_CONNECTED==cli.state)
			{
				cli.state=RealNetwork::STATE_CONNECTED;

				if(true==monitorTCP)
				{
					std::cout << "Connection Established\n";
				}

				for(auto &virConn : TCPConn)
				{
					if(virConn.ipHdr.srcIP==cli.conn.GetIPUint32() &&
					   virConn.tcpHdr.dstPort==cli.conn.VMPort &&
					   virConn.tcpHdr.srcPort==cli.conn.dstPort)
					{
						TCPConnectionEstablished(virConn,recv);
						break;
					}
				}
			}
			else if(RealNetwork::STATE_CONNECTED==cli.state || RealNetwork::STATE_DISCONNECTED_BUT_DATA_LEFTOVER==cli.state)
			{
				for(auto &virConn : TCPConn)
				{
					// src and dst swapped in TCPConnectionEstablished.
					if(true!=virConn.waitingAck &&
					   virConn.ipHdr.srcIP==cli.conn.GetIPUint32() &&
					   virConn.tcpHdr.dstPort==cli.conn.VMPort &&
					   virConn.tcpHdr.srcPort==cli.conn.dstPort)
					{
						if(0<cli.recvBuf.size() && true==recv->RxReady())
						{
							size_t send_bytes=std::min<size_t>(cli.recvBuf.size(),TCP_MAX_LENGTH);
							send_bytes=std::min<size_t>(send_bytes,virConn.MSS);
							ReceivedTCPData(virConn,send_bytes,cli.recvBuf.data(),recv);
							cli.recvBuf.erase(cli.recvBuf.begin(),cli.recvBuf.begin()+send_bytes);

							if(RealNetwork::STATE_DISCONNECTED_BUT_DATA_LEFTOVER==cli.state && 0==cli.recvBuf.size())
							{
								if(true==monitorTCP)
								{
									std::cout << "V1 Disconnected VMPort:" << uint16_t(cli.conn.VMPort) << " RemotePort:" << uint16_t(cli.conn.dstPort);
									std::cout << "IP:" << FormatIPAddress(cli.conn.GetIPUint32()) << "\n";
								}
								cli.state=RealNetwork::STATE_DISCONNECTED;
								virConn.state=STATE_CLOSING_FROM_ROUTER; // Need to send FIN.
							}
							break;
						}
						else if(0<virConn.TxData.size())
						{
							cli.sendBuf.insert(cli.sendBuf.end(),virConn.TxData.begin(),virConn.TxData.end());
							virConn.TxData.clear();
						}
						if(STATE_FIN_RECEIVED==virConn.state)
						{
							if(true==monitorTCP && true!=cli.FIN_initiated_from_VM)
							{
								std::cout << "VM initiated FIN and Real Network notified.\n";
							}
							cli.FIN_initiated_from_VM=true;
						}
					}
				}
			}
			else if(RealNetwork::STATE_DISCONNECTED_NEED_TO_SEND_FIN==cli.state)
			{
				for(auto &virConn : TCPConn)
				{
					// src and dst swapped in TCPConnectionEstablished.
					if(virConn.ipHdr.srcIP==cli.conn.GetIPUint32() &&
					   virConn.tcpHdr.dstPort==cli.conn.VMPort &&
					   virConn.tcpHdr.srcPort==cli.conn.dstPort)
					{
						if(STATE_FIN_SENT!=virConn.state)
						{
							virConn.state=STATE_CLOSING_FROM_ROUTER;
							cli.state=RealNetwork::STATE_DISCONNECTED; // Once notify, real-network side can be gone.
						}
					}
				}
			}
		}

		for(auto iter=realNet->clients.begin(); realNet->clients.end()!=iter; )
		{
			if(RealNetwork::STATE_DISCONNECTED==iter->state)
			{
				iter=realNet->clients.erase(iter);
			}
			else
			{
				++iter;	
			}
		}
	}
	{
		std::lock_guard<std::mutex> lock(realNet->DNSRequestLock);
		for(auto &vReq : DNSReq)
		{
			for(auto iter=realNet->DNSReq.begin(); realNet->DNSReq.end()!=iter; )
			{
				auto &rReq=*iter;
				if(RealNetwork::DNS_REQUESTED!=rReq.state && vReq.hostname==rReq.hostname)
				{
					if(RealNetwork::DNS_FOUND==rReq.state)
					{
						vReq.state=DNS_FOUND;
						memcpy(vReq.ipAddr,rReq.ipAddr,sizeof(rReq.ipAddr));
					}
					else
					{
						vReq.state=DNS_NOT_FOUND;
					}
					iter=realNet->DNSReq.erase(iter);
				}
				else
				{
					++iter;
				}
			}
		}
	}
	
	if(true==recv->RxReady())
	{
		for(auto &virConn : TCPConn)
		{
			if(STATE_CLOSING_FROM_ROUTER==virConn.state)
			{
				TCPInitiateFIN(virConn,recv);
			}
		}
		for(auto iter=DNSReq.begin(); iter!=DNSReq.end();)
		{
			auto &req=*iter;
			if(DNS_REQUESTED==iter->state)
			{
				++iter;
			}
			else if(DNS_FOUND==req.state)
			{
				SendDNSReplyFoundToVM(req,recv);
				iter=DNSReq.erase(iter);
			}
			else if(DNS_NOT_FOUND==req.state)
			{
				SendDNSReplyNotFoundToVM(req,recv);
				iter=DNSReq.erase(iter);
			}
			else
			{
				// Unknown state
				std::cout << "DNS Request fell to an unknown state.\n";
			}
		}
	}
}

