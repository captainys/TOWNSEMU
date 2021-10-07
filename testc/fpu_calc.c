#include <stdio.h>
#include <math.h>




int FCompare(double a,double b)
{
	if(fabs(a-b)<0.000001)
	{
		return 0;
	}
	if(a-b<0)
	{
		return -1;
	}
	return 1;
}

int main(void)
{
	int e=0;

	double a=10.001,b=10.001,c;
	int i;

	i=(int)(a+b);
	if(i==20)
	{
		printf("Correct Addition.\n");
	}
	else
	{
		e=1;
		printf("Error in addition.\n");
	}


	b=20.001;
	i=(int)(a+b);
	if(i==30)
	{
		printf("Correct Addition.\n");
	}
	else
	{
		e=1;
		printf("Error in addition.\n");
	}



	a=10.01;
	b=5.0;
	c=a-b;
	i=(int)c;
	if(i==5)
	{
		printf("Correct Subtraction.\n");
	}
	else
	{
		e=1;
		printf("Error in subtraction.\n");
	}

	a=-10.01;
	b=-5.0;
	c=a-b;
	i=(int)c;
	if(i==-5)
	{
		printf("Correct Subtraction.\n");
	}
	else
	{
		e=1;
		printf("Error in subtraction.\n");
	}



	a=100.001;
	b=10.0;
	i=(int)(a/b);
	if(10==i)
	{
		printf("Correct Division.\n");
	}
	else
	{
		e=1;
		printf("Error in division.\n");
	}


	a=-100.001;
	b=5.0;
	i=(int)(a/b);
	if(-20==i)
	{
		printf("Correct Division.\n");
	}
	else
	{
		e=1;
		printf("Error in division.\n");
	}


	a=8.0001;
	b=16.0001;
	c=a*b;
	i=(int)c;
	if(128==i)
	{
		printf("Correct Multiplication.\n");
	}
	else
	{
		e=1;
		printf("Error in Multiplication.\n");
	}

	a=2.0001;
	b=-128.0001;
	c=a*b;
	i=(int)c;
	if(-256==i)
	{
		printf("Correct Multiplication.\n");
	}
	else
	{
		e=1;
		printf("Error in Multiplication.\n");
	}


	a=fabs(-5.01);
	i=(int)a;
	if(5==i)
	{
		printf("fabs works fine.\n");
	}
	else
	{
		e=1;
		printf("Error in fabs.\n");
	}


	a=sqrt(256.01);
	i=(int)a;
	if(16==i)
	{
		printf("sqrt works fine.\n");
	}
	else
	{
		e=1;
		printf("Error in sqrt.\n");
	}


	if(0==FCompare(sin(_PI/6.0),0.5) &&
	   0==FCompare(sin(_PI/3.0),sqrt(3.0)/2.0) &&
	   0==FCompare(sin(_PI/2.0),1.0))
	{
		printf("sin works fine.\n");
	}
	else
	{
		e=1;
		printf("Error in sin.\n");
	}



	return e;
}
