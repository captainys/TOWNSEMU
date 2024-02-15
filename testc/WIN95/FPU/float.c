#define TOWNSIO_VM_HOST_IF_CMD_STATUS        0x2386
#define TOWNSIO_VM_HOST_IF_DATA              0x2387

#define TOWNS_VMIF_CMD_NOP                   0x00
#define TOWNS_VMIF_CMD_CAPTURE_CRTC          0x01
#define TOWNS_VMIF_CMD_PAUSE                 0x02
#define TOWNS_VMIF_CMD_EXIT_VM               0x03

void NotifyTestSuccess(void)
{
	_asm {
		PUSH	AX
		PUSH	DX

		MOV		AL,00H
		MOV		DX,TOWNSIO_VM_HOST_IF_DATA
		OUT		DX,AL

		MOV		AL,TOWNS_VMIF_CMD_EXIT_VM
		MOV		DX,TOWNSIO_VM_HOST_IF_CMD_STATUS
		OUT		DX,AL

		POP		DX
		POP		AX
	}
}



#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

const double tolerance=0.000001;
const double E=2.718281828459045;
const double PI=3.14159265358979323;

int Compare(double a,double b)
{
	if(tolerance<fabs(a-b))
	{
		return 1;
	}
	return 0;
}

void TestSqrt(void)
{
	const char *FUNC="TestSqrt";
	int i;
	double a,b,c;
	for(i=0; i<100; ++i)
	{
		a=(double)i;
		b=(double)(i*i);
		c=sqrt(b);
		if(0!=Compare(a,c))
		{
			printf("%s\n",FUNC);
			printf("%lf did not match %lf\n",c,a);
			exit(1);
		}
	}
}

void TestPow(void)
{
	const char *FUNC="TestPow";
	short i;
	double a,b,c;
	for(i=0; i<100; ++i)
	{
		a=(double)i;
		b=(double)(i*i);
		c=pow(b,0.5);
		if(0!=Compare(a,c))
		{
			printf("%s pow(b,0.5)\n",FUNC);
			printf("%lf did not match %lf\n",c,a);
			exit(1);
		}

		a=(double)i;
		b=a*a*a;
		c=pow(a,3.0);
		if(0!=Compare(b,c))
		{
			printf("%s pow(a,3.0)\n",FUNC);
			printf("%lf did not match %lf\n",c,b);
			exit(1);
		}

		c=pow(b,1.0/3.0);
		if(0!=Compare(a,c))
		{
			printf("%s pow(b,1.0/3.0)\n",FUNC);
			printf("%lf did not match %lf\n",c,a);
			exit(1);
		}
	}
}

void TestFloorAndCeil(void)
{
	const char *FUNC="TestFloorAndCeil";
	int i;
	for(i=0; i<100; ++i)
	{
		int I;
		short S;
		double a=(double)i,b,c;

		b=a+1.0;
		a+=0.5;

		I=(int)floor(a);
		S=(int)floor(a);
		if(I!=i || S!=i)
		{
			printf("%s (int)floor(a)\n",FUNC);
			printf("%d or %d did not match %d\n",I,S,i);
			exit(1);
		}

		c=ceil(a);
		if(0!=Compare(b,c))
		{
			printf("%s ceil(a)\n",FUNC);
			printf("%lf did not match %lf\n",b,c);
			exit(1);
		}
	}
}

// Damn it!  I want to use template.  But, I need to compile it with High-C.
void TestDoubleOp(void)
{
	const char *FUNC="TestDoubleOp";
	static int i;
	double sum;
	sum=0.0;
	for(i=0; i<=100; ++i)
	{
		double d=(double)i;
		sum+=d;
	}
	if(0!=Compare(sum,5050.0))
	{
		printf("%s sum+=i\n",FUNC);
		printf("%f did not match 5050.\n",sum);
		exit(1);
	}
	for(i=0; i<=100; ++i)
	{
		double d=(double)i;
		sum-=d;
	}
	if(0!=Compare(sum,0.0))
	{
		printf("%s sum+=i\n",FUNC);
		printf("%f did not match 0.\n",sum);
		exit(1);
	}

	sum=1.0;
	for(i=1; i<=16; ++i)
	{
		double d=(double)i;
		sum*=d;
	}
	if(0!=Compare(sum,20922789888000.0))
	{
		printf("%s sum*=i\n",FUNC);
		printf("%lf did not match 20922789888000.0.\n",sum);
		exit(1);
	}
	for(i=1; i<=16; ++i)
	{
		double d=(double)i;
		sum/=d;
	}
	if(0!=Compare(sum,1.0))
	{
		printf("%s sum/=i\n",FUNC);
		printf("%lf did not match 1.0.\n",sum);
		exit(1);
	}
}

