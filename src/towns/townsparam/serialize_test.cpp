#include <stdio.h>
#include <vector>
#include <string>
#include "townsprofile.h"

int main(int ac,char *av[])
{
	TownsProfile profile;
	auto data=profile.Serialize();
	if(true!=profile.Deserialize(data))
	{
		fprintf(stderr,"Deserialization failed.\n");
		return 1;
	}
	printf("Pass.\n");
	return 0;
}
