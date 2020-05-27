/* ////////////////////////////////////////////////////////////

File Name: yspng.cpp
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */

#include <stdio.h>

#include "yspng.h"



unsigned int YsGenericPngDecoder::verboseMode=YSFALSE;



// Updates
//   2005/03/02
//     Started
//   2005/03/03
//     Support:
//       8bit Grayscale, True color, 4bit Indexed color, 8bit Indexed color
//   2005/03/04
//     Added support for true color with alpha and grayscale with alpha
//   2005/03/14
//     Support:
//       Interlaced 8bit True color
//   2005/03/15
//     Support:
//       Interlaced/Non-Interlaced 16bit True color
//       Interlaced 8bit Grayscale
//       Interlaced 8bit Indexed color
//       Interlaced 8bit Grayscale with alpha
//       Interlaced 8bit Truecolor with alpha
//     Cleaned up a little bit.
//   2005/04/08
//     Support:
//       Non-Interlace 1 bit Grayscale
//   2010/09/18
//     Fixed transparency check in interlaced 16-bit true-color mode.
//     Based on a bug-report from a viewer.  Thanks for the good catch!
//   2011/12/28 
//     Fixed transparent color reading based on the user-report.  Thanks for the good catch!
//   2012/01/21
//     Added int Decode(FILE *fp);
//   2014/03/23
//     Bug fix: Sliding-window buffer was not populated while processing uncompressed block of data.
//   2014/06/04
//     1bit Indexed Color was already supported.  I was forgetting to add in the list below.
//   2014/12/21
//     Small improvement in the de-compression efficiency.

/* Supported color and depth

Non-Interlaced
  1bit Grayscale
  1bit Indexed Color
  4bit Indexed Color
  8bit Grayscale
  8bit Grayscale with Alpha
  8bit Indexed Color
  8bit True Color (24bit per pixel)
  8bit True Color with Alpha (32bit per pixel)
  16bit True Color (48bit per pixel)

Interlaced
  8bit Grayscale
  8bit Grayscale with Alpha
  8bit Indexed Color
  8bit True color (24bit per pixel)
  8bit True Color with Alpha (32bit per pixel)
  16bit True Color (48bit per pixel)

*/



// Memo
//   PNG Data Format http://www.w3.org/TR/PNG
//   zLib Data Format http://www.gzip.org/zlib/rfc-deflate.html

#define MakeDword(a,b,c,d) ((a)*0x1000000+(b)*0x10000+(c)*0x100+(d))

#define IHDR MakeDword('I','H','D','R')
#define IDAT MakeDword('I','D','A','T')
#define PLTE MakeDword('P','L','T','E')
#define IEND MakeDword('I','E','N','D')
#define pHYs MakeDword('p','H','y','s')
#define tRNS MakeDword('t','R','N','S')
#define gAMA MakeDword('g','A','M','A')

static inline unsigned int PngGetUnsignedInt(const unsigned char dat[4])
{
	return (unsigned)dat[3]+(unsigned)dat[2]*0x100+(unsigned)dat[1]*0x10000+(unsigned)dat[0]*0x1000000;
}

////////////////////////////////////////////////////////////

void YsPngHeader::Decode(unsigned char dat[])
{
	width=PngGetUnsignedInt(dat);
	height=PngGetUnsignedInt(dat+4);
	bitDepth=dat[8];
	colorType=dat[9];
	compressionMethod=dat[10];
	filterMethod=dat[11];
	interlaceMethod=dat[12];

	if(YsGenericPngDecoder::verboseMode==YSTRUE)
	{
		printf("Width=%d Height=%d\n",width,height);
		printf("bitDepth=%d\n",bitDepth);
		printf("colorType=%d\n",colorType);
		printf("compressionMethod=%d\n",compressionMethod);
		printf("filterMethod=%d\n",filterMethod);
		printf("interlaceMethod=%d\n",interlaceMethod);
	}
}

YsPngPalette::YsPngPalette()
{
	nEntry=0;
	entry=NULL;
}

YsPngPalette::~YsPngPalette()
{
	if(entry!=NULL)
	{
		delete [] entry;
	}
}

int YsPngPalette::Decode(unsigned length,unsigned char dat[])
{
	if(length%3!=0)
	{
		return YSERR;
	}

	if(entry!=NULL)
	{
		delete [] entry;
		nEntry=0;
		entry=NULL;
	}

	if(length>0)
	{
		entry=new unsigned char [length];
		if(entry!=NULL)
		{
			unsigned int i;
			nEntry=length/3;

			if(YsGenericPngDecoder::verboseMode==YSTRUE)
			{
				printf("%d palette entries\n",nEntry);
			}

			for(i=0; i<length; i++)
			{
				entry[i]=dat[i];
			}
		}
	}

	return YSOK;
}

////////////////////////////////////////////////////////////

