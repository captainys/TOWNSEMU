#include <stdio.h>

int main(void)
{
	FILE *ifp=fopen("AUTOEXEC.BAT","r");
	if(NULL!=ifp)
	{
		char str[256];
		while(NULL!=fgets(str,255,ifp))
		{
			printf("%s",str);
		}
		fclose(ifp);
	}
	else
	{
		printf("Cannot open AUTOEXEC.BAT!\n");
	}
	return 0;
}
