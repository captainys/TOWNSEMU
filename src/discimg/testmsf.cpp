#include "discimg.h"



int main(void)
{
	DiscImage::MinSecFrm msf1,msf2;

	msf1.Set(10,11,12);
	msf2.Set(5,6,7);
	auto msf3=msf1+msf2;
	if(msf3.min!=15 || msf3.sec!=17 || msf3.frm!=19)
	{
		return 1;
	}

	msf1.Set(10,10,30);
	msf2.Set(0,0,75);
	msf3=msf1+msf2;
	if(msf3.min!=10 || msf3.sec!=11 || msf3.frm!=30)
	{
		return 1;
	}


	msf1.Set(10,10,30);
	msf2.Set(0,0,75);
	msf3=msf1-msf2;
	if(msf3.min!=10 || msf3.sec!=9 || msf3.frm!=30)
	{
		return 1;
	}


	msf3.Set(0,2,16);
	msf3-=DiscImage::MinSecFrm::TwoSeconds();
	if(msf3.min!=0 || msf3.sec!=0 || msf3.frm!=16)
	{
		return 1;
	}



	msf1.Set(4,8,6);
	msf2.Set(0,10,0);
	msf3=msf1-msf2;
	if(msf3.min!=3 || msf3.sec!=58 || msf3.frm!=6)
	{
		return 1;
	}



	msf1=DiscImage::MinSecFrm::Zero();
	if(msf1.min!=0 || msf1.sec!=0 || msf1.frm!=0)
	{
		return 1;
	}



	msf1.Set(60,30,35);
	msf2.Set(60,30,35);
	msf3.Set(0,10,0);
	if(msf1!=msf2 || msf1==msf3 || msf2==msf3 || msf1<msf3 || msf3>msf2)
	{
		return 1;
	}



	decltype(msf1) msf4;

	auto hsg1=msf1.ToHSG();
	msf4.FromHSG(hsg1);
	if(msf1!=msf4)
	{
		return 1;
	}
	auto hsg2=msf2.ToHSG();
	msf4.FromHSG(hsg2);
	if(msf2!=msf4)
	{
		return 1;
	}
	auto hsg3=msf3.ToHSG();
	msf4.FromHSG(hsg3);
	if(msf3!=msf4)
	{
		return 1;
	}



	return 0;
}
