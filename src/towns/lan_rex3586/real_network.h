#ifndef NETWORK_THREAD_IS_INCLUDED
#define NETWORK_THREAD_IS_INCLUDED
/* { */

#include <vector>
#include <thread>
#include <atomic>
#include <list>
#include <mutex>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef _WIN32
#define _WINSOCKAPI_
#define WIN32_LEAN_AND_MEAN
  // According to the Win32API documentation,
  // I must define _WINSOCKAPI_ to prevent inclusion of
  // winsock.h from windows.h
	#include <winsock2.h>

	#ifdef min // **ck windows.h
		#undef min
	#endif
	#ifdef max
		#undef max
	#endif
	#ifdef OUT // Really fu** windows.h
		#undef OUT
	#endif
#else
	#include <unistd.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <strings.h>
	#include <arpa/inet.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/poll.h>
	typedef int SOCKET;
	typedef struct sockaddr SOCKADDR;
	typedef struct sockaddr_in SOCKADDR_IN;
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define closesocket close
#endif


class RealNetwork
{
public:
	enum
	{
		STATE_DISCONNECTED,
		STATE_JUST_CONNECTED,  // Connected, but the VM thread does not know yet.
		STATE_CONNECTED,       // Connection established, VM thread knows about it.
		STATE_DISCONNECTED_BUT_DATA_LEFTOVER,
		STATE_DISCONNECTED_NEED_TO_SEND_FIN,

		STATE_JUST_ACCEPTED,
		STATE_ACCEPTED_NOTIFIED,

		STATE_NOT_LISTENING,
		STATE_LISTENING,
	};

	const uint32_t listen_max=8;

	class Connection
	{
	public:
		uint16_t VMPort;
		uint8_t IPv4Addr[4];
		uint16_t dstPort;
		uint8_t realIPv4Addr[4]={0,0,0,0};  // For forwarding 10.0.2.101 to the host OS.

		uint32_t GetIPUint32(void) const
		{
			uint32_t ip;
			ip=IPv4Addr[0];
			ip<<=8;
			ip|=IPv4Addr[1];
			ip<<=8;
			ip|=IPv4Addr[2];
			ip<<=8;
			ip|=IPv4Addr[3];
			return ip;
		}
	};

	class Client // After naming it as Client, I realized it's more like a server.  Anyways...
	{
	friend class RealNetwork;
	public:
		int state;
		bool bytesIncoming=false;
		bool stateChanged=false;

		// Closing connection from VM >>
		bool FIN_initiated_from_VM=false;
		bool ShutdownInitiated=false;
		// Closing connection from VM <<

		std::vector <uint8_t> recvBuf; // VM <- Outside
		std::vector <uint8_t> sendBuf; // VM -> Outside
		SOCKET sock=INVALID_SOCKET;
		Connection conn;
	};

	class PortForwarding
	{
	friend class RealNetwork;
	public:
		uint32_t VMPort,HostPort;  // Forward incoming connection to Host's port to VM's VMPort.
		SOCKET sock=INVALID_SOCKET;
		uint32_t state=STATE_NOT_LISTENING;
		bool connectionIncoming=false;
	};

	class TCPConnectionRequest
	{
	public:
		bool connected=false;
		SOCKET sock;
		Connection conn;
		bool DoConnect(void);
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
		int state=DNS_REQUESTED;
		std::string hostname;
		uint8_t ipAddr[4]={0,0,0,0};
	};

	bool started=false;
	bool monitor=true;

	std::atomic_bool stopThread=false;
	std::thread workerThread;

	std::vector <Client> clients;
	mutable std::mutex clientsLock;

	std::vector <TCPConnectionRequest> TCPConnReq;
	mutable std::mutex TCPConnReqLock;

	std::vector <uint16_t> TCPDisconnectReq;
	mutable std::mutex TCPDisconnectReqLock;

	std::vector <DNSRequest> DNSReq;
	mutable std::mutex DNSRequestLock;

	std::vector <PortForwarding> portForwarding;
	mutable std::mutex portForwardingLock;

public:
	// In the VM thread.
	// Starts the thread.
	RealNetwork();

	// Closes the thread.
	~RealNetwork();

	void Start(void);
	void End(void);

	// Called from the VM thread.
	void RequestTCPConnection(uint16_t VMPort,const uint8_t IPv4Addr[4],uint16_t port);

	void RequestTCPConnection(uint16_t VMPort,const uint8_t IPv4Addr[4],uint16_t port,const uint8_t realIPV4Addr[4]);

	void RequestDNS(std::string hostname);

	void AddStatusText(std::vector <std::string> &text) const;

	void ResetReceived(uint16_t VMPort,const uint8_t IPv4Addr[4],uint16_t port);

	void RequestPortForwarding(uint16_t VMPort,uint16_t HostPort);

	// In the network thread.
	void ThreadFunc(void);
private:
	void CheckTCPConnectionIncomingData(void);
	static void CheckTCPConnectionIncomingData_Isolated(std::vector <Client> &clients,bool monitor);
	void CheckTCPConnectionRequest(void);
	void CheckTCPDisconnectionRequest(void);
	void CheckTCPConnectionOutboundData(void);
	void CheckDNSRequest(void);
	void CheckTCPPortForwarding(void);

	void StartUp(void);
	void CleanUp(void);
};

/* } */
#endif