// Damn it!  I want to use template.  But, I need to compile it with High-C.
void TestFloatOp(void)
{
	const char *FUNC="TestFloatOp";
	static int i;
	static short s;
	float sum;
	sum=0.0;
	for(i=0; i<=100; ++i)
	{
		sum+=i; // Hope the compiler emits FIADD m32int
	}
	if(0!=Compare(sum,5050.0))
	{
		printf("%s sum+=i\n",FUNC);
		printf("%f did not match 5050.\n",sum);
		exit(1);
	}
	for(i=0; i<=100; ++i)
	{
		sum-=i; // Hope the compiler emits FISUB m32int
	}
	if(0!=Compare(sum,0.0))
	{
		printf("%s sum+=i\n",FUNC);
		printf("%f did not match 0.\n",sum);
		exit(1);
	}

	sum=0.0;
	for(s=0; s<=100; ++s)
	{
		sum+=s; // Hope the compiler emits FIADD m32int
	}
	if(0!=Compare(sum,5050.0))
	{
		printf("%s sum+=s\n",FUNC);
		printf("%f did not match 5050.\n",sum);
		exit(1);
	}
	for(s=0; s<=100; ++s)
	{
		sum-=s; // Hope the compiler emits FISUB m32int
	}
	if(0!=Compare(sum,0.0))
	{
		printf("%s sum+=s\n",FUNC);
		printf("%f did not match 0.\n",sum);
		exit(1);
	}


	sum=1.0;
	for(i=1; i<=12; ++i)
	{
		sum*=i; // Hope the compiler emits FIMUL m32int
	}
	if(0!=Compare(sum,479001600.0))
	{
		printf("%s sum*=i\n",FUNC);
		printf("%f did not match 479001600.0.\n",sum);
		exit(1);
	}
	for(i=1; i<=12; ++i)
	{
		sum/=i; // Hope the compiler emits FIDIV m32int
	}
	if(0!=Compare(sum,1.0))
	{
		printf("%s sum/=i\n",FUNC);
		printf("%f did not match 1.0.\n",sum);
		exit(1);
	}


	sum=1.0;
	for(s=1; s<=12; ++s)
	{
		sum*=s; // Hope the compiler emits FIMUL m32int
	}
	if(0!=Compare(sum,479001600.0))
	{
		printf("%s sum*=s\n",FUNC);
		printf("%f did not match 479001600.0.\n",sum);
		exit(1);
	}
	for(s=1; s<=12; ++s)
	{
		sum/=s; // Hope the compiler emits FIDIV m32int
	}
	if(0!=Compare(sum,1.0))
	{
		printf("%s sum/=s\n",FUNC);
		printf("%f did not match 1.0.\n",sum);
		exit(1);
	}
}

// Damn it!  I want to use template.  But, I need to compile it with High-C.
void TestIntOp(void)
{
	const char *FUNC="TestIntOp";
	static int i;
	static short s;
	double sum;
	sum=0.0;
	for(i=0; i<=100; ++i)
	{
		sum+=i; // Hope the compiler emits FIADD m32int
	}
	if(0!=Compare(sum,5050.0))
	{
		printf("%s sum+=i\n",FUNC);
		printf("%lf did not match 5050.\n",sum);
		exit(1);
	}
	for(i=0; i<=100; ++i)
	{
		sum-=i; // Hope the compiler emits FISUB m32int
	}
	if(0!=Compare(sum,0.0))
	{
		printf("%s sum+=i\n",FUNC);
		printf("%lf did not match 0.\n",sum);
		exit(1);
	}

	sum=0.0;
	for(s=0; s<=100; ++s)
	{
		sum+=s; // Hope the compiler emits FIADD m32int
	}
	if(0!=Compare(sum,5050.0))
	{
		printf("%s sum+=s\n",FUNC);
		printf("%lf did not match 5050.\n",sum);
		exit(1);
	}
	for(s=0; s<=100; ++s)
	{
		sum-=s; // Hope the compiler emits FISUB m32int
	}
	if(0!=Compare(sum,0.0))
	{
		printf("%s sum+=s\n",FUNC);
		printf("%lf did not match 0.\n",sum);
		exit(1);
	}


	sum=1.0;
	for(i=1; i<=16; ++i)
	{
		sum*=i; // Hope the compiler emits FIMUL m32int
	}
	if(0!=Compare(sum,20922789888000.0))
	{
		printf("%s sum*=i\n",FUNC);
		printf("%lf did not match 20922789888000.0.\n",sum);
		exit(1);
	}
	for(i=1; i<=16; ++i)
	{
		sum/=i; // Hope the compiler emits FIDIV m32int
	}
	if(0!=Compare(sum,1.0))
	{
		printf("%s sum/=i\n",FUNC);
		printf("%lf did not match 1.0.\n",sum);
		exit(1);
	}


	sum=1.0;
	for(s=1; s<=16; ++s)
	{
		sum*=s; // Hope the compiler emits FIMUL m32int
	}
	if(0!=Compare(sum,20922789888000.0))
	{
		printf("%s sum*=s\n",FUNC);
		printf("%lf did not match 20922789888000.0.\n",sum);
		exit(1);
	}
	for(s=1; s<=16; ++s)
	{
		sum/=s; // Hope the compiler emits FIDIV m32int
	}
	if(0!=Compare(sum,1.0))
	{
		printf("%s sum/=s\n",FUNC);
		printf("%lf did not match 1.0.\n",sum);
		exit(1);
	}
}

