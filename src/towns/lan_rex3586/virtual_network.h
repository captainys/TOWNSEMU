#ifndef VIRTUAL_NETWORK_H_IS_INCLUDED
#define VIRTUAL_NETWORK_H_IS_INCLUDED

#include <string.h>
#include <string>
#include <vector>
#include "cpputil.h"

class RealNetwork;

class VirtualNetwork
{
public:
	const uint64_t MAC_DHCP_SERVER=0x434553534E41LL; // "CESSNA"
	const uint64_t MAC_ROUTER=0x434553534E41LL;      // "CESSNA"
	enum
	{
		ETHER_HEADER_SIZE=14,
		ARP_HEADER_SIZE=28,

		TYPE_IPV4=0x0800,
		TYPE_ARP=0x0806,
	};

	enum
	{
		DHCP_SERVER_IP=0x0A000202, // 10.0.2.2
		ROUTER_IP=0x0A000202,
		DNS_IP=0x0A000202,
		HOST_IP=0x0A0002AC,        // 10.0.2.172 to connect to the host.

		VM_DHCP_IP=0x0A00020F,     // 10.0.2.15
	};

	enum
	{
		PROTOCOL_TCP=6,
		PROTOCOL_UDP=17,
	};

	enum
	{
		DHCP_CLIENT_PORT=0x44,
		DHCP_SERVER_PORT=0x43,

		DHCP_MSG_DISCOVER=1,
		DHCP_MSG_REPLY=2,
		DHCP_MSG_REQUEST=3,
		DHCP_MSG_ACK=5,

		DHCP_OPTION_SUBNET_MASK=0x01,
		DHCP_OPTION_DEF_ROUTER=0x03,
		DHCP_OPTION_DNS_SERVER=0x06,
		DHCP_OPTION_HOSTNAME=0x0C,
		DHCP_OPTION_REQUESTED_IP_ADDR=0x32,
		DHCP_OPTION_LEASE_TIME_REQ=0x33,
		DHCP_OPTION_MESSAGE_TYPE=0x35,
		DHCP_OPTION_SERVER_IP=0x36,
		DHCP_OPTION_PARAM_REQ_LIST=0x37,
		DHCP_OPTION_MAC_ADDRESS=0x3D,
		DHCP_OPTION_END=0xFF
	};

	enum
	{
		DNS_SERVER_PORT=0x35,
	};

	enum
	{
		TCP_FLAG_FIN=1,
		TCP_FLAG_SYN=2,
		TCP_FLAG_RST=4,
		TCP_FLAG_PSH=8,
		TCP_FLAG_ACK=16,
		TCP_FLAG_URG=32,
		TCP_FLAG_ECE=64,
		TCP_FLAG_CWR=128,

		TCP_MAX_LENGTH=1500,
		TCP_WINDOW_SIZE=0x2000,

		TCP_OPTION_END=0,
		TCP_OPTION_NOP=1,
		TCP_OPTION_MSS=2,
		TCP_OPTION_WSCALE=3,
		TCP_OPTION_SACK_PERM=4,
		TCP_OPTION_SACK=5,
		TCP_OPTION_TIMESTAMP=8,
		TCP_OPTION_TIMEOUT=28,
		TCP_OPTION_AUTH_OPT=29,
		TCP_OPTION_MPTCP=30,
	};

	class EthernetHeader
	{
	public:
		uint64_t dstMAC,srcMAC;
		uint16_t type;
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

		const size_t GetHeaderLength(void) const
		{
			return 20;
		}
	};
	class TCPHeader
	{
	public:
		uint16_t srcPort;
		uint16_t dstPort;
		uint32_t sequenceNum;
		uint32_t ackNum;
		uint8_t dataOffset_reservedBits;
		uint8_t flags;
		uint16_t windowSize;
		uint16_t checkSum;
		uint16_t urgentPointer;
		uint8_t options[40];

