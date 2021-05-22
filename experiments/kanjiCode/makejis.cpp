#include <fstream>



int main(void)
{
	std::ofstream ofp("JIS.txt",std::ios::binary);
	for(unsigned int highByte=0x21; highByte<=0x7E; ++highByte)
	{
		for(unsigned int lowByte=0x21; lowByte<=0x7E; ++lowByte)
		{
			auto JISCode=(highByte<<8)|lowByte;
			char buf[16];
			sprintf(buf,"%04x",JISCode);
			buf[4]=0x1B;
			buf[5]=0x24;
			buf[6]=0x42;
			buf[7]=(char)highByte;
			buf[8]=(char)lowByte;
			buf[9]=0x1B;
			buf[10]=0x28;
			buf[11]=0x42;
			buf[12]='\n';
			buf[13]=0;
			ofp.write(buf,13);
		}
	}
	return 0;
}
