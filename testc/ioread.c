#include <stdio.h>
#include <conio.h>
#include <time.h>

unsigned int xtoi(const char *str)
{
	unsigned int i=0;
	while(0!=*str)
	{
		if('0'<=*str && *str<='9')
		{
			i<<=4;
			i|=(*str-'0');
		}
		else if('a'<=*str && *str<='f')
		{
			i<<=4;
			i+=(*str+10-'a');
		}
		else if('A'<=*str && *str<='F')
		{
			i<<=4;
			i+=(*str+10-'A');
		}
		else
		{
			break;
		}
		++str;
	}
	return i;
}

void Wait(clock_t count)
{
	clock_t clk=clock();
	while((clock()-clk)<count)
	{
	}
}

int main(int ac,char *av[])
{
	int i;
	char *portstr;
	unsigned int port;

	if(ac<2)
	{
		printf("Usage:\n");
		printf("  ioread io_port_in_hex\n");
		return 0;
	}

	portstr=av[1];

	if('0'==portstr[0] && 'x'==portstr[1])
	{
		portstr+=2;
	}

	port=xtoi(portstr);
	printf("Port 0x%x\n",port);

	for(i=0; i<10; ++i)
	{
		unsigned char data=_inp(port);
		printf("Read %02x\n",data);

		Wait(CLOCKS_PER_SEC/2);
	}

	return 0;
}
