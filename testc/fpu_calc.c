#include <stdio.h>
#include <math.h>



void FLD_m64real(const double *ptr64);
void FLD_m80real(const unsigned char *ptr64);
void FSTP_m64real(void *buf8bytes);
void FSTP_m80real(void *buf10bytes);
void FDIV_ST_STi(void);
void FDIVR_ST_STi(void);
void FDIV_STi_ST(void);
void FDIVR_STi_ST(void);
void FDIVP_STi_ST(void);
void FDIVRP_STi_ST(void);
void UseFSIN(void);
void UseFCOS(void);
void UseFPTAN(void);
void UseFLD_PI(void);
void Use_FLDL2E(void);

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

double Sine(double angle)
{
	FLD_m64real(&angle);
	UseFSIN();
	FSTP_m64real(&angle);
	return angle;
}
double Cosine(double angle)
{
	FLD_m64real(&angle);
	UseFCOS();
	FSTP_m64real(&angle);
	return angle;
}
double Tangent(double angle)
{
	FLD_m64real(&angle);
	UseFPTAN();
	FSTP_m64real(&angle);
	FSTP_m64real(&angle);
	return angle;
}

double GetPI(void)
{
	double d;
	UseFLD_PI();
	FSTP_m64real(&d);
	return d;
}

double GetL2E(void)
{
	double d;
	Use_FLDL2E();
	FSTP_m64real(&d);
	return d;
}

int RunTest(void)
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



	double pi=GetPI();

	// Apparently the following lines use FPREM instead of FSIN for unknown reason.
	// Anyway, it's a good test for FPREM.
	if(0==FCompare(sin(pi/6.0),0.5) &&
	   0==FCompare(sin(pi/3.0),sqrt(3.0)/2.0) &&
	   0==FCompare(sin(pi/2.0),1.0))
	{
		printf("sin works fine.\n");
	}
	else
	{
		e=1;
		printf("Error in sin.\n");
	}


	if(0==FCompare(Sine(pi/6.0),0.5) &&
	   0==FCompare(Sine(pi/3.0),sqrt(3.0)/2.0) &&
	   0==FCompare(Sine(pi/2.0),1.0))
	{
		printf("sin works fine.\n");
	}
	else
	{
		e=1;
		printf("Error in sin.\n");
	}

	if(0==FCompare(Cosine(pi/6.0),sqrt(3.0)/2.0) &&
	   0==FCompare(Cosine(pi/3.0),0.5) &&
	   0==FCompare(Cosine(pi/2.0),0.0))
	{
		printf("cos works fine.\n");
	}
	else
	{
		e=1;
		printf("Error in cos.\n");
	}

	if(0==FCompare(Tangent(pi/4.0),1.0) &&
	   0==FCompare(Tangent(pi/3.0),sqrt(3.0)) &&
	   0==FCompare(Tangent(pi/6.0),1.0/sqrt(3.0)) &&
	   0==FCompare(Tangent(0),0.0))
	{
		printf("tan works fine.\n");
	}
	else
	{
		e=1;
		printf("Error in tan.\n");
	}


	double l2e=GetL2E();
	double E=pow(2.0,l2e);
	if(0==FCompare(E,_E))
	{
		printf("pow and GetL2E works fine.\n");
	}
	else
	{
		printf("Error in pow or GetL2E.\n");
	}

	if(0==FCompare(sqrt(10.0),pow(10.0,0.5)))
	{
		printf("pow and sqrt works fine.\n");
	}
	else
	{
		printf("Error in pow or sqrt.\n");
	}


	a=sqrt(3.0);
	b=1.0;
	a=atan2(a,b);


	return e;
}

int RunTestFloatAndInt(void)
{
	int e=0;

	float a,b,c;
	int i;
	i=10;
	a=i;
	a=a+i;
	if(0==FCompare((double)a,20.0))
	{
		printf("Adding integer OK.\n");
	}
	else
	{
		e=1;
		printf("Error in adding integer.\n");
	}

	return e;
}

int main(void)
{
	RunTest();
	int e=RunTest()+RunTestFloatAndInt();
	return e;
}
