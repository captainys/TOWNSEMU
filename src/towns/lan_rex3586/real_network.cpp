#include <iostream>
#include "cpputil.h"
#include "real_network.h"

#ifdef _WIN32
	#include <windows.h>
	#pragma comment(lib,"wsock32.lib")
	typedef int socklen_t;
#else
	#define SD_SEND SHUT_WR
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

			int i=0;
			for(auto &cli : clients)
			{
				FD_SET(cli.sock, &readfds); // cli.sock is not accessed from outside of Network Thread
				++i;
			}
			if(64<i)
			{
				std::cout << "FD_SET overflow!\n";
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
							if(true==monitor)
							{
								std::cout << "R1 Disconnected VMPort:" << uint16_t(cli.conn.VMPort) << " RemotePort:" << uint16_t(cli.conn.dstPort);
								std::cout << " IP:" << uint16_t(cli.conn.IPv4Addr[0]) << ".";
								std::cout << uint16_t(cli.conn.IPv4Addr[1]) << ".";
								std::cout << uint16_t(cli.conn.IPv4Addr[2]) << ".";
								std::cout << uint16_t(cli.conn.IPv4Addr[3]) << "\n";
							}
							cli.state=STATE_DISCONNECTED_NEED_TO_SEND_FIN;
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
			// Problem: connect function may block long time if the server does not respond.
			// Solution:
			//   Lock
			//   Make a copy of TCPConnReq
			//   Unlock

			//   Do all connections.

			//   (VirtualNetwork may add TCPConnReq while connecting.)

			//   Lock
			//   Compare exisiting request and connected requests and copy connected flags of matching requests.

			std::vector <TCPConnectionRequest> TCPConnReqCopy;
			{
				std::lock_guard <std::mutex> lock(TCPConnReqLock);
				TCPConnReqCopy=TCPConnReq;
			}


			// Process connection requests.
			unsigned int nConnected=0;
			for(auto &req : TCPConnReqCopy)
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
				{
					std::lock_guard <std::mutex> lock(TCPConnReqLock);
					for(auto &copy : TCPConnReqCopy)
					{
						bool matched=false;
						for(auto &req : TCPConnReq)
						{
							if(copy.conn.IPv4Addr[0]==req.conn.IPv4Addr[0]	&&
							   copy.conn.IPv4Addr[1]==req.conn.IPv4Addr[1]	&&
							   copy.conn.IPv4Addr[2]==req.conn.IPv4Addr[2]	&&
							   copy.conn.IPv4Addr[3]==req.conn.IPv4Addr[3]	&&
							   copy.conn.dstPort==req.conn.dstPort	&&
							   copy.conn.VMPort==req.conn.VMPort)
							{
								req=copy;
								matched=true;
								break;
							}
						}
						if(true!=matched && true==copy.connected)
						{
							closesocket(copy.sock);
						}
					}
				}
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
						if(true==monitor)
						{
							std::cout << cli.sendBuf.size() << " bytes from VMPort " << cli.conn.VMPort << " sent to remote port " << cli.conn.dstPort << "\n";
						}
						send(cli.sock,(char *)cli.sendBuf.data(),cli.sendBuf.size(),0);
						cli.sendBuf.clear();
					}
				}

				if(true==cli.FIN_initiated_from_VM && true!=cli.ShutdownInitiated)
				{
					if(true==monitor)
					{
						std::cout << "Shutdown Initiated VMPort:" << cli.conn.VMPort << " DstPort:" << cli.conn.dstPort << "\n";
					}
					cli.ShutdownInitiated=true;
					shutdown(cli.sock,SD_SEND);
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
						if(true==monitor)
						{
							std::cout << "Host " << req.hostname << " not found.\n";
						}
					}
					else
					{
						req.state=DNS_FOUND;
						req.ipAddr[0]=((unsigned char *)table->h_addr_list[0])[0];
						req.ipAddr[1]=((unsigned char *)table->h_addr_list[0])[1];
						req.ipAddr[2]=((unsigned char *)table->h_addr_list[0])[2];
						req.ipAddr[3]=((unsigned char *)table->h_addr_list[0])[3];
						if(true==monitor)
						{
							std::cout << "Host " << req.hostname << " " << uint32_t(req.ipAddr[0]) << "." << uint32_t(req.ipAddr[1]) << "." << uint32_t(req.ipAddr[2]) << "." << uint32_t(req.ipAddr[3]) << "\n";
						}
					}
				}
			}
		}
		THREAD_PROGRESS("5");
		{
			portForwardingLock.lock();
			auto portForwardingCopy=portForwarding;
			portForwardingLock.unlock();

			uint32_t newListen=0;
			for(auto &port : portForwardingCopy)
			{
				if(STATE_NOT_LISTENING==port.state)
				{
					port.sock=socket(AF_INET,SOCK_STREAM,0);
					if(INVALID_SOCKET!=port.sock)
					{
					#ifndef _WIN32
						struct linger lingerOpt;
						lingerOpt.l_onoff=1;
						lingerOpt.l_linger=10;
						setsockopt(port.sock,SOL_SOCKET,SO_LINGER,(void *)&lingerOpt,sizeof(lingerOpt));
					#endif

						SOCKADDR_IN addr;
						addr.sin_family=AF_INET;
						addr.sin_port=htons(port.HostPort);
						addr.sin_addr.s_addr=INADDR_ANY;
						if(0!=bind(port.sock,(SOCKADDR *)&addr,sizeof(SOCKADDR_IN)))
						{
							closesocket(port.sock);
							port.sock=INVALID_SOCKET;
							continue;
						}
						if(0!=listen(port.sock,listen_max))
						{
							closesocket(port.sock);
							port.sock=INVALID_SOCKET;
							continue;
						}
						port.state=STATE_LISTENING;
						++newListen;
					}
				}
			}

			if(0<newListen)
			{
				std::lock_guard <std::mutex> lock(portForwardingLock);
				for(auto &port : portForwarding)
				{
					for(auto &copy : portForwardingCopy)
					{
						if(port.VMPort==copy.VMPort &&
						   port.HostPort==copy.HostPort &&
						   STATE_NOT_LISTENING==port.state &&
						   STATE_LISTENING==copy.state)
						{
							port=copy;
						}
					}
				}
			}

			// Check incoming connections.
		#ifdef _WIN32
			// --- Windows Path: select ---
			fd_set readfds;
			FD_ZERO(&readfds);

			int i=0;
			for(auto &lstn : portForwardingCopy)
			{
				lstn.connectionIncoming=false;
				if(STATE_LISTENING==lstn.state)
				{
					FD_SET(lstn.sock, &readfds); // cli.sock is not accessed from outside of Network Thread
					++i;
				}
			}
			if(64<i)
			{
				std::cout << "FD_SET overflow!\n";
			}

			// 10ms timeout
			timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 10000; 
			// nfds is ignored on Windows, but usually set to 0 or max fd + 1
			if (0<select(0,&readfds,NULL,NULL,&tv))
			{
				for(auto &lstn : portForwardingCopy)
				{
					if(FD_ISSET(lstn.sock,&readfds))
					{
						lstn.connectionIncoming=true;
					}
				}
			}
		#else
			// --- POSIX Path: poll ---
			std::vector<struct pollfd> fds;
			for(auto &lstn : portForwardingCopy)
			{
				struct pollfd pfd;
				pfd.fd=lstn.sock;
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
						for(auto &lstn : portForwardingCopyo)
						{
							if(lstn.sock==fd.fd)
							{
								lstn.connectionIncoming=true;
								break;
							}
						}
					}
				}
			}
		#endif
			for(auto lstn : portForwardingCopy)
			{
				if(true==lstn.connectionIncoming)
				{
					socklen_t addrlen=sizeof(SOCKADDR_IN);
					SOCKADDR_IN addr;
					SOCKET sock=accept(lstn.sock,(SOCKADDR *)&addr,&addrlen);
					if(INVALID_SOCKET!=sock)
					{
					#ifdef _WIN32
						BOOL b=TRUE;
						setsockopt(sock,IPPROTO_TCP,TCP_NODELAY,(char *)&b,sizeof(b));
					#endif
						#ifdef _WIN32
						uint16_t remotePort=addr.sin_port;
						uint8_t ipAddr[4]=
						{
							addr.sin_addr.S_un.S_un_b.s_b1,
							addr.sin_addr.S_un.S_un_b.s_b2,
							addr.sin_addr.S_un.S_un_b.s_b3,
							addr.sin_addr.S_un.S_un_b.s_b4,
						};
						#else
						uint16_t remotePort=addr.sin_port;
						uint8_t ipAddr[4]=
						{
							(addr.sin_addr.s_addr>>24)&255,
							(addr.sin_addr.s_addr>>16)&255,
							(addr.sin_addr.s_addr>> 8)&255,
							addr.sin_addr.s_addr     &255,
						};
						#endif

						Client newCli;
						newCli.state=STATE_JUST_ACCEPTED;
						newCli.sock=sock;
						newCli.conn.VMPort=lstn.VMPort;
						newCli.conn.dstPort=remotePort;
						newCli.conn.IPv4Addr[0]=ipAddr[0];
						newCli.conn.IPv4Addr[1]=ipAddr[1];
						newCli.conn.IPv4Addr[2]=ipAddr[2];
						newCli.conn.IPv4Addr[3]=ipAddr[3];

						clientsLock.lock();
						clients.push_back(newCli);
						clientsLock.unlock();
					}
				}
			}
		}
		THREAD_PROGRESS("6");
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
	{
		std::lock_guard <std::mutex> lock(clientsLock);
		for(auto &cli : clients)
		{
			if(cli.conn.IPv4Addr[0]==IPv4Addr[0] &&
			   cli.conn.IPv4Addr[1]==IPv4Addr[1] &&
			   cli.conn.IPv4Addr[2]==IPv4Addr[2] &&
			   cli.conn.IPv4Addr[3]==IPv4Addr[3] &&
			   cli.conn.dstPort==dstPort &&
			   cli.conn.VMPort==VMPort)
			{
				if(STATE_JUST_CONNECTED==cli.state)
				{
					// Leave it.  In the next polling, it will be Established.
					return;
				}
				if(STATE_CONNECTED==cli.state)
				{
					cli.state=STATE_JUST_CONNECTED;
					return;
				}
				if(STATE_DISCONNECTED==cli.state || STATE_DISCONNECTED_BUT_DATA_LEFTOVER==cli.state)
				{
					std::cout << "Matched Disconnected Port.\n";
					cli.conn.IPv4Addr[0]=0xFF;
					cli.conn.IPv4Addr[1]=0xFF;
					cli.conn.IPv4Addr[2]=0xFF;
					cli.conn.IPv4Addr[3]=0xFF;
					break;
				}
			}
		}
	}

	std::lock_guard <std::mutex> lock(TCPConnReqLock);
	for(auto &req : TCPConnReq)
	{
		if(req.conn.IPv4Addr[0]==IPv4Addr[0] &&
		   req.conn.IPv4Addr[1]==IPv4Addr[1] &&
		   req.conn.IPv4Addr[2]==IPv4Addr[2] &&
		   req.conn.IPv4Addr[3]==IPv4Addr[3] &&
		   req.conn.dstPort==dstPort &&
		   req.conn.VMPort==VMPort)
		{
			// If the request is already in, do nothing.
			return;
		}
	}

	TCPConnectionRequest req;
	req.conn.IPv4Addr[0]=IPv4Addr[0];
	req.conn.IPv4Addr[1]=IPv4Addr[1];
	req.conn.IPv4Addr[2]=IPv4Addr[2];
	req.conn.IPv4Addr[3]=IPv4Addr[3];
	req.conn.dstPort=dstPort;
	req.conn.VMPort=VMPort;

	TCPConnReq.push_back(req);
}

