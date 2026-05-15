#include <iostream>
#include "real_network.h"

#ifdef _WIN32
	#include <windows.h>
	#pragma comment(lib,"wsock32.lib")
	typedef int socklen_t;
#endif

#define THREAD_PROGRESS(x) // {std::cout << "T" << (x) << "\n";}

RealNetwork::RealNetwork()
{
}

RealNetwork::~RealNetwork()
{
	End();
}

void RealNetwork::ThreadFunc(void)
{
	StartUp();

	while(true!=stopThread)
	{
		THREAD_PROGRESS("1");

		if(0==clients.size())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		else
		{
		#ifdef _WIN32
			// --- Windows Path: select ---
			fd_set readfds;
			FD_ZERO(&readfds);

			for(auto &cli : clients)
			{
				FD_SET(cli.sock, &readfds); // cli.sock is not accessed from outside of Network Thread
			}

			// 10ms timeout
			timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 10000; 
			// nfds is ignored on Windows, but usually set to 0 or max fd + 1
			if (0<select(0,&readfds,NULL,NULL,&tv))
			{
				std::lock_guard <std::mutex> lock(clientsLock);
				for(auto &cli : clients)
				{
					if(FD_ISSET(cli.sock,&readfds))
					{
						cli.bytesIncoming=true;
					}
				}
			}
		#else
			// --- POSIX Path: poll ---
			std::vector<struct pollfd> fds;
			for(auto &cli : clients)
			{
				struct pollfd pfd;
				pfd.fd=cli.sock;
				pfd.events=POLLIN;
				fds.push_back(pfd);
			}

			// poll uses milliseconds (10ms)
			if (0<poll(fds.data(),(nfds_t)fds.size(),10)) 
			{
				std::lock_guard <std::mutex> lock(clientsLock);
				for(auto &fd : fds)
				{
					if (fd.revents & POLLIN) 
					{
						for(auto &cli : clients)
						{
							if(cli.sock==fd.fd)
							{
								cli.bytesIncoming=true;
								break;
							}
						}
					}
				}
			}
		#endif
			auto next=clients.begin();
			for(auto iter=next; clients.end()!=iter; iter=next)
			{
				next=iter;
				++next;

				auto &cli=*iter;
				if(true==cli.bytesIncoming)
				{
					std::lock_guard <std::mutex> lock(clientsLock);
					cli.bytesIncoming=false;

					if(true==monitor)
					{
						std::cout << "Received from Dst Port:" << cli.conn.dstPort << " to VM Port:" << cli.conn.VMPort << "\n";
					}

					const int bufLen=4096;
					uint8_t buf[bufLen];
					int nBytesRecv=recv(cli.sock,(char *)buf,sizeof(buf),0);
					if(SOCKET_ERROR==nBytesRecv || 0==nBytesRecv)
					{
						closesocket(cli.sock);
						if(0==cli.recvBuf.size())
						{
							cli.state=STATE_DISCONNECTED;
						}
						else
						{
							cli.state=STATE_DISCONNECTED_BUT_DATA_LEFTOVER;
						}
					}
					else
					{
						cli.recvBuf.insert(cli.recvBuf.end(),buf,buf+nBytesRecv);
					}
				}
			}
		}
		THREAD_PROGRESS("2");
		{
			// Process connection requests.
			std::lock_guard <std::mutex> lock(TCPConnReqLock);
			unsigned int nConnected=0;
			for(auto &req : TCPConnReq)
			{
				if(true==monitor)
				{
					std::cout << "Handling Connection Request\n";
					std::cout << int(req.conn.IPv4Addr[0]) << "." << int(req.conn.IPv4Addr[1]) << "." << int(req.conn.IPv4Addr[2]) << "." << int(req.conn.IPv4Addr[3]) << ":";
					std::cout << int(req.conn.dstPort) << "\n";
				}
				req.connected=req.DoConnect();
				if(true==req.connected)
				{
					++nConnected;
				}
			}
			if(0<nConnected)
			{
				std::lock_guard <std::mutex> lock(clientsLock);
				for(auto &req : TCPConnReq)
				{
					if(true==req.connected)
					{
						Client cli;
						cli.conn=req.conn;
						cli.sock=req.sock;
						cli.state=STATE_JUST_CONNECTED;
						clients.push_back(cli);
					}
				}
			}
			TCPConnReq.clear();
		}
		THREAD_PROGRESS("3");
		{
			std::lock_guard <std::mutex> lock(TCPDisconnectReqLock);
			for(auto port : TCPDisconnectReq)
			{
				std::lock_guard <std::mutex> lock2(clientsLock);
				for(auto iter=clients.begin(); clients.end()!=iter; )
				{
					if(iter->conn.VMPort==port)
					{
						closesocket(iter->sock);
						iter = clients.erase(iter);
					}
					else
					{
						// Only advance iterator if no erase happened
						++iter;
					}
				}
			}
			TCPDisconnectReq.clear();
		}
		THREAD_PROGRESS("4");
		{
			std::lock_guard <std::mutex> lock(clientsLock);
			for(auto &cli : clients)
			{
				if(0<cli.sendBuf.size())
				{
					bool ready=false;
				#ifdef _WIN32
					fd_set set;
					timeval wait;
					wait.tv_sec=0;
					wait.tv_usec=100;
					FD_ZERO(&set);
					FD_SET(cli.sock,&set);
					if(select(1,NULL,&set,NULL,&wait)>=1)
					{
						ready=true;
					}
				#else
					struct pollfd pfd;
					pfd.fd=cli.sock;
					pfd.events=POLLOUT;
					pfd.revents=0;
					if(poll(&pfd,1,1)>=1)
					{
						ready=true;
					}
				#endif
					if(true==ready)
					{
						send(cli.sock,(char *)cli.sendBuf.data(),cli.sendBuf.size(),0);
						cli.sendBuf.clear();
					}
				}
			}
		}
		THREAD_PROGRESS("4");
		{
			std::lock_guard <std::mutex> lock(DNSRequestLock);
			for(auto &req : DNSReq)
			{
				if(DNS_REQUESTED==req.state)
				{
					struct hostent *table=gethostbyname(req.hostname.c_str());
					if(nullptr==table)
					{
						req.state=DNS_NOT_FOUND;
					}
					else
					{
						req.state=DNS_FOUND;
						req.ipAddr[0]=((unsigned char *)table->h_addr_list[0])[0];
						req.ipAddr[1]=((unsigned char *)table->h_addr_list[0])[1];
						req.ipAddr[2]=((unsigned char *)table->h_addr_list[0])[2];
						req.ipAddr[3]=((unsigned char *)table->h_addr_list[0])[3];
					}
				}
			}
		}
		THREAD_PROGRESS("5");
	}

	THREAD_PROGRESS("Fin");

	CleanUp();

	THREAD_PROGRESS(".");
}

