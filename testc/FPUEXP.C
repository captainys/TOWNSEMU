#include <stdio.h>


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


void Print80Bit(unsigned char buf[10])
{
	for(int i=9; 0<=i; --i)
	{
		printf("%02x",buf[i]);
	}
	printf("\n");
}

void testFDIV(void)
{
	unsigned char three[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x00,0x40};
	unsigned char ten[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xA0,0x02,0x40};
	FLD_m80real(three);  // ST(1)
	FLD_m80real(ten);    // ST
	FDIVP_STi_ST();

	unsigned char buf[10];
	FSTP_m80real(buf);
	Print80Bit(buf);

	double div;
	FLD_m80real(buf);
	FSTP_m64real(&div);
	printf("%lf\n",div);
}

void testFDIVR(void)
{
	unsigned char three[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x00,0x40};
	unsigned char ten[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xA0,0x02,0x40};
	FLD_m80real(three);  // st(1)
	FLD_m80real(ten);    // st
	FDIVRP_STi_ST();

	unsigned char buf[10];
	FSTP_m80real(buf);
	Print80Bit(buf);

	double div;
	FLD_m80real(buf);
	FSTP_m64real(&div);
	printf("%lf\n",div);
}

void testFDIV_STi_ST(void)
{
	unsigned char three[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x00,0x40};
	unsigned char ten[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xA0,0x02,0x40};
	FLD_m80real(three);  // ST(1)
	FLD_m80real(ten);    // ST
	FDIV_STi_ST();

	unsigned char buf[10];
	FSTP_m80real(buf);

	double div;
	FSTP_m64real(&div);
	printf("FDIV STi=3,ST=10 -> %lf\n",div);
}

void testFDIVR_STi_ST(void)
{
	unsigned char three[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x00,0x40};
	unsigned char ten[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xA0,0x02,0x40};
	FLD_m80real(three);  // st(1)
	FLD_m80real(ten);    // st
	FDIVR_STi_ST();

	unsigned char buf[10];
	FSTP_m80real(buf);

	double div;
	FSTP_m64real(&div);
	printf("FDIVR STi=3,ST=10 -> %lf\n",div);
}

void testFDIV_m64real(void)
{
	unsigned char three[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x00,0x40};
	FLD_m80real(three);  // ST

	double d=10.0;
	FDIV_m64real(&d);

	double div;
	FSTP_m64real(&div);
	printf("FDIV ST=3, m64real=10 => %lf\n",div);
}

void testFDIVR_m64real(void)
{
	unsigned char three[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x00,0x40};
	FLD_m80real(three);  // ST

	double d=10.0;
	FDIVR_m64real(&d);

	double div;
	FSTP_m64real(&div);
	printf("FDIVR ST=3, m64real=10 => %lf\n",div);
}

void testFDIV_ST_STi(void)
{
	unsigned char three[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x00,0x40};
	unsigned char ten[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xA0,0x02,0x40};
	FLD_m80real(three);  // ST(1)
	FLD_m80real(ten);    // ST
	FDIV_ST_STi();

	unsigned char buf[10];
	FSTP_m80real(buf);
	Print80Bit(buf);

	double div;
	FLD_m80real(buf);
	FSTP_m64real(&div);
	printf("FDIV ST=10,STi=3 -> %lf\n",div);

	FSTP_m64real(&div);
}

void testFDIVR_ST_STi(void)
{
	unsigned char three[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x00,0x40};
	unsigned char ten[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xA0,0x02,0x40};
	FLD_m80real(three);  // st(1)
	FLD_m80real(ten);    // st
	FDIVR_ST_STi();

	unsigned char buf[10];
	FSTP_m80real(buf);
	Print80Bit(buf);

	double div;
	FLD_m80real(buf);
	FSTP_m64real(&div);
	printf("FDIVR ST=10,STi=3 -> %lf\n",div);
	FSTP_m64real(&div);
}

int main(void)
{
	unsigned char buf[10];
	double x=3.0;
	FLD_m64real(&x);
	FSTP_m80real(buf);

	Print80Bit(buf);

	testFDIV();
	testFDIVR();
	testFDIV_STi_ST();
	testFDIVR_STi_ST();
	testFDIV_ST_STi();
	testFDIVR_ST_STi();
	testFDIV_m64real();
	testFDIVR_m64real();

	return 0;
}
