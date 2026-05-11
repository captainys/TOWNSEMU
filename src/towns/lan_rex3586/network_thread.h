#ifndef NETWORK_THREAD_IS_INCLUDED
#define NETWORK_THREAD_IS_INCLUDED
/* { */

#include <vector>
#include <thread>
#include <atomic>
#include <list>
#include <list>
#include <mutex>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef _WIN32
#define _WINSOCKAPI_
  // According to the Win32API documentation,
  // I must define _WINSOCKAPI_ to prevent inclusion of
  // winsock.h from windows.h
	#include <winsock2.h>
	#include <windows.h>
	#pragma comment(lib,"wsock32.lib")
typedef int socklen_t;

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
#endif


class NetworkThread
{
public:
	enum
	{
		STATE_DISCONNECTED,
		STATE_JUST_CONNECTED,  // Connected, but the VM thread does not know yet.
		STATE_CONNECTED,       // Connection established, VM thread knows about it.
	};

	class Connection
	{
	public:
		uint16_t VMPort;
		uint8_t IPv4Addr[4];
		uint16_t dstPort;
	};

	class Client
	{
	friend class NetworkThread;
	public:
		int state;
		std::vector <uint8_t> recvBuf;
		SOCKET sock;
		Connection conn;
	};

	class TCPConnectionRequest
	{
	public:
		bool connected;
		SOCKET sock;
		Connection conn;
		bool DoConnect(void);
	};

	std::atomic_bool stopThread=false;
	std::thread workerThread;

	std::list <Client> clients;
	std::mutex clientsLock;

	std::vector <TCPConnectionRequest> TCPConnReq;
	std::mutex TCPConnReqLock;

	std::atomic_int state;


	// Starts the thread.
	NetworkThread();

	// Closes the thread.
	~NetworkThread();

	// Called from the VM thread.



	// In the network thread.
	void ThreadFunc(void);
private:
	void StartUp(void);
	void CleanUp(void);

public:
	// Called from the VM thread.
	void RequestTCPConnection(uint16_t VMPort,const uint8_t IPv4Addr[4],uint16_t port);
};

/* } */
#endif