void TestLog(void)
{
	const char *FUNC="TestLog";
	int i;
	for(i=0; i<20; ++i)
	{
		double a,b;
		a=pow(10,(double)i);
		b=log10(a);
		if(0!=Compare((double)i,b))
		{
			printf("%s pow(10,(double)i) then log10(a)\n",FUNC);
			printf("%lf did not match %lf.\n",(double)i,b);
			exit(1);
		}
	}

	for(i=0; i<20; ++i)
	{
		double a,b;
		a=pow(E,(double)i);
		b=log(a);
		if(0!=Compare((double)i,b))
		{
			printf("%s pow(E,(double)i) then log(a)\n",FUNC);
			printf("%lf did not match %lf.\n",(double)i,b);
			exit(1);
		}
	}
}

void TestTrigonometric(void)
{
	const char *FUNC="TestTrigonometric";
	int i;
	for(i=-179; i<180; ++i)
	{
		double a=(double)i;
		double rd=a*PI/180.0;
		double s=sin(rd);
		double c=cos(rd);
		double rev=atan2(s,c);
		double L;
		if(0!=Compare(rev,rd))
		{
			printf("%s atan2\n",FUNC);
			printf("%lf did not match %lf.\n",rev,rd);
			exit(1);
		}

		L=s*s+c*c;
		if(0!=Compare(L,1.0))
		{
			printf("%s sin,cos,atan2\n",FUNC);
			printf("sin*sin+cos*cos is not 1.0 (s,c)=(%lf,%lf).\n",s,c);
			exit(1);
		}

		if(i!=90 && i!=-90)
		{
			double t=tan(rd);
			if(0!=Compare(t,s/c))
			{
				printf("%s sin,cos,atan2\n",FUNC);
				printf("sin/cos does not match tan (s,c,t)=(%lf,%lf,%lf).\n",s,c,t);
				exit(1);
			}
		}
	}

	for(i=-99; i<=99; ++i)
	{
		double v=(double)i/100.0;
		double a=asin(v);
		double s=sin(a);
		double c,t;

		if(0!=Compare(v,s))
		{
			printf("%s asin\n",FUNC);
			printf("%lf does not match %lf.\n",v,s);
			exit(1);
		}

		a=acos(v);
		c=cos(a);
		if(0!=Compare(v,c))
		{
			printf("%s acos\n",FUNC);
			printf("%lf does not match %lf.\n",v,c);
			exit(1);
		}
	}

	for(i=-1000; i<=1000; ++i)
	{
		double v=(double)i/100.0;
		double a=atan(v);
		double t=tan(a);

		if(0!=Compare(v,t))
		{
			printf("%s atan\n",FUNC);
			printf("%lf does not match %lf.\n",v,t);
			exit(1);
		}
	}
}

int main(void)
{
	TestSqrt();
	TestPow();
	TestFloorAndCeil();
	TestDoubleOp();
	TestFloatOp();
	TestIntOp();
	TestLog();
	TestTrigonometric();
	printf("Test Done.\n");
	NotifyTestSuccess();
	return 0;
}
