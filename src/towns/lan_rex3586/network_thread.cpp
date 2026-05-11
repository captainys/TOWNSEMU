#include "network_thread.h"

NetworkThread::NetworkThread()
{
    std::thread t(&NetworkThread::ThreadFunc,this);
    workerThread=std::move(t);
}

NetworkThread::~NetworkThread()
{
    stopThread=true;
    workerThread.join();
}

void NetworkThread::ThreadFunc(void)
{
    StartUp();

    while(true!=stopThread)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

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
    }

    CleanUp();
}

void NetworkThread::StartUp(void)
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
void NetworkThread::CleanUp(void)
{
#ifdef _WIN32
	WSACleanup();
#endif
}

bool NetworkThread::TCPConnectionRequest:: DoConnect(void)
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