		size_t GetOptionLength(void) const
		{
			size_t total=GetTotalLength();
			if(20<total)
			{
				return total-20;
			}
			return 0;
		}
		size_t GetTotalLength(void) const
		{
			size_t L=(dataOffset_reservedBits>>4)&0x0F; // 0x0F shouldn't be necessary, but just in case.
			L=std::max<size_t>(5,L);
			return L*4;
		}
		void StripOptions(void)
		{
			dataOffset_reservedBits=0x50;
		}
		void NOPWindowScale(void)
		{
			for(size_t i=0; i<GetOptionLength(); )
			{
				if(TCP_OPTION_END==options[i])
				{
					break;
				}
				else if(TCP_OPTION_NOP==options[i])
				{
					++i;
				}
				else if(TCP_OPTION_WSCALE==options[i] && i+1<GetOptionLength())
				{
					size_t optionSize=options[i+1];
					// i+optionSize<=GetOptionLength()
					optionSize=std::min(optionSize,GetOptionLength()-i);
					memset(options+i,TCP_OPTION_NOP,optionSize);
				}
				else if(i+1<GetOptionLength())
				{
					i+=options[i+1];
				}
			}
		}
	};
	class UDPHeader
	{
	public:
		uint16_t srcPort;
		uint16_t dstPort;
		uint16_t len;
		uint16_t checkSum;
	};

	class DHCPInfo
	{
	public:
		// Apparently from this byte is already DHCP data.
		uint8_t messageType;
		uint8_t netType;
		uint8_t MAClen;
		uint8_t hop;
		uint32_t transactionID;
		// From +16 to +36 are vague.  Prob Seconds Elapsed(word), BOOTP flags(word), ClientIP(DW), MyIP(DW), ServerIP(DW), RelayIP(DW).
		uint64_t MAC;
	};

	class ARPHeader
	{
	public:
		uint16_t hardware;
		uint16_t protocol;
		uint8_t hardware_size; // MAC size?
		uint8_t protocol_size;
		uint16_t messageType;  // 02 for reply.
		uint64_t srcMAC;
		uint32_t srcIP;
		uint64_t dstMAC;
		uint32_t dstIP;
	};

	class DHCPOption
	{
	public:
		uint8_t msgType;
		std::string hostName;
		std::vector <uint8_t> paramReqList;
		uint32_t leaseTimeReq;
		uint32_t requestedIP;
		uint32_t hostIP;

		void CleanUp(void);
		bool Decode(size_t len,const uint8_t data[]);
	};

	enum
	{
		DNS_REQUESTED,
		DNS_FOUND,
		DNS_NOT_FOUND,
	};
	class DNSRequest
	{
	public:
		EthernetHeader ethernetHdr;
		IPHeader ipHdr;
		UDPHeader udpHdr;
		std::vector <uint8_t> udpPayload;
		std::string hostname;
		uint8_t ipAddr[4];
		int state=DNS_REQUESTED;
	};

	enum
	{
		STATE_PENDING,
		STATE_ESTABLISHED,
		STATE_CLOSED,

		// Disconnect from Real-Network
		// (1) Real network disconnected (recv() returns 0 or -1)
		// (2) Router sends VM  FIN|ACK   TCPInitiateFIN (Real:STATE_CLOSING_FROM_ROUTER,  Virtual:ESTABLISHED->FIN_SENT)
		// (3) VM replies with ACK for (2)
		// (4) VM sends Router  FIN|ACK
		// (5) Router replies with ACK for (4)
		STATE_CLOSING_FROM_REMOTE,             // Used to be STATE_CLOSING_FROM_ROUTER,
		STATE_CLOSING_FROM_REMOTE_SENT_FINACK, // Used to be STATE_SENT_FIN,  Closing from the remote, and FIN|ACK has been sent to the VM.

		// Disconnect from VM
		// (1) VM sends Router  FIN|ACK
		// (2) Router notifies real-network about the disconnection.  Real network initiates Shutdown
		// (3) Router replies VM with ACK for (1)
		// (4) Wait until real-network disconnects (recv() returns 0 or -1)
		// (5) Router sends VM  FIN|ACK   TCPInitiateFIN (Real:STATE_CLOSING_FROM_ROUTER,  Virtual:ESTABLISHED->FIN_SENT)
		// (6) VM replies Router with  ACK for (5)
		STATE_VM_INITIATED_FIN,               // VM initiated FIN.  Router received FIN|ACK from the VM.
		STATE_VM_INITIATED_FIN_WAIT_SHUTDOWN, // Real-Network layer notified.  Waiting for shutdown to complete.
		STATE_VM_INITIATED_FIN_SHUTDOWN_DONE, // Real-Network layer disconnected.
		STATE_VM_INITIATED_FIN_SENT_FINACK,   // Real-Network layer shutdown completed.  Sent FIN|ACK to VM.

		STATE_ACCEPTED, // Incoming connection from outside.
		STATE_ACCEPTED_SYN_SENT,
	};
	class TCPConnection
	{
	public:
		int state=STATE_PENDING;
		uint16_t MSS=1024; // Tentatively make it 1K.

		bool waitingAck=false;
		uint32_t unacknowledgedSeq=0;

