// ioctlsocket can set socket to non-blocking mode
//

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

#include "yssocket.h"


YsSocketServer::YsSocketServer(int listen,int maxNumCli)
{
	started=YSFALSE;

	listeningPort=listen;

	maxNumClient=maxNumCli;

	clientSock=new SOCKET [maxNumCli];
	clientSockUsed=new YSBOOL [maxNumCli];
	clientReady=new YSBOOL [maxNumCli];

	int i;
	for(i=0; i<maxNumCli; i++)
	{
		clientSockUsed[i]=YSFALSE;
	}
}


YsSocketServer::~YsSocketServer()
{
	if(started==YSTRUE)
	{
		Terminate();
	}

	delete [] clientSock;
	delete [] clientSockUsed;
	delete [] clientReady;
}


YSRESULT YsSocketServer::Start(void)
{
	if(started!=YSTRUE)
	{
		SOCKADDR_IN addr;
#ifdef _WIN32
		WORD wVersionRequested;
		WSADATA wsaData;

		wVersionRequested=MAKEWORD(1,1);
		if(WSAStartup(wVersionRequested,&wsaData)!=0)
		{
			printf("Error occured in WSAStartup\n");
			return YSERR;
		}
#endif

		listeningSocket=socket(PF_INET,SOCK_STREAM,0);
		if(listeningSocket==-1)
		{
			printf("Error occuerd in socket(PF_INET,SOCK_STREAM,0);\n");
			return YSERR;
		}

#ifndef _WIN32
		struct linger lingerOpt;
		lingerOpt.l_onoff=1;
		lingerOpt.l_linger=10;
		setsockopt(listeningSocket,SOL_SOCKET,SO_LINGER,(void *)&lingerOpt,sizeof(lingerOpt));
#endif


		addr.sin_family=AF_INET;
		addr.sin_port=htons(listeningPort);
		addr.sin_addr.s_addr=INADDR_ANY;



		if(bind(listeningSocket,(SOCKADDR *)&addr,sizeof(SOCKADDR_IN))!=0)
		{
#ifdef _WIN32
			closesocket(listeningSocket);
#else
			close(listeningSocket);
#endif
			printf("Error occured in bind()\n");
			return YSERR;
		}
		if(listen(listeningSocket,5)!=0)   // 5 should be replaced with maxNumClient####
		{
#ifdef _WIN32
			closesocket(listeningSocket);
#else
			close(listeningSocket);
#endif
			printf("Error occured in listen()\n");
			return YSERR;
		}

		int i;
		for(i=0; i<maxNumClient; i++)
		{
			clientSockUsed[i]=YSFALSE;
		}

		started=YSTRUE;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsSocketServer::Terminate(void)
{
	if(started==YSTRUE)
	{
		int i;
		for(i=0; i<maxNumClient; i++)
		{
			Disconnect(i);
		}

#ifdef _WIN32
		closesocket(listeningSocket);
		WSACleanup();
#else
		close(listeningSocket);
#endif

		started=YSFALSE;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}


YSRESULT YsSocketServer::CheckAndAcceptConnection(void)
{
	YSBOOL ready;
	ready=YSFALSE;

#ifdef _WIN32
	fd_set set;
	timeval wait;
	FD_ZERO(&set);
	FD_SET(listeningSocket,&set);
	wait.tv_sec=0;
	wait.tv_usec=0;
	if(select(1,&set,NULL,NULL,&wait)>=1)
	{
		ready=YSTRUE;
	}
#else
	struct pollfd pfd;
	pfd.fd=listeningSocket;
	pfd.events=POLLIN;
	pfd.revents=0;
	if(poll(&pfd,1,1)>=1)
	{
		ready=YSTRUE;
	}
#endif

	if(ready==YSTRUE)
	{
		socklen_t addrlen;
		SOCKADDR_IN addr;

		int i;
		for(i=0; i<maxNumClient; i++)
		{
			if(clientSockUsed[i]!=YSTRUE)
			{
				addrlen=sizeof(SOCKADDR_IN);
				clientSock[i]=accept(listeningSocket,(SOCKADDR *)&addr,&addrlen);

			#ifdef _WIN32
				BOOL b;                                                                  // 2009/04/05
				b=TRUE;                                                                  // 2009/04/05
				setsockopt(clientSock[i],IPPROTO_TCP,TCP_NODELAY,(char *)&b,sizeof(b));  // 2009/04/05
			#endif

				unsigned int ipAddr[4];
			#ifdef _WIN32
				ipAddr[0]=addr.sin_addr.S_un.S_un_b.s_b1;
				ipAddr[1]=addr.sin_addr.S_un.S_un_b.s_b2;
				ipAddr[2]=addr.sin_addr.S_un.S_un_b.s_b3;
				ipAddr[3]=addr.sin_addr.S_un.S_un_b.s_b4;
			#else
				ipAddr[3]=(addr.sin_addr.s_addr>>24)&255;
				ipAddr[2]=(addr.sin_addr.s_addr>>16)&255;
				ipAddr[1]=(addr.sin_addr.s_addr>> 8)&255;
				ipAddr[0]= addr.sin_addr.s_addr     &255;
			#endif

				if(clientSock[i]!=INVALID_SOCKET)
				{
					clientSockUsed[i]=YSTRUE;

					int value;
					socklen_t len;
					len=4;
					if(getsockopt(clientSock[i],SOL_SOCKET,SO_SNDBUF,(char *)&value,&len)==0)
					{
						printf("Send Buffer Size=%d\n",value);
					}
					else
					{
						printf("Cannot retrieve Send Buffer Size\n");
					}

					if(getsockopt(clientSock[i],SOL_SOCKET,SO_SNDTIMEO,(char *)&value,&len)==0)
					{
						printf("Send Time Out=%d\n",value);
					}
					else
					{
						printf("Cannot retrieve Send Time Out\n");
					}


					ConnectionAccepted(i,ipAddr);
					return YSOK;
				}
			}
		}
	}
	return YSERR;
}

YSRESULT YsSocketServer::CheckReceive(void)
{
	int byteReceived;
	int i;

	for(i=0; i<maxNumClient; i++)
	{
	REPEAT:
		if(clientSockUsed[i]==YSTRUE)
		{
			YSBOOL ready;
			ready=YSFALSE;

		#ifdef _WIN32
			fd_set set;
			timeval wait;
			FD_ZERO(&set);
			FD_SET(clientSock[i],&set);
			wait.tv_sec=0;  // Linux select modifies wait.
			wait.tv_usec=0; // It must be reset right before select in a loop.
			if(select(1,&set,NULL,NULL,&wait)>=1)
			{
				ready=YSTRUE;
			}
		#else
			struct pollfd pfd;
			pfd.fd=clientSock[i];
			pfd.events=POLLIN;
			pfd.revents=0;
			if(poll(&pfd,1,1)>=1)
			{
				ready=YSTRUE;
			}
		#endif

			if(ready==YSTRUE)
			{
				byteReceived=recv(clientSock[i],(char *)buffer,nBufferSize,0);
				if(byteReceived==0 || byteReceived==SOCKET_ERROR)
				{
				#ifdef _WIN32
					closesocket(clientSock[i]);
				#else
					close(clientSock[i]);
				#endif
					clientSockUsed[i]=YSFALSE;
					ConnectionClosedByClient(i);
				}
				else if(byteReceived>0)
				{
					ReceivedFrom(i,byteReceived,buffer);
					goto REPEAT;  /* Check remainder */
				}
			}
		}
	}
	return YSOK;
}


int YsSocketServer::GetNumClient(void) const
{
	return maxNumClient;
}

YSBOOL YsSocketServer::IsClientActive(int clientId) const
{
	if(0<=clientId && clientId<maxNumClient)
	{
		return clientSockUsed[clientId];
	}
	return YSFALSE;
}

int YsSocketServer::GetNumConnectedClient(void) const
{
	int n=0;
	for(int i=0; i<GetNumClient(); ++i)
	{
		if(YSTRUE==IsClientActive(i))
		{
			++n;
		}
	}
	return n;
}

YSRESULT YsSocketServer::Disconnect(int clientId)   /* clientId=-1 to close all connections */
{
	if(0<=clientId && clientId<maxNumClient && clientSockUsed[clientId]==YSTRUE)
	{
#ifdef _WIN32
		closesocket(clientSock[clientId]);
#else
		close(clientSock[clientId]);
#endif
		clientSockUsed[clientId]=YSFALSE;
		return YSOK;
	}

	return YSERR;
}

YSRESULT YsSocketServer::Send(int clientId,YSSIZE_T nBytes,unsigned char dat[],unsigned timeout)
      /* clientId=-1 to broadcast */
{
	if(0<=clientId && clientId<maxNumClient && clientSockUsed[clientId]==YSTRUE)
	{
		YSBOOL ready;
		ready=YSFALSE;

	#ifdef _WIN32
		fd_set set;
		timeval wait;
		wait.tv_sec=timeout/1000;
		wait.tv_usec=timeout%1000;
		FD_ZERO(&set);
		FD_SET(clientSock[clientId],&set);
		if(select(1,NULL,&set,NULL,&wait)>=1)
		{
			ready=YSTRUE;
		}
	#else
		struct pollfd pfd;
		pfd.fd=clientSock[clientId];
		pfd.events=POLLOUT;  // 2004/01/12
		pfd.revents=0;
		if(poll(&pfd,1,1)>=1)
		{
			ready=YSTRUE;
		}
	#endif


		if(ready==YSTRUE)
		{
			send(clientSock[clientId],(char *)dat,(int)nBytes,0);
			return YSOK;
		}
		else
		{
			return YSERR;  // Timeout
		}
	}
	else if(clientId<0)
	{
		int i;

		for(i=0; i<maxNumClient; i++)
		{
			if(clientSockUsed[i]==YSTRUE)
			{
				YSBOOL ready;
				ready=YSFALSE;

			#ifdef _WIN32
				fd_set set;
				timeval wait;
				FD_ZERO(&set);
				FD_SET(clientSock[i],&set);
				wait.tv_sec=timeout/1000;
				wait.tv_usec=timeout%1000;   // Wait 10 microseconds
				if(select(1,NULL,&set,NULL,&wait)>=1)
				{
					ready=YSTRUE;
				}
			#else
				struct pollfd pfd;
				pfd.fd=clientSock[i];
				pfd.events=POLLOUT;
				pfd.revents=0;
				if(poll(&pfd,1,1)>=1)
				{
					ready=YSTRUE;
				}
			#endif

				if(ready==YSTRUE)
				{
					send(clientSock[i],(char *)dat,(int)nBytes,0);
				}
				else
				{
					return YSERR;   // Timeout
				}
			}
		}
		return YSOK;
	}
	return YSERR;
}

// YSRESULT YsSocketServer::ReceivedFrom(int clientId,YSSIZE_T nBytes,unsigned char dat[])
// {
// 	// This function must be overridden
// 	printf("SVR : Received a message from Client[%d]\n",clientId);
// 	printf("  \"%s\"\n",dat);
// 
// 	return YSOK;
// }

YSRESULT YsSocketServer::SendTerminateMessage(int clientId,unsigned timeout)
{
	// This function must be overridden
	const char *dat="  Connection Terminating.\n";

	Send(clientId,(int)strlen(dat)+1,(unsigned char *)dat,timeout);

	return YSOK;
}

YSRESULT YsSocketServer::ConnectionAccepted(int clientId,unsigned int ipAddr[])
{
	// This function must be overridden
	printf("SVR : Connection from Client[%d] is accepted\n",clientId);

	return YSOK;
}

YSRESULT YsSocketServer::ConnectionClosedByClient(int clientId)
{
	// This function must be overridden
	printf("SVR : Connection closed from Client[%d]\n",clientId);

	return YSOK;
}


////////////////////////////////////////////////////////////



YsSocketClient::YsSocketClient()
{
	Initialize(-1);
}

YsSocketClient::YsSocketClient(int port)
{
	Initialize(port);
}

void YsSocketClient::Initialize(int port)
{
	started=YSFALSE;
	connected=YSFALSE;
	this->port=port;
}

YsSocketClient::~YsSocketClient()
{
	if(started==YSTRUE)
	{
		Terminate();
	}
}

YSRESULT YsSocketClient::Start(int port)
{
	this->port=port;
	return Start();
}

YSRESULT YsSocketClient::Start(void)
{
	if(0>port)
	{
		printf("Error: Port not set.\n");
		return YSERR;
	}

	if(started!=YSTRUE)
	{
#ifdef _WIN32
		WORD wVersionRequested;
		WSADATA wsaData;

		wVersionRequested=MAKEWORD(1,1);
		if(WSAStartup(wVersionRequested,&wsaData)!=0)
		{
			printf("Error occured in WSAStartup\n");
			return YSERR;
		}
#endif

		started=YSTRUE;
		connected=YSFALSE;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsSocketClient::Terminate(void)
{
	if(started==YSTRUE)
	{
		if(connected==YSTRUE)
		{
			Disconnect();
		}

#ifdef _WIN32
		WSACleanup();
#endif

		started=YSFALSE;
		return YSOK;
	}
	return YSERR;
}


YSRESULT YsSocketClient::Connect(const char host[])
{
	if(started!=YSTRUE)
	{
		printf("Client not started.\n");
		printf("Call Start().\n");
		return YSERR;
	}

	if(connected!=YSTRUE)
	{
		YSBOOL IpDirect;
		int i,ipset,ip[4];
		sock=socket(PF_INET,SOCK_STREAM,0);

		IpDirect=YSTRUE;
		ip[0]=atoi(host);
		ipset=1;
		for(i=0; host[i]!=0; i++)
		{
			if((host[i]<'0' || '9'<host[i]) && host[i]!='.')
			{
				IpDirect=YSFALSE;
				break;
			}
			else if(host[i]=='.')
			{
				if(ipset<4)
				{
					ip[ipset++]=atoi(&host[i+1]);
				}
				else
				{
					IpDirect=YSFALSE;
					break;
				}
			}
		}

		SOCKADDR_IN addr;
		addr.sin_family=AF_INET;
		addr.sin_port=htons(port);
		switch(IpDirect)
		{
		default:
			break;
		case YSFALSE:
			struct hostent *table;
			table=gethostbyname(host);
			if(table==NULL)
			{
#ifdef _WIN32
				closesocket(sock);
#else
				close(sock);
#endif
				return YSERR;
			}

#ifdef _WIN32
			addr.sin_addr.S_un.S_un_b.s_b1=
			    ((unsigned char *)table->h_addr_list[0])[0];
			addr.sin_addr.S_un.S_un_b.s_b2=
			    ((unsigned char *)table->h_addr_list[0])[1];
			addr.sin_addr.S_un.S_un_b.s_b3=
			    ((unsigned char *)table->h_addr_list[0])[2];
			addr.sin_addr.S_un.S_un_b.s_b4=
			    ((unsigned char *)table->h_addr_list[0])[3];

			printf("%d.%d.%d.%d\n",
				addr.sin_addr.S_un.S_un_b.s_b1,
				addr.sin_addr.S_un.S_un_b.s_b2,
				addr.sin_addr.S_un.S_un_b.s_b3,
				addr.sin_addr.S_un.S_un_b.s_b4);
#else
			bcopy(table->h_addr_list[0],&addr.sin_addr,table->h_length);
#endif

			break;
		case YSTRUE:
			if(ipset>=4)
			{
#ifdef _WIN32
				addr.sin_addr.S_un.S_un_b.s_b1=ip[0];
				addr.sin_addr.S_un.S_un_b.s_b2=ip[1];
				addr.sin_addr.S_un.S_un_b.s_b3=ip[2];
				addr.sin_addr.S_un.S_un_b.s_b4=ip[3];
#else
				unsigned suck;
				suck=inet_addr(host);

				/* This gethostbyaddr function does not work when an address is registered in DNS.
				Like 192.168.1.100 is not accepted.

				table=gethostbyaddr((char *)&suck,4,AF_INET);
				if(table==NULL)
				{
					close(sock);
					return YSERR;
				}
				bcopy(table->h_addr_list[0],&addr.sin_addr,table->h_length); */

				bcopy(&suck,&addr.sin_addr,sizeof(suck));
#endif
			}
			else
			{
				printf("Invalid Address");
				return YSERR;
			}
			break;
		}

		for(int sainit=0; sainit<8; sainit++)
		{
			addr.sin_zero[sainit]=0;
		}

		char buf[260];
#ifdef _WIN32
		sprintf(buf,"%d.%d.%d.%d",
			(int)addr.sin_addr.S_un.S_un_b.s_b1,
			(int)addr.sin_addr.S_un.S_un_b.s_b2,
			(int)addr.sin_addr.S_un.S_un_b.s_b3,
			(int)addr.sin_addr.S_un.S_un_b.s_b4);
#else
		strcpy(buf,inet_ntoa(addr.sin_addr));
#endif
		printf("Trying to : %s\n",buf);

		if(connect(sock,(SOCKADDR *)&addr,sizeof(SOCKADDR_IN))!=0)
		{
			printf("Error occured in connect()\n");
#ifdef _WIN32
			closesocket(sock);
#else
			close(sock);
#endif
			return YSERR;
		}
		printf("Connected.\n");

#ifdef _WIN32
		BOOL b;                                                         // 2009/04/05
		b=TRUE;                                                         // 2009/04/05
		setsockopt(sock,IPPROTO_TCP,TCP_NODELAY,(char *)&b,sizeof(b));  // 2009/04/05
#endif

		connected=YSTRUE;

		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsSocketClient::Disconnect(void)
{
	if(connected==YSTRUE)
	{
#ifdef _WIN32
		closesocket(sock);
#else
		close(sock);
#endif
		connected=YSFALSE;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}


YSBOOL YsSocketClient::IsConnected(void)
{
	return connected;
}


YSRESULT YsSocketClient::Send(YSSIZE_T nBytes,unsigned char dat[],unsigned timeout)
{
	YSBOOL ready;
	ready=YSFALSE;

#ifdef _WIN32
	fd_set set;
	timeval wait;
	wait.tv_sec=timeout/1000;
	wait.tv_usec=timeout%1000;
	FD_ZERO(&set);
	FD_SET(sock,&set);
	if(select(1,NULL,&set,NULL,&wait)>=1)
	{
		ready=YSTRUE;
	}
#else
	struct pollfd pfd;
	pfd.fd=sock;
	pfd.events=POLLOUT;
	pfd.revents=0;
	if(poll(&pfd,1,1)>=1)
	{
		ready=YSTRUE;
	}
#endif

	if(ready==YSTRUE)
	{
		send(sock,(char *)dat,(int)nBytes,0);
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsSocketClient::CheckReceive(void)
{
	YSBOOL ready;
	ready=YSFALSE;

#ifdef _WIN32
	fd_set set;
	timeval wait;
	wait.tv_sec=0;
	wait.tv_usec=0;  // Don't wait. If no message, no need to read.
	FD_ZERO(&set);
	FD_SET(sock,&set);
	if(select(1,&set,NULL,NULL,&wait)>=1)
	{
		ready=YSTRUE;
	}
#else
	struct pollfd pfd;
	pfd.fd=sock;
	pfd.events=POLLIN;
	pfd.revents=0;
	if(poll(&pfd,1,1)>=1)
	{
		ready=YSTRUE;
	}
#endif

	if(ready==YSTRUE)
	{
		int nBytesReceived;
		nBytesReceived=recv(sock,(char *)buffer,nBufferSize,0);
		if(nBytesReceived==0 || nBytesReceived==SOCKET_ERROR)
		{
			ConnectionClosedByServer();
			Disconnect();
			return YSOK;
		}
		else
		{
			Received(nBytesReceived,buffer);
			return YSOK;
		}
	}
	return YSOK;
}


// YSRESULT YsSocketClient::Received(YSSIZE_T nBytes,unsigned char dat[])
// {
// 	printf("Received Message From Server:\n");
// 	printf("  %s\n",dat);
// 
// 	return YSOK;
// }

YSRESULT YsSocketClient::ConnectionClosedByServer(void)
{
	printf("Connection closed by server.\n");
	return YSOK;
}