void RealNetwork::RequestDNS(std::string hostname)
{
	DNSRequest req;
	req.hostname=hostname;

	std::lock_guard <std::mutex> lock(DNSRequestLock);
	DNSReq.push_back(req);
}

void RealNetwork::ResetReceived(uint16_t VMPort,const uint8_t IPv4Addr[4],uint16_t dstPort)
{
	std::lock_guard <std::mutex> lock(clientsLock);
	for(auto iter=clients.begin(); clients.end()!=iter; )
	{
		if(iter->conn.VMPort==VMPort &&
		   iter->conn.IPv4Addr[0]==IPv4Addr[0] &&
		   iter->conn.IPv4Addr[1]==IPv4Addr[1] &&
		   iter->conn.IPv4Addr[2]==IPv4Addr[2] &&
		   iter->conn.IPv4Addr[3]==IPv4Addr[3] &&
		   iter->conn.dstPort==dstPort &&
		   iter->conn.VMPort==VMPort)
		{
			closesocket(iter->sock);
			iter=clients.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void RealNetwork::RequestPortForwarding(uint16_t VMPort,uint16_t HostPort)
{
	PortForwarding lstn;
	lstn.VMPort=VMPort;
	lstn.HostPort=HostPort;

	std::lock_guard <std::mutex> lock(portForwardingLock);
	portForwarding.push_back(lstn);
}

void RealNetwork::AddStatusText(std::vector <std::string> &text) const
{
	{
		std::lock_guard <std::mutex> lock(clientsLock);
		for(auto &cli : clients)
		{
			std::string str;
			str="TCP VMPort:";
			str+=cpputil::Ustox(cli.conn.VMPort);
			str+="H IP:";
			str+=cpputil::Uitox(cli.conn.GetIPUint32());
			str+="H Remote Port:";
			str+=cpputil::Ustox(cli.conn.dstPort);
			str.push_back(' ');
			switch(cli.state)
			{
			case STATE_DISCONNECTED:
				str+="DISCONNECTED";
				break;
			case STATE_JUST_CONNECTED:  // Connected, but the VM thread does not know yet.
				str+="JUST_CONNECTED";
				break;
			case STATE_CONNECTED:       // Connection established, VM thread knows about it.
				str+="CONNECTED";
				break;
			case STATE_DISCONNECTED_BUT_DATA_LEFTOVER:
				str+="DISCONNECTED_WITH_LEFTOVER";
				break;
			case STATE_DISCONNECTED_NEED_TO_SEND_FIN:
				str+="DISCONNECTED_NEED_TO_SEND_FIN";
				break;
			case STATE_JUST_ACCEPTED:
				str+="JUST_ACCEPTED";
				break;
			case STATE_ACCEPTED_NOTIFIED:
				str+="ACCEPTED_NOTIFIED";
				break;
			default:
				str+="!! UNDEFINED STATE !!";
				break;
			}

			str+=" RecvBuf:";
			str+=cpputil::Ustox(cli.recvBuf.size());
			str+="H bytes";

			str+=" SendBuf:";
			str+=cpputil::Ustox(cli.sendBuf.size());
			str+="H bytes";

			if(true==cli.FIN_initiated_from_VM)
			{
				str+=" FIN-from-VM";
			}
			if(true==cli.ShutdownInitiated)
			{
				str+=" Shutdown";
			}
			text.push_back(str);
		}
	}
	{
		std::lock_guard <std::mutex> lock(portForwardingLock);
		for(auto &lstn : portForwarding)
		{
			std::string str="Port Fwd  VMPort:";
			str+=cpputil::Ustox(lstn.VMPort);
			str+="H HostPort:";
			str+=cpputil::Ustox(lstn.HostPort);
			str.push_back('H');

			str.push_back(' ');

			switch(lstn.state)
			{
			case STATE_NOT_LISTENING:
				str+="NOT_LISTENING";
				break;
			case STATE_LISTENING:
				str+="LISTENING";
				break;
			default:
				str+="!! UNDEFINED STATE !!";
				break;
			}
			text.push_back(str);
		}
	}
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
