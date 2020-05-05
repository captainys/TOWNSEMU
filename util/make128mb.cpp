#include <stdio.h>

unsigned char zero[1024*1024];

int main(void)
{
	for(auto &z : zero)
	{
		z=0;
	}

	FILE *fp=fopen("binary.bin","wb");
	for(int i=0; i<128; ++i)
	{
		fwrite(zero,1,sizeof(zero),fp);
	}
	fclose(fp);

	return 0;
}
