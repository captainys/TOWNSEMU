#include <iostream>
#include "real_network.h"

#ifdef _WIN32
	#include <windows.h>
	#pragma comment(lib,"wsock32.lib")
	typedef int socklen_t;
#endif

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
		if(0==clients.size())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		else
		{
		#ifdef _WIN32
			std::lock_guard <std::mutex> lock(clientsLock);

			// --- Windows Path: select ---
			fd_set readfds;
			FD_ZERO(&readfds);
			
			for(auto &cli : clients)
			{
				cli.bytesIncoming=false;
				FD_SET(cli.sock, &readfds);
			}

			// 10ms timeout
			timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 10000; 

			// nfds is ignored on Windows, but usually set to 0 or max fd + 1
			if (0<select(0,&readfds,NULL,NULL,&tv))
			{
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
			std::lock_guard <std::mutex> lock(clientsLock);

			std::vector<struct pollfd> fds;
			for(auto &cli : clients)
			{
				cli.bytesIncoming=false;

				struct pollfd pfd;
				pfd.fd=cli.sock;
				pfd.events=POLLIN;
				fds.push_back(pfd);
			}

			// poll uses milliseconds (10ms)
			if (0<poll(fds.data(),(nfds_t)fds.size(),10)) 
			{
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
				++iter;

				auto &cli=*iter;
				if(true==cli.bytesIncoming)
				{
					cli.bytesIncoming=false;

					const int bufLen=4096;
					uint8_t buf[bufLen];
					int nBytesRecv=recv(cli.sock,(char *)buf,sizeof(buf),0);
					if(SOCKET_ERROR==nBytesRecv || 0==nBytesRecv)
					{
						closesocket(cli.sock);
						clients.erase(iter);
					}
					else
					{
						cli.recvBuf.insert(cli.recvBuf.end(),buf,buf+nBytesRecv);
					}
				}
			}
		}

		{
			// Process connection requests.
			std::lock_guard <std::mutex> lock(TCPConnReqLock);
			unsigned int nConnected=0;
			for(auto &req : TCPConnReq)
			{
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
	}

	CleanUp();
}

void RealNetwork::Start(void)
{
	if(true!=started)
	{
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