int YsPngTransparency::Decode(unsigned int length,unsigned char dat[],unsigned int colorType)
{
	unsigned int i;
	switch(colorType)
	{
	case 0:
		if(length>=2)
		{
			col[0]=(unsigned int)dat[0]*256+(unsigned int)dat[1];
			return YSOK;
		}
		break;
	case 2:
		if(length>=6)
		{
			col[0]=(unsigned int)dat[0]*256+(unsigned int)dat[1]; // 2011/12/28 Bug fix based on the user-report.  Thanks!
			col[1]=(unsigned int)dat[2]*256+(unsigned int)dat[3];
			col[2]=(unsigned int)dat[4]*256+(unsigned int)dat[5];
			return YSOK;
		}
		break;
	case 3:
		for(i=0; i<3 && i<length; i++)
		{
			col[i]=dat[i];
		}
		return YSOK;
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

YsPngBinaryFileStream::YsPngBinaryFileStream(FILE *fp)
{
	this->fp=fp;
}

size_t YsPngBinaryFileStream::GetSize(void) const
{
	size_t curPos=ftell(fp);
    fseek(fp,0,2 /* SEEK_END */);
    size_t fsize=ftell(fp);
    fseek(fp,(long)curPos,0 /* SEEK_SET */);
    return fsize;
}

size_t YsPngBinaryFileStream::Read(unsigned char buf[],size_t readSize)
{
	return fread(buf,1,readSize,fp);
}

YsPngBinaryMemoryStream::YsPngBinaryMemoryStream(size_t dataSize,const unsigned char binaryData[])
{
	this->offset=0;
	this->dataSize=dataSize;
	this->binaryData=binaryData;
}

size_t YsPngBinaryMemoryStream::GetSize(void) const
{
	return dataSize;
}

size_t YsPngBinaryMemoryStream::Read(unsigned char buf[],size_t readSize)
{
	size_t byteCopied=0;
	for(size_t idx=0; idx<readSize && offset<dataSize; ++idx)
	{
		buf[idx]=binaryData[offset];
		++offset;
		++byteCopied;
	}
	return byteCopied;
}



////////////////////////////////////////////////////////////

YsGenericPngDecoder::YsGenericPngDecoder()
{
	Initialize();
}

void YsGenericPngDecoder::Initialize(void)
{
	gamma=gamma_default;
	trns.col[0]=0x7fffffff;
	trns.col[1]=0x7fffffff;
	trns.col[2]=0x7fffffff;
}

int YsGenericPngDecoder::CheckSignature(YsPngGenericBinaryStream &binStream)
{
	unsigned char buf[8];
	binStream.Read(buf,8);
	if(buf[0]==0x89 && buf[1]==0x50 && buf[2]==0x4e && buf[3]==0x47 && 
	   buf[4]==0x0d && buf[5]==0x0a && buf[6]==0x1a && buf[7]==0x0a)
	{
		return YSOK;
	}
	return YSERR;
}

int YsGenericPngDecoder::ReadChunk(unsigned &length,unsigned char *&buf,unsigned &chunkType,unsigned &crc,YsPngGenericBinaryStream &binStream)
{
	unsigned char dwBuf[4];

	if(binStream.Read(dwBuf,4)<4)
	{
		return YSERR;
	}
	length=PngGetUnsignedInt(dwBuf);

	if(binStream.Read(dwBuf,4)<4)
	{
		return YSERR;
	}
	chunkType=PngGetUnsignedInt(dwBuf);

	if(YsGenericPngDecoder::verboseMode==YSTRUE)
	{
		printf("Chunk name=%c%c%c%c\n",dwBuf[0],dwBuf[1],dwBuf[2],dwBuf[3]);
	}

	if(length>0)
	{
		buf=new unsigned char [length];
		if(binStream.Read(buf,length)<length)
		{
			return YSERR;
		}
	}
	else
	{
		buf=NULL;
	}

	if(binStream.Read(dwBuf,4)<4)
	{
		return YSERR;
	}
	crc=PngGetUnsignedInt(dwBuf);

	return YSOK;
}

////////////////////////////////////////////////////////////

int YsPngHuffmanTree::leakTracker=0;

YsPngHuffmanTree::YsPngHuffmanTree()
{
	zeroOne[0]=NULL;
	zeroOne[1]=NULL;
	dat=0x7fffffff;
	weight=0;
	depth=1;
	leakTracker++;
}

YsPngHuffmanTree::~YsPngHuffmanTree()
{
	leakTracker--;
}

void YsPngHuffmanTree::DeleteHuffmanTree(YsPngHuffmanTree *node)
{
	if(node!=NULL)
	{
		DeleteHuffmanTree(node->Zero());
		DeleteHuffmanTree(node->One());
		delete node;
	}
}

////////////////////////////////////////////////////////////

void YsPngUncompressor::MakeFixedHuffmanCode(unsigned hLength[288],unsigned hCode[288])
{
	unsigned i;
	for(i=0; i<=143; i++)
	{
		hLength[i]=8;
		hCode[i]=0x30+i;
	}
	for(i=144; i<=255; i++)
	{
		hLength[i]=9;
		hCode[i]=0x190+(i-144);
	}
	for(i=256; i<=279; i++)
	{
		hLength[i]=7;
		hCode[i]=i-256;
	}
	for(i=280; i<=287; i++)
	{
		hLength[i]=8;
		hCode[i]=0xc0+(i-280);
	}
}

void YsPngUncompressor::MakeDynamicHuffmanCode(unsigned hLength[],unsigned hCode[],unsigned nLng,unsigned lng[])
{
	unsigned i,maxLng,code,*bl_count,*next_code,bits,n;

	for(i=0; i<nLng; i++)
	{
		hLength[i]=lng[i];
		hCode[i]=0;
	}

	maxLng=0;
	for(i=0; i<nLng; i++)
	{
		if(maxLng<lng[i])
		{
			maxLng=lng[i];
		}
	}

	bl_count=new unsigned [maxLng+1];
	next_code=new unsigned [maxLng+1];
	for(i=0; i<maxLng+1; i++)
	{
		bl_count[i]=0;
		next_code[i]=0;
	}
	for(i=0; i<nLng; i++)
	{
		bl_count[lng[i]]++;
	}

	// for(i=0; i<maxLng+1; i++)
	// {
	// 	printf("bl_count[%d]=%d\n",i,bl_count[i]);
	// }

	// See RFC1951 Specification
	code=0;
	bl_count[0]=0;
	for(bits=1; bits<=maxLng; bits++)
	{
		code=(code+bl_count[bits-1])<<1;
		next_code[bits]=code;
	}

	for(n=0; n<nLng; n++)
	{
		unsigned len;
		len=lng[n];
		if(len>0)
		{
			hCode[n]=next_code[len]++;
		}
	}

	if(bl_count!=NULL)
	{
		delete [] bl_count;
	}
	if(next_code!=NULL)
	{
		delete [] next_code;
	}
}

int YsPngUncompressor::DecodeDynamicHuffmanCode
	   (unsigned int &hLit,unsigned int &hDist,unsigned int &hCLen,
	    unsigned int *&hLengthLiteral,unsigned int *&hCodeLiteral,
	    unsigned int *&hLengthDist,unsigned int *&hCodeDist,
	    unsigned int hLengthBuf[322],unsigned int hCodeBuf[322],
	    const unsigned char dat[],unsigned int &bytePtr,unsigned int &bitPtr)
{
	unsigned int i;
	hLit=0;
	hDist=0;
	hCLen=0;

	hLit=GetNextMultiBit(dat,bytePtr,bitPtr,5);
	hDist=GetNextMultiBit(dat,bytePtr,bitPtr,5);
	hCLen=GetNextMultiBit(dat,bytePtr,bitPtr,4);

	if(YsGenericPngDecoder::verboseMode==YSTRUE)
	{
		printf("hLit=%d hDist=%d hCLen=%d\n",hLit,hDist,hCLen);
	}

	const unsigned int codeLengthLen=19;
	unsigned codeLengthOrder[codeLengthLen]=
	{
		16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15
	};
	unsigned codeLengthCode[codeLengthLen];
	for(i=0; i<codeLengthLen; i++)
	{
		codeLengthCode[i]=0;
	}
	for(i=0; i<hCLen+4; i++)
	{
		codeLengthCode[codeLengthOrder[i]]=GetNextMultiBit(dat,bytePtr,bitPtr,3);
		// printf("Code length code[%d]=%d (for %d)\n",
		//     codeLengthOrder[i],codeLengthCode[i],codeLengthOrder[i]);
	}

	unsigned hLengthCode[codeLengthLen],hCodeCode[codeLengthLen];
	MakeDynamicHuffmanCode(hLengthCode,hCodeCode,codeLengthLen,codeLengthCode);

	if(YSTRUE==YsGenericPngDecoder::verboseMode)
	{
		for(i=0; i<hCLen+4; i++)
		{
			printf("CodeLengthLen[%3d]=%d  HuffmanCode=%08x\n",i,codeLengthCode[i],hCodeCode[i]);
		}
	}

	// for(i=0; i<codeLengthLen; i++)
	// {
	// 	printf("[%d] %08x %08xx\n",i,hLengthCode[i],hCodeCode[i]);
	// }

	hLengthLiteral=hLengthBuf;
	hCodeLiteral=hCodeBuf;
	hLengthDist=hLengthBuf+hLit+257;
	hCodeDist=hCodeBuf+hLit+257;

	YsPngHuffmanTree *lengthTree,*lengthTreePtr;
	lengthTree=MakeHuffmanTree(codeLengthLen,hLengthCode,hCodeCode);

	unsigned int nExtr;
	nExtr=0;
	lengthTreePtr=lengthTree;
	while(nExtr<hLit+257+hDist+1)
	{
		lengthTreePtr=lengthTreePtr->Traverse(GetNextBit(dat,bytePtr,bitPtr));
		if(lengthTreePtr->Zero()==NULL && lengthTreePtr->One()==NULL)
		{
			unsigned value,copyLength;
			value=lengthTreePtr->dat;

			// printf("Value=%d\n",value);

			if(value<=15)
			{
				hLengthBuf[nExtr++]=value;
			}
			else if(value==16)
			{
				copyLength=3+GetNextMultiBit(dat,bytePtr,bitPtr,2);
				// printf("copyLength=%d\n",copyLength);
				while(copyLength>0)
				{
					hLengthBuf[nExtr]=hLengthBuf[nExtr-1];
					nExtr++;
					copyLength--;
				}
			}
			else if(value==17)
			{
				copyLength=3+GetNextMultiBit(dat,bytePtr,bitPtr,3);
				// printf("copyLength=%d\n",copyLength);
				while(copyLength>0)
				{
					hLengthBuf[nExtr++]=0;
					copyLength--;
				}
			}
			else if(value==18)
			{
				copyLength=11+GetNextMultiBit(dat,bytePtr,bitPtr,7);
				// printf("copyLength=%d\n",copyLength);
				while(copyLength>0)
				{
					hLengthBuf[nExtr++]=0;
					copyLength--;
				}
			}

			lengthTreePtr=lengthTree;

			// printf("nExtr=%d/%d\n",nExtr,hLit+257+hDist+1);
		}
	}

	if(YSTRUE==YsGenericPngDecoder::verboseMode)
	{
		for(i=0; i<hLit+257; i++)
		{
			printf("LiteralLength[%3d]=%d\n",i,hLengthLiteral[i]);
		}
		for(i=0; i<hDist+1; i++)
		{
			printf("Dist [%d] Length %d\n",i,hLengthDist[i]);
		}
	}

	if(YsGenericPngDecoder::verboseMode==YSTRUE)
	{
		printf("Making Huffman Code from Code Lengths\n");
	}
	MakeDynamicHuffmanCode(hLengthLiteral,hCodeLiteral,hLit+257,hLengthLiteral);
	MakeDynamicHuffmanCode(hLengthDist,hCodeDist,hDist+1,hLengthDist);

	DeleteHuffmanTree(lengthTree);

	return YSOK;
}

YsPngHuffmanTree *YsPngUncompressor::MakeHuffmanTree(unsigned n,unsigned hLength[],unsigned hCode[])
{
	unsigned i,j,mask;
	YsPngHuffmanTree *root,*ptr;
	root=new YsPngHuffmanTree;

	for(i=0; i<n; i++)
	{
		if(hLength[i]>0)
		{
			ptr=root;
			mask=(1<<(hLength[i]-1));
			for(j=0; j<hLength[i]; j++)
			{
				if(hCode[i]&mask)
				{
					if(ptr->One()==NULL)
					{
						ptr->One()=new YsPngHuffmanTree;
					}
					ptr=ptr->One();
				}
				else
				{
					if(ptr->Zero()==NULL)
					{
						ptr->Zero()=new YsPngHuffmanTree;
					}
					ptr=ptr->Zero();
				}
				mask>>=1;
			}
			ptr->dat=i;
		}
	}

	return root;
}

void YsPngUncompressor::DeleteHuffmanTree(YsPngHuffmanTree *node)
{
	YsPngHuffmanTree::DeleteHuffmanTree(node);
}

unsigned YsPngUncompressor::GetCopyLength(unsigned value,unsigned char dat[],unsigned &bytePtr,unsigned &bitPtr)
{
	unsigned copyLength;

	if(value<=264)
	{
		copyLength=3+(value-257);
	}
	else if(value>=285)
	{
		copyLength=258;
	}
	else
	{
		unsigned base,offset,extBits;
		extBits=1+(value-265)/4;
		base=(8<<((value-265)/4))+3;
		offset=((value-265)&3)*(2<<((value-265)/4));

		copyLength=GetNextMultiBit(dat,bytePtr,bitPtr,extBits);
		copyLength+=base+offset;
	}

	return copyLength;
}

unsigned YsPngUncompressor::GetBackwardDistance
   (unsigned distCode,unsigned char dat[],unsigned &bytePtr,unsigned &bitPtr)
{
	unsigned backDist;

	if(distCode<=3)
	{
		backDist=distCode+1;
	}
	else
	{
		unsigned base,offset,extBits;

		base=(4<<((distCode-4)/2))+1;
		offset=(distCode&1)*(2<<((distCode-4)/2));
		extBits=(distCode-2)/2;

		backDist=GetNextMultiBit(dat,bytePtr,bitPtr,extBits);
		backDist+=base+offset;
	}

	return backDist;
}

int YsPngUncompressor::Uncompress(unsigned length,unsigned char dat[])
{
	unsigned windowUsed;
	unsigned char *windowBuf;
	unsigned nByteExtracted;

	YsPngHuffmanTree *codeTree=NULL;
	YsPngHuffmanTree *distTree=NULL;


	windowBuf=NULL;

	unsigned bytePtr,bitPtr;
	bytePtr=0;
	bitPtr=1;
	nByteExtracted=0;


	if(YsGenericPngDecoder::verboseMode==YSTRUE)
	{
		printf("Begin zLib block length=%d bytePtr=%d bitPtr=0x%02x\n",length,bytePtr,bitPtr);
	}

	unsigned char cmf,flg;
	cmf=dat[bytePtr++];
	flg=dat[bytePtr++];

	unsigned cm,cInfo,windowSize;
	cm=cmf&0x0f;
	if(cm!=8)
	{
		printf("Unsupported compression method! (%d)\n",cm);
		goto ERREND;
	}

	cInfo=(cmf&0xf0)>>4;
	windowSize=1<<(cInfo+8);

	if(YsGenericPngDecoder::verboseMode==YSTRUE)
	{
		printf("cInfo=%d, Window Size=%d\n",cInfo,windowSize);
	}

	windowBuf=new unsigned char [windowSize];
	windowUsed=0;



	unsigned fCheck,fDict,fLevel;
	fCheck=(flg&15);
	fDict=(flg&32)>>5;
	fLevel=(flg&192)>>6;

	if(YsGenericPngDecoder::verboseMode==YSTRUE)
	{
		printf("fCheck=%d fDict=%d fLevel=%d\n",fCheck,fDict,fLevel);
	}


	if(fDict!=0)
	{
		printf("PNG is not supposed to have a preset dictionary.\n");
		goto ERREND;
	}


	YsPngHuffmanTree *codeTreePtr;
	YsPngHuffmanTree *distTreePtr;

	for(;;)
	{
		unsigned bFinal,bType;

		bFinal=GetNextBit(dat,bytePtr,bitPtr);
		bType=GetNextMultiBit(dat,bytePtr,bitPtr,2);

		if(bytePtr>=length)
		{
			printf("Buffer overflow\n");
			goto ERREND;
		}

		if(YsGenericPngDecoder::verboseMode==YSTRUE)
		{
			printf("bFinal=%d bType=%d\n",bFinal,bType);
		}

		if(bType==0) // No Compression
		{
			unsigned len;
			if(bitPtr!=1)
			{
				bitPtr=1;
				bytePtr++;
			}
			if(bytePtr>=length)
			{
				printf("Buffer overflow\n");
				goto ERREND;
			}

			len=dat[bytePtr]+dat[bytePtr+1]*256;
			bytePtr+=4;

			// Feed len bytes

			for(int i=0; i<(int)len; i++)  // 2010/02/08
			{
				output->Output(dat[bytePtr+i]);
				windowBuf[windowUsed++]=dat[bytePtr+i];  // 2014/03/22
				windowUsed&=(windowSize-1);              // 2014/03/22
			}

			bytePtr+=len;
		}
		else if(bType==1 || bType==2)
		{
			codeTree=NULL;

			if(bType==1)
			{
				unsigned hLength[288],hCode[288];
				MakeFixedHuffmanCode(hLength,hCode);
				codeTree=MakeHuffmanTree(288,hLength,hCode);
				distTree=NULL;
			}
			else
			{
				unsigned hLit,hDist,hCLen;
				unsigned *hLengthLiteral,*hCodeLiteral;
				unsigned *hLengthDist,*hCodeDist;
				unsigned hLengthBuf[322],hCodeBuf[322];

				DecodeDynamicHuffmanCode
				   (hLit,hDist,hCLen,
				    hLengthLiteral,hCodeLiteral,hLengthDist,hCodeDist,hLengthBuf,hCodeBuf,
				    dat,bytePtr,bitPtr);

				if(YsGenericPngDecoder::verboseMode==YSTRUE)
				{
					printf("Making Huffman Tree\n");
				}
				codeTree=MakeHuffmanTree(hLit+257,hLengthLiteral,hCodeLiteral);
				distTree=MakeHuffmanTree(hDist+1,hLengthDist,hCodeDist);
			}


			if(YsGenericPngDecoder::verboseMode==YSTRUE)
			{
				printf("Huffman table paprared\n");
			}


			codeTreePtr=codeTree;
			if(codeTree!=NULL)
			{
				for(;;)
				{
					codeTreePtr=codeTreePtr->Traverse(GetNextBit(dat,bytePtr,bitPtr));

					if(codeTreePtr==NULL)
					{
						printf("Huffman Decompression: Reached NULL node.\n");
						goto ERREND;
					}

					if(codeTreePtr->Zero()==NULL && codeTreePtr->One()==NULL)
					{
						// printf("[%d]\n",codeTreePtr->dat);

						unsigned value;
						value=codeTreePtr->dat;
						if(value<256)
						{
							windowBuf[windowUsed++]=(unsigned char)value;
							windowUsed&=(windowSize-1);
							if(output->Output((unsigned char)value)!=YSOK)
							{
								goto ERREND;
							}
							nByteExtracted++;
						}
						else if(value==256)
						{
							break;
						}
						else if(value<=285)
						{
							unsigned copyLength,distCode,backDist;
							copyLength=GetCopyLength(value,dat,bytePtr,bitPtr);
							// printf("CopyLength %d\n",copyLength);

							if(bType==1)
							{
								distCode=16*GetNextBit(dat,bytePtr,bitPtr);  // 5 bits fixed
								distCode+=8*GetNextBit(dat,bytePtr,bitPtr);  // Reversed order
								distCode+=4*GetNextBit(dat,bytePtr,bitPtr);
								distCode+=2*GetNextBit(dat,bytePtr,bitPtr);
								distCode+=  GetNextBit(dat,bytePtr,bitPtr);
							}
							else
							{
								distTreePtr=distTree;
								while(distTreePtr->Zero()!=NULL || distTreePtr->One()!=NULL)
								{
									distTreePtr=distTreePtr->Traverse(GetNextBit(dat,bytePtr,bitPtr));
								}
								distCode=distTreePtr->dat;
							}
							backDist=GetBackwardDistance(distCode,dat,bytePtr,bitPtr);
							// printf("DistCode %d BackDist %d\n",distCode,backDist);


							unsigned i;
							for(i=0; i<copyLength; i++)
							{
								unsigned char dat;
								dat=windowBuf[(windowUsed-backDist)&(windowSize-1)];
								if(output->Output(dat)!=YSOK)
								{
									goto ERREND;
								}
								nByteExtracted++;
								windowBuf[windowUsed++]=dat;
								windowUsed&=(windowSize-1);
							}
						}

						codeTreePtr=codeTree;
					}

					if(length<=bytePtr)
					{
						goto ERREND;
					}
				}
			}


			DeleteHuffmanTree(codeTree);
			DeleteHuffmanTree(distTree);
			codeTree=NULL;
			distTree=NULL;
		}
		else
		{
			printf("Unknown compression type (bType=3)\n");
			goto ERREND;
		}


		if(bFinal!=0)
		{
			break;
		}
	}

	delete [] windowBuf;
	windowBuf=NULL;



	if(YsGenericPngDecoder::verboseMode==YSTRUE)
	{
		printf("End zLib block length=%d bytePtr=%d bitPtr=0x%02x\n",length,bytePtr,bitPtr);
		printf("Huffman Tree Leak Tracker = %d\n",YsPngHuffmanTree::leakTracker);
		printf("Output %d bytes.\n",nByteExtracted);
	}

	return YSOK;

ERREND:
	if(windowBuf!=NULL)
	{
		delete [] windowBuf;
	}
	if(codeTree!=NULL)
	{
		DeleteHuffmanTree(codeTree);
	}
	if(distTree!=NULL)
	{
		DeleteHuffmanTree(distTree);
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

int YsGenericPngDecoder::Decode(const char fn[])
{
	int res=YSERR;
	FILE *fp=fopen(fn,"rb");
	if(NULL!=fp)
	{
		YsPngBinaryFileStream binStream(fp);
		res=Decode(binStream);
		fclose(fp);
	}
	return res;
}

int YsGenericPngDecoder::Decode(FILE *fp)
{
	if(NULL!=fp)
	{
		YsPngBinaryFileStream binStream(fp);
		return Decode(binStream);
	}
	return YSERR;
}

int YsGenericPngDecoder::Decode(YsPngGenericBinaryStream &binStream)
{
	const size_t fileSize=binStream.GetSize();

	if(CheckSignature(binStream)!=YSOK)
	{
		printf("The file does not have PNG signature.\n");
		return YSERR;
	}

	unsigned datBufUsed;
	unsigned char *datBuf;


	datBufUsed=0;
	datBuf=new unsigned char [fileSize];


	unsigned char *buf;
	unsigned length,chunkType,crc;
	while(ReadChunk(length,buf,chunkType,crc,binStream)==YSOK && chunkType!=IEND)
	{
		switch(chunkType)
		{
		default:
			if(buf!=NULL)
			{
				delete [] buf;
			}
			break;
		case IHDR:
			if(buf!=NULL)
			{
				if(length>=13)
				{
					hdr.Decode(buf);
				}
				delete [] buf;
			}
			break;
		case PLTE:
			if(buf!=NULL)
			{
				if(plt.Decode(length,buf)!=YSOK)
				{
					delete [] buf;
					return YSERR;
				}
				delete [] buf;
			}
			break;
		case tRNS:
			if(buf!=NULL)
			{
				trns.Decode(length,buf,hdr.colorType);
				delete [] buf;
			}
			break;
		case gAMA:
			if(buf!=NULL && length>=4)
			{
				gamma=PngGetUnsignedInt(buf);
				if(YsGenericPngDecoder::verboseMode==YSTRUE)
				{
					printf("Gamma %d (default=%d)\n",gamma,gamma_default);
				}
				delete [] buf;
			}
			break;
		case IDAT:
			if(buf!=NULL)
			{
				unsigned i;
				for(i=0; i<length; i++)
				{
					datBuf[datBufUsed+i]=buf[i];
				}
				datBufUsed+=length;
				delete [] buf;
			}
			break;
		}
	}



	if(PrepareOutput()==YSOK)
	{
		YsPngUncompressor uncompressor;
		uncompressor.output=this;
		uncompressor.Uncompress(datBufUsed,datBuf);

		EndOutput();
	}


	delete [] datBuf;
	return YSOK;
}

int YsGenericPngDecoder::PrepareOutput(void)
{
	return YSOK;
}

int YsGenericPngDecoder::Output(unsigned char)
{
	return YSOK;
}

int YsGenericPngDecoder::EndOutput(void)
{
	return YSOK;
}



////////////////////////////////////////////////////////////

static inline unsigned char Paeth(unsigned int ua,unsigned int ub,unsigned int uc)
{
	int a,b,c,p,pa,pb,pc;

	a=(int)ua;
	b=(int)ub;
	c=(int)uc;

	p=a+b-c;
	pa=(p>a ? p-a : a-p);
	pb=(p>b ? p-b : b-p);
	pc=(p>c ? p-c : c-p);

	if(pa<=pb && pa<=pc)
	{
		return (unsigned char)a;
	}
	else if(pb<=pc)
	{
		return (unsigned char)b;
	}
	else
	{
		return (unsigned char)c;
	}
}

static inline void Filter8(unsigned char curLine[],unsigned char prvLine[],int x,int y,int unitLng,int filter)
{
	int i;
	switch(filter)
	{
	case 1:
		if(x>0)
		{
			for(i=0; i<unitLng; i++)
			{
				curLine[x*unitLng+i]+=curLine[x*unitLng+i-unitLng];
			}
		}
		break;
	case 2:
		if(y>0)
		{
			for(i=0; i<unitLng; i++)
			{
				curLine[x*unitLng+i]+=prvLine[x*unitLng+i];
			}
		}
		break;
	case 3:
		for(i=0; i<unitLng; i++)
		{
			unsigned int a;
			a=(x>0 ? curLine[x*unitLng+i-unitLng] : 0);
			a+=(y>0 ? prvLine[x*unitLng+i] : 0);
			curLine[x*unitLng+i]+=(unsigned char)(a/2);
		}
		break;
	case 4:
		for(i=0; i<unitLng; i++)
		{
			unsigned int a,b,c;
			a=(x>0 ? curLine[x*unitLng+i-unitLng] : 0);
			b=(y>0 ? prvLine[x*unitLng+i] : 0);
			c=((x>0 && y>0) ? prvLine[x*unitLng-unitLng+i] : 0);
			curLine[x*unitLng+i]+=Paeth(a,b,c);
		}
		break;
	}
}

YsRawPngDecoder::YsRawPngDecoder()
{
	wid=0;
	hei=0;
	rgba=NULL;
	twoLineBuf8=NULL;

	curLine8=NULL;
	prvLine8=NULL;

	autoDeleteRgbaBuffer=1;
}

YsRawPngDecoder::~YsRawPngDecoder()
{
	if(autoDeleteRgbaBuffer==1 && rgba!=NULL)
	{
		delete [] rgba;
	}
	if(twoLineBuf8!=NULL)
	{
		delete [] twoLineBuf8;
	}
}

void YsRawPngDecoder::ShiftTwoLineBuf(void)
{
	if(twoLineBuf8!=NULL)
	{
		unsigned char *swap;
		swap=curLine8;
		curLine8=prvLine8;
		prvLine8=swap;
	}
}

int YsRawPngDecoder::PrepareOutput(void)
{
	int supported;

	supported=0;
	switch(hdr.colorType)
	{
	case 0:   // Greyscale
		switch(hdr.bitDepth)
		{
		case 8:
		case 1:
			supported=1;
			break;
		case 2:
		case 4:
		case 16:
			break;
		}
		break;
	case 2:   // Truecolor
		switch(hdr.bitDepth)
		{
		case 8:
		case 16:
			supported=1;
			break;
		}
		break;
	case 3:   // Indexed-color
		switch(hdr.bitDepth)
		{
		case 1:
		case 2:
		case 4:
		case 8:
			supported=1;
			break;
		}
		break;
	case 4:   // Greyscale with alpha
		switch(hdr.bitDepth)
		{
		case 8:
			supported=1;
			break;
		case 16:
			break;
		}
		break;
	case 6:   // Truecolor with alpha
		switch(hdr.bitDepth)
		{
		case 8:
			supported=1;
			break;
		case 16:
			break;
		}
		break;
	}

	if(supported==0)
	{
		printf("Unsupported colorType-bitDepth combination.\n");
		printf("  Color type=%d\n",hdr.colorType);
		printf("  Bit deptch=%d\n",hdr.bitDepth);
		return YSERR;
	}



	wid=hdr.width;
	hei=hdr.height;
	if(autoDeleteRgbaBuffer==1 && rgba!=NULL)
	{
		delete [] rgba;
		rgba=NULL;
	}
	rgba=new unsigned char [wid*hei*4];
	x=-1;
	y=0;
	filter=0;
	inLineCount=0;
	inPixelCount=0;
	firstByte=1;
	index=0;
	interlacePass=1;

	if(twoLineBuf8!=NULL)
	{
		delete [] twoLineBuf8;
		twoLineBuf8=NULL;
	}


	// See PNG Specification 11.2 for Allowed combinations of color type and bit depth
	unsigned int twoLineBufLngPerLine=0;
	switch(hdr.colorType)
	{
	default:
		printf("Internal error!  Color type was supposed to be checked in the previous switch/case statement!\n");
		return YSERR;
	case 0:   // Greyscale
		switch(hdr.bitDepth)
		{
		case 1:
			twoLineBufLngPerLine=(hdr.width+7)/8;
			break;
		case 2:
			twoLineBufLngPerLine=(hdr.width+3)/4;
			break;
		case 4:
			twoLineBufLngPerLine=(hdr.width+1)/2;
			break;
		case 8:
			twoLineBufLngPerLine=hdr.width;
			break;
		case 16:
			twoLineBufLngPerLine=hdr.width*2;
			break;
		}
		break;
	case 2:   // Truecolor
		switch(hdr.bitDepth)
		{
		case 8:
			twoLineBufLngPerLine=hdr.width*3;
			break;
		case 16:
			twoLineBufLngPerLine=hdr.width*6;
			break;
		}
		break;
	case 3:   // Indexed-color
		switch(hdr.bitDepth)
		{
		case 1:
			twoLineBufLngPerLine=(hdr.width+7)/8;
			break;
		case 2:
			twoLineBufLngPerLine=(hdr.width+3)/4;
			break;
		case 4:
			twoLineBufLngPerLine=(hdr.width+1)/2;
			break;
		case 8:
			twoLineBufLngPerLine=hdr.width;
			break;
		}
		break;
	case 4:   // Greyscale with alpha
		switch(hdr.bitDepth)
		{
		case 8:
			twoLineBufLngPerLine=hdr.width*2;
			break;
		case 16:
			twoLineBufLngPerLine=hdr.width*4;
			break;
		}
		break;
	case 6:   // Truecolor with alpha
		switch(hdr.bitDepth)
		{
		case 8:
			twoLineBufLngPerLine=hdr.width*4;
			break;
		case 16:
			twoLineBufLngPerLine=hdr.width*8;
			break;
		}
		break;
	}

	twoLineBuf8=new unsigned char [twoLineBufLngPerLine*2];
	curLine8=twoLineBuf8;
	prvLine8=twoLineBuf8+twoLineBufLngPerLine;

	return YSOK;
}

int YsRawPngDecoder::Output(unsigned char dat)
{
	unsigned int i;
	unsigned int colIdx;
	unsigned int interlaceWid,interlaceHei,interlaceX,interlaceY;

	if(y>=hei)
	{
		return YSERR;
	}

	if(x==-1)  // First byte is filter type for the line.  
	{
		filter=dat;   // See PNG Specification 4.5.4 Filtering, 9 Filtering
		inLineCount=0;
		inPixelCount=0;
		x++;
		// printf("y=%d filter=%d\n",y,filter);
		return YSOK;
	}
	else 
	{
		switch(hdr.interlaceMethod)
		{
		// Non-Interlace
		case 0:
			switch(hdr.colorType)  // See PNG Specification 6.1 Colour types and values
			{
			// Grayscale
			case 0:
				switch(hdr.bitDepth)
				{
				case 1:
					curLine8[x/8]=dat;
					Filter8(curLine8,prvLine8,x/8,y,1,filter);

					for(i=0; i<8 && x<wid; i++)
					{
						colIdx=(curLine8[x/8]>>(7-i))&1;
						colIdx=(colIdx<<1)+colIdx;
						colIdx=(colIdx<<2)+colIdx;
						colIdx=(colIdx<<4)+colIdx;

						rgba[index  ]=(unsigned char)colIdx;
						rgba[index+1]=(unsigned char)colIdx;
						rgba[index+2]=(unsigned char)colIdx;
						rgba[index+3]=0;
						x++;
						index+=4;
					}
					break;

				case 8:
					curLine8[x]=dat;
					Filter8(curLine8,prvLine8,x,y,1,filter);
					colIdx=curLine8[x];

					rgba[index  ]=curLine8[x];
					rgba[index+1]=curLine8[x];
					rgba[index+2]=curLine8[x];
					if(curLine8[x]==trns.col[0] || curLine8[x]==trns.col[1] || curLine8[x]==trns.col[2])
					{
						rgba[index+3]=0;
					}
					else
					{
						rgba[index+3]=255;
					}

					x++;
					index+=4;
					break;
				}
				break;



			// True color
			case 2:
				switch(hdr.bitDepth)
				{
				case 8:
					curLine8[inLineCount+inPixelCount]=dat;
					inPixelCount++;
					if(inPixelCount==3)
					{
						Filter8(curLine8,prvLine8,x,y,3,filter);
						rgba[index  ]=curLine8[inLineCount];
						rgba[index+1]=curLine8[inLineCount+1];
						rgba[index+2]=curLine8[inLineCount+2];

						if(curLine8[inLineCount  ]==trns.col[0] && 
						   curLine8[inLineCount+1]==trns.col[1] && 
						   curLine8[inLineCount+2]==trns.col[2])
						{
							rgba[index+3]=0;
						}
						else
						{
							rgba[index+3]=255;
						}

						x++;
						index+=4;
						inLineCount+=3;
						inPixelCount=0;
					}
					break;
				case 16:
					curLine8[inLineCount+inPixelCount]=dat;
					inPixelCount++;
					if(inPixelCount==6)
					{
						Filter8(curLine8,prvLine8,x,y,6,filter);
						rgba[index  ]=curLine8[inLineCount  ];
						rgba[index+1]=curLine8[inLineCount+2];
						rgba[index+2]=curLine8[inLineCount+4];

						r=curLine8[inLineCount  ]*256+curLine8[inLineCount+1];
						g=curLine8[inLineCount+2]*256+curLine8[inLineCount+3];
						b=curLine8[inLineCount+4]*256+curLine8[inLineCount+5];
						if(r==trns.col[0] && g==trns.col[1] && b==trns.col[2])
						{
							rgba[index+3]=0;
						}
						else
						{
							rgba[index+3]=255;
						}
						x++;
						index+=4;
						inLineCount+=6;
						inPixelCount=0;
					}
					break;
				}
				break;



			// Indexed color
			case 3:
				switch(hdr.bitDepth)
				{
				case 1:
					curLine8[x/8]=dat;
					Filter8(curLine8,prvLine8,x/8,y,1,filter);

					for(i=0; i<8 && x<wid; i++)
					{
						colIdx=(curLine8[x/8]>>(7-i))&1;
						if(colIdx<plt.nEntry)
						{
							rgba[index  ]=plt.entry[colIdx*3  ];
							rgba[index+1]=plt.entry[colIdx*3+1];
							rgba[index+2]=plt.entry[colIdx*3+2];
							if(colIdx==trns.col[0] || colIdx==trns.col[1] || colIdx==trns.col[2])
							{
								rgba[index+3]=0;
							}
							else
							{
								rgba[index+3]=255;
							}
						}
						else
						{
							printf("Not enough palette entry! (%d)\n",plt.nEntry);
						}
						x++;
						index+=4;
					}
					break;
				case 2:
					curLine8[x/4]=dat;
					Filter8(curLine8,prvLine8,x/4,y,1,filter);

					for(i=0; i<4 && x<wid; i++)
					{
						colIdx=(curLine8[x/4]>>((3-i)*2))&0x03;

						if(colIdx<plt.nEntry)
						{
							rgba[index  ]=plt.entry[colIdx*3  ];
							rgba[index+1]=plt.entry[colIdx*3+1];
							rgba[index+2]=plt.entry[colIdx*3+2];
							if(colIdx==trns.col[0] || colIdx==trns.col[1] || colIdx==trns.col[2])
							{
								rgba[index+3]=0;
							}
							else
							{
								rgba[index+3]=255;
							}
						}
						x++;
						index+=4;
					}
					break;

				case 4:
					curLine8[x/2]=dat;
					Filter8(curLine8,prvLine8,x/2,y,1,filter);

					for(i=0; i<2 && x<wid; i++)
					{
						colIdx=(curLine8[x/2]>>((1-i)*4))&0x0f;

						if(colIdx<plt.nEntry)
						{
							rgba[index  ]=plt.entry[colIdx*3  ];
							rgba[index+1]=plt.entry[colIdx*3+1];
							rgba[index+2]=plt.entry[colIdx*3+2];
							if(colIdx==trns.col[0] || colIdx==trns.col[1] || colIdx==trns.col[2])
							{
								rgba[index+3]=0;
							}
							else
							{
								rgba[index+3]=255;
							}
						}
						x++;
						index+=4;
					}
					break;

				case 8:
					curLine8[x]=dat;
					Filter8(curLine8,prvLine8,x,y,1,filter);
					colIdx=curLine8[x];

					if(colIdx<plt.nEntry)
					{
						rgba[index  ]=plt.entry[colIdx*3  ];
						rgba[index+1]=plt.entry[colIdx*3+1];
						rgba[index+2]=plt.entry[colIdx*3+2];
						if(colIdx==trns.col[0] || colIdx==trns.col[1] || colIdx==trns.col[2])
						{
							rgba[index+3]=0;
						}
						else
						{
							rgba[index+3]=255;
						}
					}
					x++;
					index+=4;
					break;
				}
				break;



			// Greyscale with alpha
			case 4:
				switch(hdr.bitDepth)
				{
				case 8:
					curLine8[inLineCount+inPixelCount]=dat;
					inPixelCount++;
					if(inPixelCount==2)
					{
						Filter8(curLine8,prvLine8,x,y,2,filter);
						rgba[index  ]=curLine8[inLineCount  ];
						rgba[index+1]=curLine8[inLineCount  ];
						rgba[index+2]=curLine8[inLineCount  ];
						rgba[index+3]=curLine8[inLineCount+1];
						index+=4;
						x++;
						inLineCount+=2;
						inPixelCount=0;
					}
					break;
				}
				break;



			// Truecolor with alpha
			case 6:
				switch(hdr.bitDepth)
				{
				case 8:
					curLine8[inLineCount+inPixelCount]=dat;
					inPixelCount++;
					if(inPixelCount==4)
					{
						Filter8(curLine8,prvLine8,x,y,4,filter);
						rgba[index  ]=curLine8[inLineCount  ];
						rgba[index+1]=curLine8[inLineCount+1];
						rgba[index+2]=curLine8[inLineCount+2];
						rgba[index+3]=curLine8[inLineCount+3];
						index+=4;
						x++;
						inLineCount+=4;
						inPixelCount=0;
					}
					break;
				}
				break;
			}  // switch(hdr.colorType)

			if(x>=wid)
			{
				y++;
				x=-1;
				ShiftTwoLineBuf();
			}

			return YSOK;


		// Interlace
		case 1:
			//   1 6 4 6 2 6 4 6
			//   7 7 7 7 7 7 7 7
			//   5 6 5 6 5 6 5 6
			//   7 7 7 7 7 7 7 7
			//   3 6 4 6 3 6 4 6
			//   7 7 7 7 7 7 7 7
			//   5 6 5 6 5 6 5 6
			//   7 7 7 7 7 7 7 7
			switch(interlacePass)
			{
			case 1:
				interlaceWid=(wid+7)/8;
				interlaceHei=(hei+7)/8;
				interlaceX=x*8;
				interlaceY=y*8;
				break;
			case 2:
				interlaceWid=(wid+3)/8;
				interlaceHei=(hei+7)/8;
				interlaceX=4+x*8;
				interlaceY=y*8;
				break;
			case 3:
				interlaceWid=(wid+3)/4;
				interlaceHei=(hei+3)/8;
				interlaceX=x*4;
				interlaceY=4+y*8;
				break;
			case 4:
				interlaceWid=(wid+1)/4;
				interlaceHei=(hei+3)/4;
				interlaceX=2+x*4;
				interlaceY=y*4;
				break;
			case 5:
				interlaceWid=(wid+1)/2;
				interlaceHei=(hei+1)/4;
				interlaceX=x*2;
				interlaceY=2+y*4;
				break;
			case 6:
				interlaceWid=(wid  )/2;
				interlaceHei=(hei+1)/2;
				interlaceX=1+x*2;
				interlaceY=y*2;
				break;
			case 7:
				interlaceWid=wid;
				interlaceHei=hei/2;
				interlaceX=x;
				interlaceY=1+y*2;
				break;
			default:
				return YSERR;
			} // switch(interlacePass)

			switch(hdr.colorType)  // See PNG Specification 6.1 Colour types and values
			{
			// Grayscale
			case 0:
				switch(hdr.bitDepth)
				{
				case 8:
					curLine8[inLineCount]=dat;
					Filter8(curLine8,prvLine8,x,y,1,filter);

					index=interlaceX*4+interlaceY*wid*4;
					rgba[index  ]=curLine8[inLineCount];
					rgba[index+1]=curLine8[inLineCount];
					rgba[index+2]=curLine8[inLineCount];

					if(curLine8[inLineCount]==trns.col[0] || 
					   curLine8[inLineCount]==trns.col[1] || 
					   curLine8[inLineCount]==trns.col[2])
					{
						rgba[index+3]=0;
					}
					else
					{
						rgba[index+3]=255;
					}

					x++;
					inLineCount++;
					break;
				}
				break;


			// True color
			case 2:
				switch(hdr.bitDepth)
				{
				case 8:
					curLine8[inLineCount+inPixelCount]=dat;
					inPixelCount++;
					if(inPixelCount==3)
					{
						Filter8(curLine8,prvLine8,x,y,3,filter);

						index=interlaceX*4+interlaceY*wid*4;
						rgba[index  ]=curLine8[inLineCount  ];
						rgba[index+1]=curLine8[inLineCount+1];
						rgba[index+2]=curLine8[inLineCount+2];

						if(curLine8[inLineCount  ]==trns.col[0] && 
						   curLine8[inLineCount+1]==trns.col[1] && 
						   curLine8[inLineCount+2]==trns.col[2])
						{
							rgba[index+3]=0;
						}
						else
						{
							rgba[index+3]=255;
						}

						x++;
						inLineCount+=3;
						inPixelCount=0;
					}
					break;

				case 16:
					curLine8[inLineCount+inPixelCount]=dat;
					inPixelCount++;
					if(inPixelCount==6)
					{
						Filter8(curLine8,prvLine8,x,y,6,filter);
						index=interlaceX*4+interlaceY*wid*4;
						rgba[index  ]=curLine8[inLineCount  ];
						rgba[index+1]=curLine8[inLineCount+2];
						rgba[index+2]=curLine8[inLineCount+4];

						r=curLine8[inLineCount  ]*256+curLine8[inLineCount+1];
						g=curLine8[inLineCount+2]*256+curLine8[inLineCount+3];
						b=curLine8[inLineCount+4]*256+curLine8[inLineCount+5];

						if(r==trns.col[0] && g==trns.col[1] && b==trns.col[2])
						// Fixed based on a bug report from a viewer.  Thanks for the good catch!
						//   Report received 09/10/2010
						//   Fixed           09/18/2010
						{
							rgba[index+3]=0;
						}
						else
						{
							rgba[index+3]=255;
						}

						x++;
						inLineCount+=6;
						inPixelCount=0;
					}
					break;
				}
				break;



			// Indexed color
			case 3:
				switch(hdr.bitDepth)
				{
				case 8:
					curLine8[inLineCount]=dat;
					Filter8(curLine8,prvLine8,x,y,1,filter);

					index=interlaceX*4+interlaceY*wid*4;
					colIdx=curLine8[inLineCount  ];
					if(colIdx<plt.nEntry)
					{
						rgba[index  ]=plt.entry[colIdx*3  ];
						rgba[index+1]=plt.entry[colIdx*3+1];
						rgba[index+2]=plt.entry[colIdx*3+2];
						if(colIdx==trns.col[0] || colIdx==trns.col[1] || colIdx==trns.col[2])
						{
							rgba[index+3]=0;
						}
						else
						{
							rgba[index+3]=255;
						}
					}

					x++;
					inLineCount++;
					break;
				}
				break;



			// Greyscale with alpha
			case 4:
				switch(hdr.bitDepth)
				{
				case 8:
					curLine8[inLineCount+inPixelCount]=dat;
					inPixelCount++;
					if(inPixelCount==2)
					{
						Filter8(curLine8,prvLine8,x,y,2,filter);

						index=interlaceX*4+interlaceY*wid*4;
						rgba[index  ]=curLine8[inLineCount  ];
						rgba[index+1]=curLine8[inLineCount  ];
						rgba[index+2]=curLine8[inLineCount  ];
						rgba[index+3]=curLine8[inLineCount+1];

						x++;
						inLineCount+=2;
						inPixelCount=0;
					}
					break;
				}
				break;



			// Truecolor with alpha
			case 6:
				switch(hdr.bitDepth)
				{
				case 8:
					curLine8[inLineCount+inPixelCount]=dat;
					inPixelCount++;
					if(inPixelCount==4)
					{
						Filter8(curLine8,prvLine8,x,y,4,filter);

						index=interlaceX*4+interlaceY*wid*4;
						rgba[index  ]=curLine8[inLineCount  ];
						rgba[index+1]=curLine8[inLineCount+1];
						rgba[index+2]=curLine8[inLineCount+2];
						rgba[index+3]=curLine8[inLineCount+3];

						x++;
						inLineCount+=4;
						inPixelCount=0;
					}
					break;
				}
			break;
			} // switch(hdr.colorType)

			if(x>=(int)interlaceWid)
			{
				y++;
				x=-1;
				ShiftTwoLineBuf();
				if(y>=(int)interlaceHei)
				{
					y=0;
					interlacePass++;

					if(YsGenericPngDecoder::verboseMode==YSTRUE)
					{
						printf("Interlace Pass %d\n",interlacePass);
					}
				}
			}

			return YSOK;
		default:
			printf("Unsupported interlace method.\n");
			return YSERR;
		}
	}
	// return YSERR;
}

int YsRawPngDecoder::EndOutput(void)
{
	if(YsGenericPngDecoder::verboseMode==YSTRUE)
	{
		printf("Final Position (%d,%d)\n",x,y);
	}
	return YSOK;
}

void YsRawPngDecoder::Flip(void)  // For drawing in OpenGL
{
	int x,y,bytePerLine;
	unsigned int swp;
	bytePerLine=wid*4;
	for(y=0; y<hei/2; y++)
	{
		for(x=0; x<bytePerLine; x++)
		{
			swp=rgba[y*bytePerLine+x];
			rgba[y*bytePerLine+x]=rgba[(hei-1-y)*bytePerLine+x];
			rgba[(hei-1-y)*bytePerLine+x]=(unsigned char)swp;
		}
	}
}
