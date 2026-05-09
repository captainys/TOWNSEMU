#ifndef VIRTUAL_NETWORK_H_IS_INCLUDED
#define VIRTUAL_NETWORK_H_IS_INCLUDED

#include "cpputil.h"

class VirtualNetwork
{
public:
	enum
	{
		MAC_DHCP_SERVER=0x00000001,
		VM_DHCP_IP=0xC0A80164, // 192.168.1.100
	};

	enum
	{
		DHCP_SERVER_IP=0xC0A80101, // 192.168.1.1
		ROUTER_IP=0xC0A80101,
		DNS_IP=0xC0A80101,

		DHCP_CLIENT_PORT=0x44,
		DHCP_SERVER_PORT=0x43,

		DHCP_MSG_DISCOVER=1,
		DHCP_MSG_REPLY=2,
		DHCP_MSG_REQUEST=3,

		DHCP_OPTION_SUBNET_MASK=0x01,
		DHCP_OPTION_DEF_ROUTER=0x03,
		DHCP_OPTION_DNS_SERVER=0x06,
		DHCP_OPTION_HOSTNAME=0x0C,
		DHCP_OPTION_LEASE_TIME_REQ=0x33,
		DHCP_OPTION_MESSAGE_TYPE=0x35,
		DHCP_OPTION_SERVER_IP=0x36,
		DHCP_OPTION_PARAM_REQ_LIST=0x37,
		DHCP_OPTION_END=0xFF
	};

	class EthernetHeader
	{
	public:
		uint64_t dstMAC,srcMAC;
		uint8_t type;
	};
	class IPHeader
	{
	public:
		uint8_t version_headerLen;
		uint8_t QoS;
		uint16_t len;
		uint16_t fragID;
		uint16_t flagOrFragOffset;
		uint8_t TTL;
		uint8_t protocol;  // 06H for TCP  11H for UDP
		uint16_t checkSum;
		uint32_t srcIP;
		uint32_t dstIP;
	};
	class TCPHeader
	{
	public:
	};
	class UDPHeader
	{
	public:
		uint16_t srcPort;
		uint16_t dstPort;
		uint16_t len;
		uint16_t checkSum;
		uint8_t messageType;
		uint8_t netType;
		uint8_t MAClen;
		uint8_t hop;
		uint32_t transactionID;
		// From +16 to +36 are vague.  Prob Seconds Elapsed(word), BOOTP flags(word), ClientIP(DW), MyIP(DW), ServerIP(DW), RelayIP(DW).
		uint64_t MAC;
	};


	class DHCPOption
	{
	public:
		uint8_t msgType;
		std::string hostName;
		std::vector <uint8_t> paramReqList;
		uint32_t leaseTimeReq;

		void CleanUp(void);
		bool Decode(size_t len,const uint8_t data[]);
	};


	class PacketReceiver
	{
	public:
		// Virtual Network -> Adapter
		virtual void ReceivePacket(size_t len,const uint8_t data[])=0;
	};

	bool monitorTX=true,monitorRX=true;

	static uint64_t GetMAC(const uint8_t data[]);
	static uint16_t GetWordBE(const uint8_t data[]);
	static uint32_t GetDwordBE(const uint8_t data[]);

	static void PutMAC(uint8_t data[],uint64_t MAC);
	static void PutWordBE(uint8_t data[],uint16_t i);
	static void PutDwordBE(uint8_t data[],uint32_t i);

	static uint16_t CalcIPCheckSum(size_t len,const uint8_t data[]);

	// Adapter -> Virtual Network
	void TransmitPacket(size_t len,const uint8_t data[],PacketReceiver *recv);
protected:
	void ProcessUDP_DHCP_Packet(EthernetHeader ether,IPHeader ip,UDPHeader udp,size_t len,const uint8_t data[],PacketReceiver *recv);
	std::vector <uint8_t> MakeDHCPReturnPacket(EthernetHeader ether,IPHeader ip,UDPHeader udp,DHCPOption opt);

public:
	// Polling.
	void Poll(PacketReceiver *recv);
};


#endif