		std::vector <uint8_t> TxData;
	
		EthernetHeader ethernetHdr;
		IPHeader ipHdr;
		TCPHeader tcpHdr;
	};

	class PacketReceiver
	{
	public:
		// Virtual Network -> Adapter
		virtual void ReceivePacket(size_t len,const uint8_t data[])=0;
		virtual bool RxReady(void) const=0;
		virtual uint64_t GetMACAddress(void) const=0;
	};

	bool TxReady=true;
	std::vector <TCPConnection> TCPConn;
	std::vector <DNSRequest> DNSReq;

	bool monitorTX=true,monitorRX=true;
	bool monitorTCP=true;
	bool monitorDNS=true;
	uint32_t sequenceNumSource=59673459;

	static uint64_t GetMAC(const uint8_t data[]);
	static uint16_t GetWordBE(const uint8_t data[]);
	static uint32_t GetDwordBE(const uint8_t data[]);

	static void PutMAC(uint8_t data[],uint64_t MAC);
	static void PutWordBE(uint8_t data[],uint16_t i);
	static void PutDwordBE(uint8_t data[],uint32_t i);

	static uint16_t CalcIPCheckSum(size_t len,const uint8_t data[]);

	static uint16_t TestTCPCheckSum(size_t len,const uint8_t data[],uint32_t srcIP,uint32_t dstIP);

	static std::string FormatIPAddress(uint32_t IP);

	static EthernetHeader DecodeEthernetHeader(size_t len,const uint8_t data[]);
	static void AddEthernetHeader(std::vector <uint8_t> &data,const EthernetHeader &hdr);

	static void AddIPHeader(std::vector <uint8_t> &data,const IPHeader &hdr);
	static void RecalculateIPHeaderCheckSum(size_t len,uint8_t data[]);

	static ARPHeader DecodeARPHeader(size_t len,const uint8_t data[]);
	static void AddARPHeader(std::vector <uint8_t> &data,const ARPHeader &hdr);

	static void AddUDPHeader(std::vector <uint8_t> &data,const UDPHeader &hdr);

	static TCPHeader DecodeTCPHeader(size_t len,const uint8_t data[]);
	static void AddTCPHeader(std::vector <uint8_t> &data,TCPHeader &hdr);
	static void RecalculateTCPHeaderCheckSum(size_t len,uint8_t data[],uint32_t srcIP,uint32_t dstIP);


	static void RequestPortForwarding(RealNetwork *realNet,uint16_t VMPort,uint16_t HostPort);


	void AddStatusText(std::vector <std::string> &text) const;


	// Prepare for reset/state-load
	void DisconnectAll(void);


	// Adapter -> Virtual Network
	void TransmitPacket(size_t len,const uint8_t data[],PacketReceiver *recv,RealNetwork *realNet);
protected:
	void ProcessUDP_DHCP_Packet(EthernetHeader ether,IPHeader ip,UDPHeader udp,DHCPInfo dhcp,size_t len,const uint8_t data[],PacketReceiver *recv);
	std::vector <uint8_t> MakeDHCPReturnPacket(EthernetHeader ether,IPHeader ip,UDPHeader udp,DHCPInfo dhcp,DHCPOption opt);

	void ProcessARP_Packet(EthernetHeader ether,size_t len,const uint8_t data[],PacketReceiver *recv);

	void ProcessUDP_DNS_Packet(EthernetHeader ether,IPHeader ip,UDPHeader udp,size_t len,const uint8_t data[],RealNetwork *realNet);
	void SendDNSReplyFoundToVM(DNSRequest &req,PacketReceiver *recv);
	void SendDNSReplyNotFoundToVM(DNSRequest &req,PacketReceiver *recv);

	void ProcessTCP_Packet(EthernetHeader ether,IPHeader ip,TCPHeader tcp,size_t len,const uint8_t data[],PacketReceiver *recv,RealNetwork *realNet);

	static void TCPSendPureAckToVM(EthernetHeader ether,IPHeader &ip,TCPHeader &tcp,uint32_t incoming_seq_num,uint32_t ackConsumption,uint32_t payloadLength,PacketReceiver *recv);

	void TCPConnectionEstablished(TCPConnection &conn,PacketReceiver *recv);
	void ReceivedTCPData(TCPConnection &conn,size_t len,const uint8_t data[],PacketReceiver *recv);

	void TCPSendFINACK(TCPConnection &conn,PacketReceiver *recv,uint32_t nextState);

public:
	void Polling(PacketReceiver *recv,class RealNetwork *realNet);
};


#endif
