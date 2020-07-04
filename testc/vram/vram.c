#include <stdio.h>



void CLEAR_VRAM(void);
void WRITE_TO_VRAM1(unsigned int offset);
unsigned int SEARCH_VRAM3(void);

#define VRAM_SIZE 0x80000

int main(void)
{
	FILE *fp;
	unsigned int offset;

	fp=fopen("result.txt","w");
	for(offset=0; offset<0x200; ++offset)
	{
		unsigned int mapped;
		CLEAR_VRAM();
		WRITE_TO_VRAM1(offset);
		mapped=SEARCH_VRAM3();
		fprintf(fp,"0x%06x,0x%06x,\n",offset,mapped);
	}
	for(offset=0x26000; offset<0x26200; ++offset)
	{
		unsigned int mapped;
		CLEAR_VRAM();
		WRITE_TO_VRAM1(offset);
		mapped=SEARCH_VRAM3();
		fprintf(fp,"0x%06x,0x%06x,\n",offset,mapped);
	}
	for(offset=0x40000; offset<0x40200; ++offset)
	{
		unsigned int mapped;
		CLEAR_VRAM();
		WRITE_TO_VRAM1(offset);
		mapped=SEARCH_VRAM3();
		fprintf(fp,"0x%06x,0x%06x,\n",offset,mapped);
	}
	for(offset=0x66000; offset<0x66200; ++offset)
	{
		unsigned int mapped;
		CLEAR_VRAM();
		WRITE_TO_VRAM1(offset);
		mapped=SEARCH_VRAM3();
		fprintf(fp,"0x%06x,0x%06x,\n",offset,mapped);
	}
	for(offset=0; offset<VRAM_SIZE; offset+=0x10)
	{
		unsigned int mapped;
		CLEAR_VRAM();
		WRITE_TO_VRAM1(offset);
		mapped=SEARCH_VRAM3();
		fprintf(fp,"0x%06x,0x%06x,\n",offset,mapped);
	}
	fclose(fp);

	return 0;
}