void RealNetwork::Start(void)
{
	if(true!=started)
	{
		std::cout << "Starting RealNetwork.\n";
		std::thread t(&RealNetwork::ThreadFunc,this);
		workerThread=std::move(t);
		started=true;
	}
	else
	{
		std::cout << "Error: RealNetwork is already running.\n";
	}
}
void RealNetwork::End(void)
{
	if(true==started)
	{
		stopThread=true;
		workerThread.join();
		started=false;
	}
}

// Called from the VM thread.
void RealNetwork::RequestTCPConnection(uint16_t VMPort,const uint8_t IPv4Addr[4],uint16_t dstPort)
{
	TCPConnectionRequest req;
	req.conn.IPv4Addr[0]=IPv4Addr[0];
	req.conn.IPv4Addr[1]=IPv4Addr[1];
	req.conn.IPv4Addr[2]=IPv4Addr[2];
	req.conn.IPv4Addr[3]=IPv4Addr[3];
	req.conn.dstPort=dstPort;
	req.conn.VMPort=VMPort;

	std::lock_guard <std::mutex> lock(TCPConnReqLock);
	TCPConnReq.push_back(req);
}

void RealNetwork::RequestDNS(std::string hostname)
{
	DNSRequest req;
	req.hostname=hostname;

	std::lock_guard <std::mutex> lock(DNSRequestLock);
	DNSReq.push_back(req);
}

void RealNetwork::StartUp(void)
{
#ifdef _WIN32
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested=MAKEWORD(1,1);
	if(WSAStartup(wVersionRequested,&wsaData)!=0)
	{
		printf("Error occured in WSAStartup\n");
	}
#endif
}
void RealNetwork::CleanUp(void)
{
#ifdef _WIN32
	WSACleanup();
#endif
}

bool RealNetwork::TCPConnectionRequest:: DoConnect(void)
{
	this->sock=socket(AF_INET,SOCK_STREAM,0);

	SOCKADDR_IN addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(conn.dstPort);

std::cout << "2\n";

#ifdef _WIN32
	addr.sin_addr.S_un.S_un_b.s_b1=conn.IPv4Addr[0];
	addr.sin_addr.S_un.S_un_b.s_b2=conn.IPv4Addr[1];
	addr.sin_addr.S_un.S_un_b.s_b3=conn.IPv4Addr[2];
	addr.sin_addr.S_un.S_un_b.s_b4=conn.IPv4Addr[3];
#else
	memcpy(&addr.sin_addr.s_addr,conn.IPv4Addr,4);
#endif

	for(int sainit=0; sainit<8; sainit++)
	{
		addr.sin_zero[sainit]=0;
	}

	if(connect(sock,(SOCKADDR *)&addr,sizeof(SOCKADDR_IN))!=0)
	{
		std::cout << "Connection Error\n";
	#ifdef _WIN32
		closesocket(sock);
	#else
		close(sock);
	#endif
		return false;
	}

#ifdef _WIN32
	BOOL opt=TRUE;
	setsockopt(sock,IPPROTO_TCP,TCP_NODELAY,(char *)&opt,sizeof(opt));
#endif

	return true;
}
