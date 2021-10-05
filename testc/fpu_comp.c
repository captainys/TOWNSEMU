#include <stdio.h>

int main(void)
{
	int e=0;
	double one=1.0,two=2.0,oneAgain=1.0;
	if(one<two)
	{
		printf("One is less than two.  Correct!\n");
	}
	else
	{
		printf("Error!  My FPU is thinking One is not less than two.\n");
		e=1;
	}

	if(one==oneAgain)
	{
		printf("One equals to one.  Correct!\n");
	}
	else
	{
		printf("Error!  My FPU is thinking One is not equal to one.\n");
		e=1;
	}


	float onef=1.0f,twof=2.0f,oneAgainf=1.0f;
	if(onef<twof)
	{
		printf("One is less than two.  Correct!\n");
	}
	else
	{
		printf("Error!  My FPU is thinking One is not less than two.\n");
		e=1;
	}

	if(onef==oneAgainf)
	{
		printf("One equals to one.  Correct!\n");
	}
	else
	{
		printf("Error!  My FPU is thinking One is not equal to one.\n");
		e=1;
	}

	return e;
}
