#include <stdio.h>


void FLD_m64real(const double *ptr64);
void FLD_m80real(const unsigned char *ptr64);
void FSTP_m64real(void *buf8bytes);
void FSTP_m80real(void *buf10bytes);
void FDIV(void);


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
	FLD_m80real(three);
	FLD_m80real(ten);
	FDIV();

	unsigned char buf[10];
	FSTP_m80real(buf);
	Print80Bit(buf);

	FLD_m80real(three);
	FLD_m80real(ten);
	FDIV();

	double div;
	FSTP_m64real(&div);
	printf("%lf\n",div);
}

int main(void)
{
	unsigned char buf[10];
	double x=3.0;
	FLD_m64real(&x);
	FSTP_m80real(buf);

	Print80Bit(buf);

	testFDIV();

	return 0;
}
