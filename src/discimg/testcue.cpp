#include <stdio.h>

#include "discimg.h"



int main(int argc,char *argv[])
{
	if(1==argc)
	{
		return 1;
	}

	DiscImage img;
	auto errCode=img.Open(argv[1]);
	printf("%s\n",img.ErrorCodeToText(errCode));
	if(0!=errCode)
	{
		return 1;
	}

	return 0;
}
