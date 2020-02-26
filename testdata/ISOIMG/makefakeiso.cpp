#include <vector>
#include <fstream>



int main(void)
{
	std::vector <unsigned char> iso;
	for(int i=0; i<8192; ++i)
	{
		iso.push_back((unsigned char)(i/2048));
	}

	std::ofstream ofp;
	ofp.open("iso4sect.iso",std::ios::binary);
	ofp.write((char *)iso.data(),iso.size());
	return 0;
}


