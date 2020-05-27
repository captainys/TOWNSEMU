/* ////////////////////////////////////////////////////////////

File Name: yspngenc.cpp
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
#include <string.h>
#include <stdlib.h>

#include "yspng.h"  // For the future, these should be merged.
#include "yspngenc.h"





class YsPngCRCCalculator
{
protected:
	static const unsigned long crcTable[256];
	unsigned long crc;

	enum
	{
		CRCMASK=0xffffffff
	};
public:
	void Initialize(void);
	inline void Add(unsigned int byteData);
	unsigned long GetCRC(void);
};

const unsigned long YsPngCRCCalculator::crcTable[256]=
{
	0x00000000,0x77073096,0xee0e612c,0x990951ba,0x076dc419,0x706af48f,0xe963a535,0x9e6495a3,
	0x0edb8832,0x79dcb8a4,0xe0d5e91e,0x97d2d988,0x09b64c2b,0x7eb17cbd,0xe7b82d07,0x90bf1d91,
	0x1db71064,0x6ab020f2,0xf3b97148,0x84be41de,0x1adad47d,0x6ddde4eb,0xf4d4b551,0x83d385c7,
	0x136c9856,0x646ba8c0,0xfd62f97a,0x8a65c9ec,0x14015c4f,0x63066cd9,0xfa0f3d63,0x8d080df5,
	0x3b6e20c8,0x4c69105e,0xd56041e4,0xa2677172,0x3c03e4d1,0x4b04d447,0xd20d85fd,0xa50ab56b,
	0x35b5a8fa,0x42b2986c,0xdbbbc9d6,0xacbcf940,0x32d86ce3,0x45df5c75,0xdcd60dcf,0xabd13d59,
	0x26d930ac,0x51de003a,0xc8d75180,0xbfd06116,0x21b4f4b5,0x56b3c423,0xcfba9599,0xb8bda50f,
	0x2802b89e,0x5f058808,0xc60cd9b2,0xb10be924,0x2f6f7c87,0x58684c11,0xc1611dab,0xb6662d3d,
	0x76dc4190,0x01db7106,0x98d220bc,0xefd5102a,0x71b18589,0x06b6b51f,0x9fbfe4a5,0xe8b8d433,
	0x7807c9a2,0x0f00f934,0x9609a88e,0xe10e9818,0x7f6a0dbb,0x086d3d2d,0x91646c97,0xe6635c01,
	0x6b6b51f4,0x1c6c6162,0x856530d8,0xf262004e,0x6c0695ed,0x1b01a57b,0x8208f4c1,0xf50fc457,
	0x65b0d9c6,0x12b7e950,0x8bbeb8ea,0xfcb9887c,0x62dd1ddf,0x15da2d49,0x8cd37cf3,0xfbd44c65,
	0x4db26158,0x3ab551ce,0xa3bc0074,0xd4bb30e2,0x4adfa541,0x3dd895d7,0xa4d1c46d,0xd3d6f4fb,
	0x4369e96a,0x346ed9fc,0xad678846,0xda60b8d0,0x44042d73,0x33031de5,0xaa0a4c5f,0xdd0d7cc9,
	0x5005713c,0x270241aa,0xbe0b1010,0xc90c2086,0x5768b525,0x206f85b3,0xb966d409,0xce61e49f,
	0x5edef90e,0x29d9c998,0xb0d09822,0xc7d7a8b4,0x59b33d17,0x2eb40d81,0xb7bd5c3b,0xc0ba6cad,
	0xedb88320,0x9abfb3b6,0x03b6e20c,0x74b1d29a,0xead54739,0x9dd277af,0x04db2615,0x73dc1683,
	0xe3630b12,0x94643b84,0x0d6d6a3e,0x7a6a5aa8,0xe40ecf0b,0x9309ff9d,0x0a00ae27,0x7d079eb1,
	0xf00f9344,0x8708a3d2,0x1e01f268,0x6906c2fe,0xf762575d,0x806567cb,0x196c3671,0x6e6b06e7,
	0xfed41b76,0x89d32be0,0x10da7a5a,0x67dd4acc,0xf9b9df6f,0x8ebeeff9,0x17b7be43,0x60b08ed5,
	0xd6d6a3e8,0xa1d1937e,0x38d8c2c4,0x4fdff252,0xd1bb67f1,0xa6bc5767,0x3fb506dd,0x48b2364b,
	0xd80d2bda,0xaf0a1b4c,0x36034af6,0x41047a60,0xdf60efc3,0xa867df55,0x316e8eef,0x4669be79,
	0xcb61b38c,0xbc66831a,0x256fd2a0,0x5268e236,0xcc0c7795,0xbb0b4703,0x220216b9,0x5505262f,
	0xc5ba3bbe,0xb2bd0b28,0x2bb45a92,0x5cb36a04,0xc2d7ffa7,0xb5d0cf31,0x2cd99e8b,0x5bdeae1d,
	0x9b64c2b0,0xec63f226,0x756aa39c,0x026d930a,0x9c0906a9,0xeb0e363f,0x72076785,0x05005713,
	0x95bf4a82,0xe2b87a14,0x7bb12bae,0x0cb61b38,0x92d28e9b,0xe5d5be0d,0x7cdcefb7,0x0bdbdf21,
	0x86d3d2d4,0xf1d4e242,0x68ddb3f8,0x1fda836e,0x81be16cd,0xf6b9265b,0x6fb077e1,0x18b74777,
	0x88085ae6,0xff0f6a70,0x66063bca,0x11010b5c,0x8f659eff,0xf862ae69,0x616bffd3,0x166ccf45,
	0xa00ae278,0xd70dd2ee,0x4e048354,0x3903b3c2,0xa7672661,0xd06016f7,0x4969474d,0x3e6e77db,
	0xaed16a4a,0xd9d65adc,0x40df0b66,0x37d83bf0,0xa9bcae53,0xdebb9ec5,0x47b2cf7f,0x30b5ffe9,
	0xbdbdf21c,0xcabac28a,0x53b39330,0x24b4a3a6,0xbad03605,0xcdd70693,0x54de5729,0x23d967bf,
	0xb3667a2e,0xc4614ab8,0x5d681b02,0x2a6f2b94,0xb40bbe37,0xc30c8ea1,0x5a05df1b,0x2d02ef8d
};

void YsPngCRCCalculator::Initialize(void)
{
	crc=CRCMASK;
}

inline void YsPngCRCCalculator::Add(unsigned int byteData)
{
	crc=crcTable[(crc^byteData)&0xff]^(crc>>8);
}

unsigned long YsPngCRCCalculator::GetCRC(void)
{
	return (~crc)&CRCMASK;
}





class YsPngHuffmanTreeManager
{
public:
	YsPngHuffmanTree *root;
	unsigned int nElem;
	unsigned int *elemFreq;

public:
	YsPngHuffmanTreeManager();
	~YsPngHuffmanTreeManager();

	int MakeTreeFromData(int nData,unsigned int byteData[],int nData2,unsigned int byteData2[],unsigned int maxValue);
	int MakeTreeFromFrequencyTable(unsigned int nElem,unsigned int elemFreq[]);
	unsigned int GetTreeDepth(void) const;

	int MakeCodeLengthArray(unsigned int hLength[]) const;  // Length must be equal to nElem

	void ReduceTreeDepth(void);

	void PrintInfo(void) const;

protected:
	void ClearFrequencyTable(void);
	void ClearTree(void);
	int RebuildHuffmanTree(void);
	void SortFreeNode(int nFreeNode,YsPngHuffmanTree **freeNode);
	int MakeCodeLengthArray(unsigned int hLength[],YsPngHuffmanTree *node,int depth) const;
};

YsPngHuffmanTreeManager::YsPngHuffmanTreeManager()
{
	root=NULL;
	nElem=0;
	elemFreq=NULL;
}

YsPngHuffmanTreeManager::~YsPngHuffmanTreeManager()
{
	ClearTree();
	ClearFrequencyTable();
}

int YsPngHuffmanTreeManager::MakeTreeFromData(int nData,unsigned int codeData[],int nData2,unsigned int codeData2[],unsigned int maxValue)
{
	int i;
	ClearFrequencyTable();
	ClearTree();

	nElem=maxValue+1;
	elemFreq=new unsigned int [nElem];
	for(i=0; i<(int)nElem; i++)
	{
		elemFreq[i]=0;
	}
	for(i=0; i<nData; i++)
	{
		if(maxValue<codeData[i])
		{
			printf("%s  Code data out of range.\n",__FUNCTION__);
			return YSERR;
		}
		elemFreq[codeData[i]]++;
	}
	for(i=0; i<nData2; i++)
	{
		if(maxValue<codeData2[i])
		{
			printf("%s  Code data2 out of range.\n",__FUNCTION__);
			return YSERR;
		}
		elemFreq[codeData2[i]]++;
	}

	return RebuildHuffmanTree();
}

int YsPngHuffmanTreeManager::MakeTreeFromFrequencyTable(unsigned int nElem,unsigned int elemFreq[])
{
	ClearTree();
	ClearFrequencyTable();

	this->nElem=nElem;
	this->elemFreq=new unsigned int [nElem];

	int i;
	for(i=0; i<(int)nElem; i++)
	{
		this->elemFreq[i]=elemFreq[i];
	}

	return RebuildHuffmanTree();
}

unsigned int YsPngHuffmanTreeManager::GetTreeDepth(void) const
{
	if(NULL!=root)
	{
		return root->depth;
	}
	return 0;
}

int YsPngHuffmanTreeManager::MakeCodeLengthArray(unsigned int hLength[]) const
{
	int i;
	for(i=0; i<(int)nElem; i++)
	{
		hLength[i]=0;
	}

	if(NULL==root->Zero() && NULL==root->One())
	{
		hLength[root->dat]=1;
		return YSOK;
	}
	else
	{
		return MakeCodeLengthArray(hLength,root,0);
	}
}

void YsPngHuffmanTreeManager::ReduceTreeDepth(void)
{
	int i;
	for(i=0; i<(int)nElem; i++)
	{
		if(elemFreq[i]>1)
		{
			elemFreq[i]/=2;
		}
	}
	RebuildHuffmanTree();
}

void YsPngHuffmanTreeManager::PrintInfo(void) const
{
	if(NULL!=root)
	{
		printf("Max depth=%d\n",root->depth);
		printf("Root Weight=%d\n",root->weight);
	}
}

void YsPngHuffmanTreeManager::ClearFrequencyTable(void)
{
	if(NULL!=elemFreq)
	{
		delete [] elemFreq;
		elemFreq=NULL;
	}
}

void YsPngHuffmanTreeManager::ClearTree(void)
{
	if(root!=NULL)
	{
		YsPngHuffmanTree::DeleteHuffmanTree(root);
		root=NULL;
	}
}

int YsPngHuffmanTreeManager::RebuildHuffmanTree(void)
{
	ClearTree();
	if(nElem>0)
	{
		YsPngHuffmanTree **freeNode;
		freeNode=new YsPngHuffmanTree * [nElem];

		int i;
		int nFreeNode=0;
		for(i=0; i<(int)nElem; i++)
		{
			if(elemFreq[i]>0)
			{
				freeNode[nFreeNode]=new YsPngHuffmanTree;
				freeNode[nFreeNode]->dat=i;
				freeNode[nFreeNode]->weight=elemFreq[i];
				freeNode[nFreeNode]->depth=0;
				nFreeNode++;
			}
		}

		if(nFreeNode==1)
		{
			root=freeNode[0];
			root->depth=1;
		}
		else if(nFreeNode>1)
		{
			SortFreeNode(nFreeNode,freeNode);
			while(nFreeNode>1)
			{
				YsPngHuffmanTree *newNode;
				newNode=new YsPngHuffmanTree;

				newNode->Zero()=freeNode[nFreeNode-2];
				newNode->One()=freeNode[nFreeNode-1];

				newNode->weight=(newNode->Zero()->weight+newNode->One()->weight);
				if(newNode->Zero()->depth>newNode->One()->depth)
				{
					newNode->depth=newNode->Zero()->depth+1;
				}
				else
				{
					newNode->depth=newNode->One()->depth+1;
				}

				nFreeNode-=2;
				if(nFreeNode==0)
				{
					nFreeNode=1;
					freeNode[0]=newNode;
				}
				else if(freeNode[nFreeNode-1]->weight>=newNode->weight)  // Smaller one to the right
				{
					freeNode[nFreeNode++]=newNode;
				}
				else if(freeNode[0]->weight<=newNode->weight)
				{
					int i;
					for(i=nFreeNode; i>0; i--)
					{
						freeNode[i]=freeNode[i-1];
					}
					freeNode[0]=newNode;
					nFreeNode++;
				}
				else // if(freeNode[0]->weight>newNode->weight && newNode->weight<freeNode[nFreeNode-1]->weight)
				{
					int i1,i2;
					i1=0;
					i2=nFreeNode-1;
					while(i2<1+i1)
					{
						unsigned int mid;
						mid=(i1+i2)/2;
						if(freeNode[mid]->weight>newNode->weight)
						{
							i1=mid;
						}
						else
						{
							i2=mid;
						}
					}

					int i;
					for(i=nFreeNode; i>i2; i--)
					{
						freeNode[i]=freeNode[i-1];
					}
					freeNode[i2]=newNode;
					nFreeNode++;
				}
			}
			root=freeNode[0];
		}

		delete [] freeNode;
	}
	return YSOK;
}

void YsPngHuffmanTreeManager::SortFreeNode(int nFreeNode,YsPngHuffmanTree **freeNode)
{
	int i,j; // I just want to have something that works.  I'm going to use quick sort later.
	for(i=0; i<nFreeNode; i++)
	{
		for(j=i+1; j<nFreeNode; j++)
		{
			if(freeNode[i]->weight<freeNode[j]->weight)
			{
				YsPngHuffmanTree *tmp;
				tmp=freeNode[i];
				freeNode[i]=freeNode[j];
				freeNode[j]=tmp;
			}
		}
	}
}

int YsPngHuffmanTreeManager::MakeCodeLengthArray(unsigned int hLength[],YsPngHuffmanTree *node,int depth) const
{
	if(NULL==node->Zero() && NULL==node->One())
	{
		hLength[node->dat]=depth;
	}
	else
	{
		if(NULL!=node->Zero())
		{
			MakeCodeLengthArray(hLength,node->Zero(),depth+1);
		}
		if(NULL!=node->One())
		{
			MakeCodeLengthArray(hLength,node->One(),depth+1);
		}
	}
	return YSOK;
}

////////////////////////////////////////////////////////////
/*
This lazy-match hash table was written according to the algorithm described in RFC 1951 Section 4.
This hash table can easily be made more efficient by using doubly-linked list and additional list
based on chronological order.  However, RFC 1951 states that deviation from the presented algorithm
may risk feeding a patent troll.

However, the observations imply that this simple and suboptimal method is not too slow after all.
*/

class YsLazyMatchHashNode
{
public:
	YsLazyMatchHashNode *next;
	unsigned int hashKey;
	unsigned int index;

	YsLazyMatchHashNode();
};

YsLazyMatchHashNode::YsLazyMatchHashNode()
{
	next=NULL;
}

class YsLazyMatchHashTable
{
protected:
	YsLazyMatchHashNode *nodeBuffer;
	YsLazyMatchHashNode **hashTable;

	YsLazyMatchHashNode *freeNode;

	int hashSize;
	int allocSize;
public:
	YsLazyMatchHashTable(int hashSize,int allocSize);
	~YsLazyMatchHashTable();

	void Add(unsigned int b0,unsigned int b1,unsigned int b2,int pointer);
	YsLazyMatchHashNode *GetHashChain(unsigned int b0,unsigned int b1,unsigned int b2);

protected:
	unsigned int ComputeHashKey(unsigned int b0,unsigned int b1,unsigned int b2);
	void DeleteOldest(void);
};

YsLazyMatchHashTable::YsLazyMatchHashTable(int hashSize,int allocSize)
{
	int i;

	this->hashSize=hashSize;
	this->allocSize=allocSize;

	hashTable=new YsLazyMatchHashNode *[hashSize];
	nodeBuffer=new YsLazyMatchHashNode[allocSize];

	freeNode=&nodeBuffer[0];
	for(i=0; i<allocSize-1; i++)
	{
		nodeBuffer[i].next=&nodeBuffer[i+1];
	}
	nodeBuffer[allocSize-1].next=NULL;

	for(i=0; i<hashSize; i++)
	{
		hashTable[i]=NULL;
	}
}

YsLazyMatchHashTable::~YsLazyMatchHashTable()
{
	delete [] nodeBuffer;
	delete [] hashTable;
}

void YsLazyMatchHashTable::Add(unsigned int b0,unsigned int b1,unsigned int b2,int index)
{
	unsigned hashKey=ComputeHashKey(b0,b1,b2);

	if(NULL==freeNode)
	{
		DeleteOldest();
	}

	YsLazyMatchHashNode *newNode=freeNode;
	if(NULL!=newNode)
	{
		freeNode=freeNode->next;

		newNode->hashKey=hashKey;
		newNode->index=index;

		newNode->next=hashTable[hashKey];
		hashTable[hashKey]=newNode;
	}
}

YsLazyMatchHashNode *YsLazyMatchHashTable::GetHashChain(unsigned int b0,unsigned int b1,unsigned int b2)
{
	unsigned int hashKey=ComputeHashKey(b0,b1,b2);
	return hashTable[hashKey];
}

unsigned int YsLazyMatchHashTable::ComputeHashKey(unsigned int b0,unsigned int b1,unsigned int b2)
{
	return ((b0<<24)+(b1<<8)+b2)%hashSize; // Easiest possible calculation I can imagine.
}

void YsLazyMatchHashTable::DeleteOldest(void)
{
	int i;
	int first=1;
	unsigned long minIndex=0,maxIndex=0,midIndex;

	for(i=0; i<hashSize; i++)
	{
		YsLazyMatchHashNode *ptr;
		for(ptr=hashTable[i]; NULL!=ptr; ptr=ptr->next)
		{
			if(0!=first)
			{
				minIndex=ptr->index;
				maxIndex=ptr->index;
				first=0;
			}
			else
			{
				minIndex=(minIndex<ptr->index ? minIndex : ptr->index);
				maxIndex=(maxIndex>ptr->index ? maxIndex : ptr->index);
			}
		}
	}


	YsLazyMatchHashNode **nextOfLastFreeNode=&freeNode;
	midIndex=(minIndex+maxIndex)/2;
	if(midIndex+32768<maxIndex)
	{
		midIndex=maxIndex-32768;
	}

	// printf("Drop older table entry\n");
	// printf("Max %d Min %d Mid %d Dist %d\n",minIndex,midIndex,maxIndex,maxIndex-midIndex);

	for(i=0; i<hashSize; i++)
	{
		YsLazyMatchHashNode *ptr,**nextOfPrev;
		nextOfPrev=&hashTable[i];
		for(ptr=hashTable[i]; NULL!=ptr; ptr=ptr->next)
		{
			if(ptr->index<midIndex)
			{
				(*nextOfPrev)=NULL;

				(*nextOfLastFreeNode)=ptr;
				while(NULL!=ptr->next)
				{
					ptr=ptr->next;
				}
				nextOfLastFreeNode=&(ptr->next);
				break;
			}
			nextOfPrev=&(ptr->next);
		}
	}
}

////////////////////////////////////////////////////////////

YsPngCompressor::YsPngCompressor()
{
	bufSize=0;
	buf=NULL;
	bufPtr=0;
	bufBit=1;
	verboseMode=YSTRUE;
}

YsPngCompressor::~YsPngCompressor()
{
	if(NULL!=buf)
	{
		delete [] buf;
		buf=NULL;
	}
}

void YsPngCompressor::SaveState(YsPngCompressorState &state)
{
	state.bufPtr=bufPtr;
	state.bufBit=bufBit;
	if(bufBit!=1)
	{
		state.lastByte=buf[bufPtr];
	}
	else
	{
		state.lastByte=0;
	}
	state.adler32_s1=adler32_s1;
	state.adler32_s2=adler32_s2;
	state.nByteReceived=nByteReceived;
}

void YsPngCompressor::RestoreState(const YsPngCompressorState &state)
{
	bufPtr=state.bufPtr;
	bufBit=state.bufBit;
	if(bufPtr<bufSize)
	{
		int i;
		buf[bufPtr]=state.lastByte;
		for(i=bufPtr+1; i<(int)bufSize; i++)
		{
			buf[i]=0;
		}
	}
	adler32_s1=state.adler32_s1;
	adler32_s2=state.adler32_s2;
	nByteReceived=state.nByteReceived;
}

int YsPngCompressor::BeginCompression(unsigned int nByte)
{
	nByteExpect=nByte;
	nByteReceived=0;


	unsigned int windowSizeExp;
	unsigned int utilBit;

	utilBit=256;
	windowSizeExp=8;
	while(windowSizeExp<15 && utilBit<nByte)
	{
		windowSizeExp++;
		utilBit*=2;
	}
	windowSize=(1<<windowSizeExp);

	if(YSTRUE==verboseMode)
	{
		printf("Compression Window Size=%d\n",windowSize);
	}

	unsigned int CMF;
	CMF=((windowSizeExp-8)<<4)+8;

	unsigned int FLG=0,FCHECK,FDICT,FLEVEL;
	FDICT=0;  // PNG does not support preset dictionary.
	FLEVEL=2; // Default algorithm, I suppose so...

	for(FCHECK=0; FCHECK<32; FCHECK++)
	{
		FLG=FCHECK+(FDICT<<5)+(FLEVEL<<6);
		if((CMF*256+FLG)%31==0)
		{
			break;
		}
	}
	if(FCHECK>=32)
	{
		printf("Unable to make legitimate FCHECK value.\n");
		return YSERR;
	}

	AddCMFandFLG(CMF,FLG);

	BeginAdler32();

	return YSOK;
}

int YsPngCompressor::AddCompressionBlock(unsigned int nByte,unsigned char byteData[],int bFinal)
{
	int i;
	unsigned int nCode=0;
	unsigned int *codeArray=new unsigned int [nByte+1];
	unsigned int *copyParamArray=new unsigned int [nByte+1];  // <- High16bit:Copy Length  Low16bit:Back dist

	for(i=0; i<(int)nByte+1; i++)
	{
		copyParamArray[i]=0xffffffff;
	}

	// EncodeWithNoRepetitionReduction(nCode,codeArray,copyParamArray,nByte,byteData);
	// EncodeWithDumbestRepetitionReduction(nCode,codeArray,copyParamArray,nByte,byteData);
	EncodeWithLazyMatchAsDescribedInRFC1951(nCode,codeArray,copyParamArray,nByte,byteData);



	// Making code for data
	int hLit;
	unsigned int hLenLit[286];
	if(MakeLengthLiteral(hLit,hLenLit,nCode,codeArray)!=YSOK)
	{
		delete [] codeArray;
		delete [] copyParamArray;
		return YSERR;
	}
	unsigned int hLitCode[286];
	YsPngUncompressor::MakeDynamicHuffmanCode(hLenLit,hLitCode,hLit,hLenLit);
	InvertHuffmanCodeForWriting(hLit,hLenLit,hLitCode);



	// Making code for backward distance
	const unsigned int distLen=30;
	int hDist;
	unsigned int hLenDist[distLen];
	if(MakeLengthBackDist(hDist,hLenDist,nCode,copyParamArray)!=YSOK)
	{
		delete [] codeArray;
		delete [] copyParamArray;
		return YSERR;
	}
	unsigned int hCodeDist[distLen];
	YsPngUncompressor::MakeDynamicHuffmanCode(hLenDist,hCodeDist,hDist,hLenDist);
	InvertHuffmanCodeForWriting(hDist,hLenDist,hCodeDist);

	if(YSTRUE==verboseMode)
	{
		for(i=0; i<distLen; i++)
		{
			printf("DistLen[%3d]=%d  HuffmanCode=%08x\n",i,hLenDist[i],hCodeDist[i]);
		}
	}



	// Making code for data length
	int hCLen;
	const unsigned int codeLengthLen=19;
	unsigned int hLenCodeLen[codeLengthLen];
	if(MakeLengthCodeLength(hCLen,hLenCodeLen,hLit,hLenLit,hDist,hLenDist)!=YSOK)
	{
		delete [] codeArray;
		delete [] copyParamArray;
		return YSERR;
	}
	unsigned int hLenCode[codeLengthLen];  // Hell confusing!  
	YsPngUncompressor::MakeDynamicHuffmanCode(hLenCodeLen,hLenCode,hCLen,hLenCodeLen);
	InvertHuffmanCodeForWriting(hCLen,hLenCodeLen,hLenCode);

	if(YSTRUE==verboseMode)
	{
		for(i=0; i<codeLengthLen; i++)
		{
			printf("CodeLengthLen[%3d]=%d  HuffmanCode=%08x\n",i,hLenCodeLen[i],hLenCode[i]);
		}
	}





	// Flags
	unsigned int flags;
	flags=4+bFinal;  // 10x: 10=Dynamic Huffman code   x=bFinal
	WriteMultiBit(3,flags);
	WriteMultiBit(5,hLit-257);
	WriteMultiBit(5,hDist-1);
	WriteMultiBit(4,hCLen-4);

	unsigned codeLengthOrder[codeLengthLen]=
	{
		16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15
	};
	for(i=0; i<hCLen; i++)
	{
		WriteMultiBit(3,hLenCodeLen[codeLengthOrder[i]]);
	}



	for(i=0; i<hLit; i++)
	{
		unsigned int bitLenOfLiteral;
		unsigned int bitLenOfLenCode;
		unsigned int lenCode;

		bitLenOfLiteral=hLenLit[i];                    // This should be 0 to 15.
		bitLenOfLenCode=hLenCodeLen[bitLenOfLiteral];  // This should be 0 to 7
		lenCode=hLenCode[bitLenOfLiteral];

		WriteMultiBit(bitLenOfLenCode,lenCode);
	}



	for(i=0; i<hDist; i++)
	{
		unsigned int bitLenOfLiteral;
		unsigned int bitLenOfLenCode;
		unsigned int lenCode;

		bitLenOfLiteral=hLenDist[i];                   // This should be 0 to 5
		bitLenOfLenCode=hLenCodeLen[bitLenOfLiteral];  // This should be 0 to 7
		lenCode=hLenCode[bitLenOfLiteral];

		WriteMultiBit(bitLenOfLenCode,lenCode);
	}



	for(i=0; i<(int)nCode; i++)
	{
		if(codeArray[i]<=256)
		{
			WriteMultiBit(hLenLit[codeArray[i]],hLitCode[codeArray[i]]);
		}
		else
		{
			WriteMultiBit(hLenLit[codeArray[i]],hLitCode[codeArray[i]]);

			unsigned int copyLength=(copyParamArray[i]>>16)&0xffff;
			unsigned int backDist=copyParamArray[i]&0xffff;
			unsigned int nExtBit,extBit;

			unsigned int copyCode;
			GetCopyCodeAndExtraBit(copyCode,nExtBit,extBit,copyLength);
			if(copyCode!=codeArray[i])
			{
				printf("!Internal error! Copy code doesn't match the copy length.\n");
				delete [] codeArray;
				delete [] copyParamArray;
				return YSERR;
			}
			WriteMultiBit(nExtBit,extBit);

			unsigned int distCode;
			GetDistCodeAndExtraBit(distCode,nExtBit,extBit,backDist);

			WriteMultiBit(hLenDist[distCode],hCodeDist[distCode]);
			WriteMultiBit(nExtBit,extBit);
		}
	}

	for(i=0; i<(int)nByte; i++)
	{
		AddAdler32(byteData[i]);
		nByteReceived++;
	}

	delete [] codeArray;
	delete [] copyParamArray;
	return YSOK;
}

// BackDist must be 1 to 32768
int YsPngCompressor::GetDistCodeAndExtraBit(unsigned int &distCode,unsigned int &nExtraBit,unsigned int &distExtraBit,unsigned int backDist) const
{
	// See RFC 1951 3.2.5  => Table directly above 3.2.6
	if(backDist==0)
	{
		return YSERR;
	}
	else if(backDist<=4)
	{
		distCode=backDist-1;
		nExtraBit=0;
		distExtraBit=0;
	}
	else if(backDist<=8)
	{
		distCode=4+(backDist-5)/2;
		nExtraBit=1;
		distExtraBit=(backDist-5)&1;
	}
	else if(backDist<=16)
	{
		distCode=6+(backDist-9)/4;
		nExtraBit=2;
		distExtraBit=(backDist-9)&3;
	}
	else if(backDist<=32)
	{
		distCode=8+(backDist-17)/8;
		nExtraBit=3;
		distExtraBit=(backDist-17)&7;
	}
	else if(backDist<=64)
	{
		distCode=10+(backDist-33)/16;
		nExtraBit=4;
		distExtraBit=(backDist-33)&15;
	}
	else if(backDist<=128)
	{
		distCode=12+(backDist-65)/32;
		nExtraBit=5;
		distExtraBit=(backDist-65)&31;
	}
	else if(backDist<=256)
	{
		distCode=14+(backDist-129)/64;
		nExtraBit=6;
		distExtraBit=(backDist-129)&63;
	}
	else if(backDist<=512)
	{
		distCode=16+(backDist-257)/128;
		nExtraBit=7;
		distExtraBit=(backDist-257)&127;
	}
	else if(backDist<=1024)
	{
		distCode=18+(backDist-513)/256;
		nExtraBit=8;
		distExtraBit=(backDist-513)&255;
	}
	else if(backDist<=2048)
	{
		distCode=20+(backDist-1025)/512;
		nExtraBit=9;
		distExtraBit=(backDist-1025)&511;
	}
	else if(backDist<=4096)
	{
		distCode=22+(backDist-2049)/1024;
		nExtraBit=10;
		distExtraBit=(backDist-2049)&1023;
	}
	else if(backDist<=8192)
	{
		distCode=24+(backDist-4097)/2048;
		nExtraBit=11;
		distExtraBit=(backDist-4097)&2047;
	}
	else if(backDist<=16384)
	{
		distCode=26+(backDist-8193)/4096;
		nExtraBit=12;
		distExtraBit=(backDist-8193)&4095;
	}
	else if(backDist<=32768)
	{
		distCode=28+(backDist-16385)/8192;
		nExtraBit=13;
		distExtraBit=(backDist-16385)&8191;
	}
	else
	{
		return YSERR;
	}
	return YSOK;
}

// Copy length must be 0 to 258
int YsPngCompressor::GetCopyCodeAndExtraBit(unsigned int &copyCode,unsigned int &nExtraBit,unsigned int &extraBit,unsigned int copyLength) const
{
	if(copyLength<=10)
	{
		copyCode=257+(copyLength-3);
		nExtraBit=0;
		extraBit=0;
	}
	else if(copyLength<=18)
	{
		copyCode=265+(copyLength-11)/2;
		nExtraBit=1;
		extraBit=(copyLength-11)&1;
	}
	else if(copyLength<=34)
	{
		copyCode=269+(copyLength-19)/4;
		nExtraBit=2;
		extraBit=(copyLength-19)&3;
	}
	else if(copyLength<=66)
	{
		copyCode=273+(copyLength-35)/8;
		nExtraBit=3;
		extraBit=(copyLength-35)&7;
	}
	else if(copyLength<=130)
	{
		copyCode=277+(copyLength-67)/16;
		nExtraBit=4;
		extraBit=(copyLength-67)&15;
	}
	else if(copyLength<=257)
	{
		copyCode=281+(copyLength-131)/32;
		nExtraBit=5;
		extraBit=(copyLength-131)&31;
	}
	else if(copyLength==258)
	{
		copyCode=285;
		nExtraBit=0;
		extraBit=0;
	}
	else
	{
		return YSERR;
	}
	return YSOK;
}

void YsPngCompressor::EncodeWithNoRepetitionReduction(
    unsigned int &nCode,unsigned int codeArray[],unsigned int copyParamArray[],
    unsigned int nByte,const unsigned char byteData[]) const
{
	int i;

	nCode=nByte+1;

	for(i=0; i<(int)nByte; i++)
	{
		codeArray[i]=byteData[i];
	}
	codeArray[nByte]=256;  // Terminator
}

void YsPngCompressor::EncodeWithDumbestRepetitionReduction(
    unsigned int &nCode,unsigned int codeArray[],unsigned int copyParamArray[],
    unsigned int nByte,const unsigned char byteData[]) const
{
	if(nByte<8)
	{
		EncodeWithNoRepetitionReduction(nCode,codeArray,copyParamArray,nByte,byteData);
		return;
	}

	unsigned int maxBackDist=0,maxCopyLength=0;
	int i,j,k;
	int n=0;
	for(i=0; i<4; i++)
	{
		codeArray[n]=byteData[i];
		n++;
	}

	for(i=i; i<(int)nByte-4; i++)
	{
		int range0;
		range0=i-512;  // Limit up to XX bytes search
		               // Theoretically, it can be up to 32768
		               // But, the exhaustive (dumbest) search will take forever.
		if(range0<0)
		{
			range0=0;
		}

		int matchStart,nMatch;
		matchStart=0;
		nMatch=0;
		for(j=range0; j<=i-4; j++)
		{
			if(byteData[j]==byteData[i])
			{
				for(k=0; k<258 && j+k<i && i+k<(int)nByte && byteData[j+k]==byteData[i+k]; k++)
				{
				}

				if(k>=3 && k>nMatch)
				{
					matchStart=j;
					nMatch=k;
				}
			}
		}

		if(nMatch>=3)
		{
			unsigned int copyCode,nExtBit,extBit;
			unsigned int backDist=(unsigned int)(i-matchStart);

			GetCopyCodeAndExtraBit(copyCode,nExtBit,extBit,nMatch);

			codeArray[n]=copyCode;
			copyParamArray[n]=(nMatch<<16)|backDist;
			n++;
			i+=(nMatch-1);

			maxBackDist=(maxBackDist<backDist ? backDist : maxBackDist);
			maxCopyLength=(maxCopyLength<(unsigned int)nMatch ? nMatch : maxCopyLength);
		}
		else
		{
			codeArray[n]=byteData[i];
			n++;
		}
	}

	for(i=i; i<(int)nByte; i++)
	{
		codeArray[n]=byteData[i];
		n++;
	}

	if(YSTRUE==verboseMode)
	{
		printf("Max Back Dist=%d\n",maxBackDist);
		printf("Max Copy Length=%d\n",maxCopyLength);
	}

	codeArray[n++]=256; // Terminator
	nCode=n;
}

void YsPngCompressor::EncodeWithLazyMatchAsDescribedInRFC1951(
    unsigned int &nCode,unsigned int codeArray[],unsigned int copyParamArray[],
    unsigned int nByte,const unsigned char byteData[]) const
{
	YsLazyMatchHashTable hashTable(65521,32768);

	int i,j;
	i=0;
	nCode=0;
	while(i<(int)nByte)
	{
		if(i+3<(int)nByte)
		{
			YsLazyMatchHashNode *hashChain;
			hashChain=hashTable.GetHashChain(byteData[i],byteData[i+1],byteData[i+2]);
			int copyLength=0;
			int copyIndex=-1;
			while(NULL!=hashChain)
			{
				if((int)(hashChain->index+32768)>=i)  // Otherwise out of range from 32K window
				{
					for(j=0; j<258 && (int)(hashChain->index)+j<i && i+j<(int)nByte && byteData[hashChain->index+j]==byteData[i+j]; j++)
					{
					}

					if(j>=3 && j>copyLength)
					{
						copyLength=j;
						copyIndex=hashChain->index;
					}
				}

				hashChain=hashChain->next;
			}

			if(copyIndex<0) // Not found?
			{
				hashTable.Add(byteData[i],byteData[i+1],byteData[i+2],i);
				codeArray[nCode++]=byteData[i++];
			}
			else
			{
				unsigned int copyCode,nExtBit,extBit;
				unsigned int backDist=(unsigned int)(i-copyIndex);

				GetCopyCodeAndExtraBit(copyCode,nExtBit,extBit,copyLength);

				codeArray[nCode]=copyCode;
				copyParamArray[nCode]=(copyLength<<16)|backDist;
				nCode++;
				i+=copyLength;
			}
		}
		else
		{
			codeArray[nCode++]=byteData[i++];
		}
	}

	codeArray[nCode++]=256; // Terminator
}

int YsPngCompressor::MakeLengthLiteral(int &hLit,unsigned int hLenLit[],int nCode,unsigned int codeArray[]) const
{
	int i;
	YsPngHuffmanTreeManager litTreeManager;

	litTreeManager.MakeTreeFromData(nCode,codeArray,0,NULL,285);  // zLib code(literal?) could be up to 285.
	if(litTreeManager.GetTreeDepth()>15)
	{
		int i;
		printf("Code Tree depth exceeds maximum allowed... %d\n",litTreeManager.GetTreeDepth());
		for(i=0; i<32 && litTreeManager.GetTreeDepth()>15; i++)
		{
			litTreeManager.ReduceTreeDepth();
			printf("Reducing Code Tree Depth... %d\n",litTreeManager.GetTreeDepth());
		}
	}
	if(litTreeManager.GetTreeDepth()>15)
	{
		printf("Cannot bring the code tree depth below maximum %d\n",litTreeManager.GetTreeDepth());
		return YSERR;
	}

	litTreeManager.MakeCodeLengthArray(hLenLit);

	if(YSTRUE==verboseMode)
	{
		litTreeManager.PrintInfo();
		for(i=0; i<286; i++)
		{
			printf("LiteralLength[%3d]=%d\n",i,hLenLit[i]);
		}
	}
	for(hLit=286; hLit>0; hLit--)
	{
		if(hLenLit[hLit-1]!=0)
		{
			break;
		}
	}

	return YSOK;
}

int YsPngCompressor::MakeLengthCodeLength(
    int &hCLen,unsigned int hLenCodeLen[],
    int nLenCode,unsigned int lenCode[],int nLenDistCode,unsigned int lenDistCode[]) const
{

	const unsigned int codeLengthLen=19;
	// CodeLength=16,17,18 are for copying previous values.
	// Not used in this encoder at this time.

	YsPngHuffmanTreeManager lenTreeManager;
	lenTreeManager.MakeTreeFromData(nLenCode,lenCode,nLenDistCode,lenDistCode,codeLengthLen-1);
	//  lenCode[n] <= Bit length of the code.  If lenCode[0] is 10, Code 0 will take 10 bits.
	//  Problem encountered:  If all code has at least one bit assigned, there is no way of specifying 
	//                        0 length for distance table.
	//             Solution:  Take nDistCode and nDistCode[] as additional parameters.

	if(lenTreeManager.GetTreeDepth()>7)  // 3bit each->Max 7
	{
		int i;
		printf("Code-Length Tree depth exceeds maximum allowed... %d\n",lenTreeManager.GetTreeDepth());
		for(i=0; i<32 && lenTreeManager.GetTreeDepth()>7; i++)
		{
			lenTreeManager.ReduceTreeDepth();
			printf("Reducing Code-Length Tree Depth... %d\n",lenTreeManager.GetTreeDepth());
		}
	}
	if(lenTreeManager.GetTreeDepth()>7)
	{
		printf("Cannot bring the Code-Length tree depth below maximum %d\n",lenTreeManager.GetTreeDepth());
		return YSERR;
	}

	lenTreeManager.MakeCodeLengthArray(hLenCodeLen);
	hCLen=codeLengthLen;  // Order is twisted.  Let's write everything.
	return YSOK;
}

int YsPngCompressor::MakeLengthBackDist(int &hDist,unsigned int hLenDist[],int nCode,unsigned int copyParamArray[]) const
{
	const unsigned int distLen=30;  // <- It is a length for frequency table.  Therefore, distCode can take 0 to 29.
	int i;
	unsigned int distFreq[distLen],totalOccurence=0;

	for(i=0; i<distLen; i++)
	{
		distFreq[i]=0;
	}
	totalOccurence=0;
	for(i=0; i<nCode; i++)
	{
		unsigned int backDist;
		backDist=copyParamArray[i]&0xffff;
		if(backDist!=0xffff)
		{
			unsigned int backDistCode,nExtBit,extBit;
			GetDistCodeAndExtraBit(backDistCode,nExtBit,extBit,backDist);

			if(backDistCode>=distLen)
			{
				return YSERR;
			}
			distFreq[backDistCode]++;
			totalOccurence++;
		}
	}


	if(0==totalOccurence)
	{
		if(YSTRUE==verboseMode)
		{
			printf("hDistLen cannot be zero.  It needs to be at least one.\n");
			printf("Making up a dummy backdist.\n");
		}
		hDist=1;
		hLenDist[0]=0;
		return YSOK;
	}


	YsPngHuffmanTreeManager treeManager;
	treeManager.MakeTreeFromFrequencyTable(distLen,distFreq);

	if(treeManager.GetTreeDepth()>15)
	{
		int i;
		printf("Backdist Tree depth exceeds maximum allowed... %d\n",treeManager.GetTreeDepth());
		for(i=0; i<32 && treeManager.GetTreeDepth()>15; i++)
		{
			treeManager.ReduceTreeDepth();
			printf("Reducing Backdist Tree Depth... %d\n",treeManager.GetTreeDepth());
		}
	}
	if(treeManager.GetTreeDepth()>15)
	{
		printf("Cannot bring the backdist tree depth below maximum %d\n",treeManager.GetTreeDepth());
		return YSERR;
	}

	treeManager.MakeCodeLengthArray(hLenDist);

	if(YSTRUE==verboseMode)
	{
		treeManager.PrintInfo();
		for(i=0; i<distLen; i++)
		{
			printf("BackdistLength[%3d]=%d\n",i,hLenDist[i]);
		}
	}
	for(hDist=distLen; hDist>0; hDist--)
	{
		if(hLenDist[hDist-1]!=0)
		{
			break;
		}
	}

	if(YSTRUE==verboseMode)
	{
		printf("hDist=%d\n",hDist);
	}

	return YSOK;
}

// nByte needs to be less than 32768
int YsPngCompressor::AddNonCompressionBlock(unsigned int nByte,unsigned char byteData[],int bFinal)
{
	nByteReceived+=nByte;

	if(YSTRUE==verboseMode)
	{
		printf("zLib Block nByte=%d bFinal=%d\n",nByte,bFinal);
	}

	// Break the data into blocks and write block by block
	// First 3 bits of the block:
	//   Final Flag (The last block of all)
	//   00  No Compression
	//   01  Compression with fixed Huffman codes
	//   10  Compression with dynamic Huffman codes

	// For testing, write as uncompressed
	bFinal&=~6;
	WriteMultiBit(3,bFinal);
	FlushByte();

	unsigned lenByte[4];

	// Caution!  Byte order seems to be Intel-type byte order within zLib territory.  (Motorola type in PNG territory.)
	lenByte[0]=nByte&255;
	lenByte[1]=(nByte>>8)&255;
	lenByte[2]=~lenByte[0];  // <- I wish whoever developed zLib decided to make use of these two byte to make
	lenByte[3]=~lenByte[1];  //    an uncompressed block of greater than 64KB.

	WriteByte(lenByte[0]);
	WriteByte(lenByte[1]);
	WriteByte(lenByte[2]);
	WriteByte(lenByte[3]);

	if(YSTRUE==verboseMode)
	{
		printf("BufPtr=%d BufBit=%d\n",bufPtr,bufBit);
	}

	int j;
	for(j=0; j<(int)nByte; j++)
	{
		AddAdler32(byteData[j]);
		WriteByte(byteData[j]);
	}

	return YSOK;
}

int YsPngCompressor::EndCompression(void)
{
	unsigned int adler32;
	adler32=GetAdler32();

	if(YSTRUE==verboseMode)
	{
		printf("Check Sum=%08x\n",adler32);
		printf("Received=%d Expected=%d\n",nByteReceived,nByteExpect);
	}

	WriteByte((adler32>>24)&255);
	WriteByte((adler32>>16)&255);
	WriteByte((adler32>>8 )&255);
	WriteByte( adler32     &255);

	return YSOK;
}

unsigned int YsPngCompressor::GetCompressedLength(void) const
{
	if(bufBit==1)
	{
		return bufPtr;
	}
	else
	{
		return bufPtr+1;
	}
}

const unsigned char *YsPngCompressor::GetCompressedData(void) const
{
	return buf;
}

void YsPngCompressor::ClearBuffer(void)
{
	if(NULL!=buf)
	{
		delete [] buf;
		buf=NULL;
	}
	bufSize=0;
	bufPtr=0;
	bufBit=1;
}

void YsPngCompressor::AddCMFandFLG(unsigned int CMF,unsigned int FLG)
{
	WriteByte(CMF);
	WriteByte(FLG);
}

void YsPngCompressor::TestAndGrowBuffer(void)
{
	if(bufSize<=bufPtr)
	{
		int i;
		unsigned int newBufSize;

		if(0==bufSize)
		{
			newBufSize=256;
		}
		else
		{
			newBufSize=bufSize*2;
		}

		if(YSTRUE==verboseMode)
		{
			printf("Buffer grows from %d to %d\n",bufSize,newBufSize);
		}

		unsigned char *newBuf;
		newBuf=new unsigned char [newBufSize];
		for(i=0; i<(int)bufSize; i++)
		{
			newBuf[i]=buf[i];
		}
		for(i=bufSize; i<(int)newBufSize; i++)
		{
			newBuf[i]=0;
		}

		if(NULL!=buf)
		{
			delete [] buf;
			buf=NULL;
		}

		buf=newBuf;
		bufSize=newBufSize;
	}
}

void YsPngCompressor::InvertHuffmanCodeForWriting(int nCode,unsigned int bitLength[],unsigned int code[]) const
{
	int i;
	for(i=0; i<nCode; i++)
	{
		unsigned int compareBit,orBit;
		unsigned int inverted;

		inverted=0;
		compareBit=1<<(bitLength[i]-1);
		orBit=1;

		while(compareBit>0)
		{
			if(code[i]&compareBit)
			{
				inverted|=orBit;
			}
			compareBit>>=1;
			orBit<<=1;
		}

		code[i]=inverted;
	}
}

void YsPngCompressor::BeginAdler32(void)
{
	adler32_s1=1;
	adler32_s2=0;
}

void YsPngCompressor::AddAdler32(unsigned char byteData)
{
	const unsigned int magicNumber=65521;
	adler32_s1=(adler32_s1+byteData  )%magicNumber;
	adler32_s2=(adler32_s2+adler32_s1)%magicNumber;
}

unsigned int YsPngCompressor::GetAdler32(void)
{
	return adler32_s2*0x10000+adler32_s1;
}






static inline void PngSetUnsignedInt(unsigned char dat[],unsigned int value)
{
	dat[0]=(value>>24)&255;
	dat[1]=(value>>16)&255;
	dat[2]=(value>>8)&255;
	dat[3]=value&255;
}

YsGenericPngEncoder::YsGenericPngEncoder()
{
	verboseMode=YSTRUE;
	dontCompress=YSFALSE;
}

int YsGenericPngEncoder::StreamOut(int nByte,const unsigned char byteData[]) const
{
	return YSOK;
}

void YsGenericPngEncoder::WritePngSignature(void)
{
	const unsigned char pngSignature[8]={0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a};
	StreamOut(8,pngSignature);
}

void YsGenericPngEncoder::CalculateChunkCRC(unsigned char chunk[])
{
	unsigned long chunkSize;
	chunkSize=(chunk[0]<<24)+(chunk[1]<<16)+(chunk[2]<<8)+chunk[3];

	YsPngCRCCalculator crcCalc;
	crcCalc.Initialize();
	int i;
	for(i=0; i<(int)chunkSize+4; i++)
	{
		crcCalc.Add(chunk[4+i]);
	}

	unsigned long crcCode=crcCalc.GetCRC();
	PngSetUnsignedInt(chunk+8+chunkSize,(unsigned int)crcCode);
}

void YsGenericPngEncoder::MakeIHDRChunk(int &nByte,unsigned char chunk[4+4+13+4],int width,int height,int bitDepth,int colorType)
{
	PngSetUnsignedInt(chunk+0,13);
	chunk[4]='I';
	chunk[5]='H';
	chunk[6]='D';
	chunk[7]='R';

	unsigned char *dataField=chunk+8;
	PngSetUnsignedInt(dataField+0,width);
	PngSetUnsignedInt(dataField+4,height);
	dataField[ 8]=(unsigned char)bitDepth;
	dataField[ 9]=(unsigned char)colorType;
	dataField[10]=0; /* Compression Method: Deflate/Inflate with a 32K sliding window*/
	dataField[11]=0; /* Filter Method: Only zero allowed at present */
	dataField[12]=0; /* Interlace Method: No interlace */

	chunk[4+4+13]=0;
	chunk[4+4+14]=0;
	chunk[4+4+15]=0;
	chunk[4+4+16]=0;

	nByte=4+4+13+4;

	CalculateChunkCRC(chunk);
}

int YsGenericPngEncoder::WriteIHDRChunk(int width,int height,int bitDepth,int colorType)
{
	int nByte;
	unsigned char chunk[4+4+13+4];
	MakeIHDRChunk(nByte,chunk,width,height,bitDepth,colorType);
	return StreamOut(nByte,chunk);
}

void YsGenericPngEncoder::MakeIENDChunk(int &nByte,unsigned char chunk[4+4+4])
{
	PngSetUnsignedInt(chunk+0,0);
	chunk[4]='I';
	chunk[5]='E';
	chunk[6]='N';
	chunk[7]='D';
	chunk[8]=0;
	chunk[9]=0;
	chunk[10]=0;
	chunk[11]=0;

	nByte=12;

	CalculateChunkCRC(chunk);
}

int YsGenericPngEncoder::WriteIENDChunk(void)
{
	int nByte;
	unsigned char chunk[4+4+4];
	MakeIENDChunk(nByte,chunk);
	return StreamOut(nByte,chunk);
}

int YsGenericPngEncoder::WritetEXtChunk(const char keyword[],const char text[])
{
	unsigned char *chunk;

	int nKeyword,nText,nChunk;
	nKeyword=(int)strlen(keyword);
	if(nKeyword>79)
	{
		nKeyword=79;
	}

	nText=(int)strlen(text);

	nChunk=4+4+nKeyword+1+nText+4;


	chunk=new unsigned char [nChunk];

	if(NULL!=chunk)
	{
		int res;

		PngSetUnsignedInt(chunk,nKeyword+1+nText);
		chunk[4]='t';
		chunk[5]='E';
		chunk[6]='X';
		chunk[7]='t';

		unsigned char *dataField=chunk+8;
		strncpy((char *)dataField,keyword,nKeyword);

		dataField[nKeyword]=0;

		strncpy((char *)dataField+nKeyword+1,text,nText);

		dataField[nKeyword+1+nText  ]=0;
		dataField[nKeyword+1+nText+1]=0;
		dataField[nKeyword+1+nText+2]=0;
		dataField[nKeyword+1+nText+3]=0;

		CalculateChunkCRC(chunk);

		res=StreamOut(nChunk,chunk);

		delete [] chunk;

		return YSOK;
	}

	return YSERR;
}

int YsGenericPngEncoder::WriteIDATChunk(unsigned int nLine,unsigned int bytePerLine,const unsigned char dat[])
{
	YsPngCompressor compressor;
	unsigned char *chunk;

	unsigned int totalRawPixelByte;
	totalRawPixelByte=nLine*(bytePerLine+1); // Each line has one extra byte of filter.  Therefore one must be added.

	compressor.BeginCompression(totalRawPixelByte);

	const unsigned int nLinePerUnit=256;

	if(verboseMode==YSTRUE)
	{
		printf("nLinePerUnit=%d\n",nLinePerUnit);
	}

	unsigned char *zLibBlock;
	zLibBlock=new unsigned char [(bytePerLine+1)*nLinePerUnit];

	int y;
	for(y=0; y<(int)nLine; y+=nLinePerUnit)
	{
		if(verboseMode==YSTRUE)
		{
			printf("Y=%d\n",y);
		}

		int x,yInBlock,nLineInBlock;
		unsigned int finalUnit;
		if(y+nLinePerUnit<nLine)
		{
			nLineInBlock=nLinePerUnit;
			finalUnit=0;
		}
		else
		{
			nLineInBlock=nLine-y;
			finalUnit=1;
		}

		for(yInBlock=0; yInBlock<nLineInBlock; yInBlock++)
		{
			int headPtrDst,headPtrSrc;
			headPtrSrc=(y+yInBlock)*bytePerLine;
			headPtrDst=yInBlock*(bytePerLine+1);

			zLibBlock[headPtrDst]=0;  // No Filter.  See section 9 of PNG documentation.

			for(x=0; x<(int)bytePerLine; x++)
			{
				zLibBlock[headPtrDst+1+x]=dat[headPtrSrc+x];
			}
		}



		unsigned int totalByte=nLineInBlock*(bytePerLine+1);
		unsigned int compressedLength0=compressor.GetCompressedLength();

		YsPngCompressorState state;
		compressor.SaveState(state);
		if(YSTRUE==dontCompress ||
		   compressor.AddCompressionBlock(totalByte,zLibBlock,finalUnit)!=YSOK ||  // Compression failed?
		   (compressor.GetCompressedLength()-compressedLength0)>totalByte*105/100) // Size increased by 5%?
		{
			compressor.RestoreState(state);

			unsigned int k;
			const int maxBytePerBlock=32768; // Uncompressed block cannot be larger than 65535

			for(k=0; k<totalByte; k+=maxBytePerBlock)
			{
				unsigned int blockSize;
				unsigned int bFinal;
				if(k+maxBytePerBlock<totalByte)
				{
					blockSize=maxBytePerBlock;
					bFinal=0;
				}
				else
				{
					blockSize=totalByte-k;
					bFinal=finalUnit;
				}
				compressor.AddNonCompressionBlock(blockSize,zLibBlock+k,bFinal);
			}
		}
	}

	delete [] zLibBlock;

	compressor.EndCompression();


	/* if(YSTRUE==save zlib format binary)
	{
		printf("For Test Writing c:/tmp/compressed.dat\n");
		FILE *fp;
		fp=fopen("c:/tmp/compressed.dat","wb");
		if(fp!=NULL)
		{
			fwrite(compressor.GetCompressedData(),1,compressor.GetCompressedLength(),fp);
			fclose(fp);
		}
	} */


	const unsigned long chunkDatSize=compressor.GetCompressedLength();

	chunk=new unsigned char [chunkDatSize+12];
	if(NULL!=chunk)
	{
		PngSetUnsignedInt(chunk,(unsigned int)chunkDatSize);
		chunk[4]='I';
		chunk[5]='D';
		chunk[6]='A';
		chunk[7]='T';

		int i;
		for(i=0; i<(int)chunkDatSize; i++)
		{
			chunk[8+i]=compressor.GetCompressedData()[i];
		}

		chunk[8+chunkDatSize  ]=0;
		chunk[8+chunkDatSize+1]=0;
		chunk[8+chunkDatSize+2]=0;
		chunk[8+chunkDatSize+3]=0;

		CalculateChunkCRC(chunk);

		StreamOut(12+(int)chunkDatSize,chunk);

		delete [] chunk;
		return YSOK;
	}
	return YSERR;
}

unsigned int YsGenericPngEncoder::CalculateBytePerLine(int width,int bitDepth,int colorType)
{
	switch(colorType)
	{
	case 0:   // Greyscale
		switch(bitDepth)
		{
		case 1:
			return (width+7)/8;
			break;
		case 2:
			return (width+3)/4;
			break;
		case 4:
			return (width+1)/2;
			break;
		case 8:
			return width;
			break;
		case 16:
			return width*2;
			break;
		}
		break;
	case 2:   // Truecolor
		switch(bitDepth)
		{
		case 8:
			return width*3;
			break;
		case 16:
			return width*6;
			break;
		}
		break;
	case 3:   // Indexed-color
		/* switch(bitDepth)
		{
		case 1:
			return (width+7)/8;
			break;
		case 2:
			return (width+3)/4;
			break;
		case 4:
			return (width+1)/2;
			break;
		case 8:
			return width;
			break;
		} */
		return 0;  // Unsupported
		break;
	case 4:   // Greyscale with alpha
		switch(bitDepth)
		{
		case 8:
			return width*2;
			break;
		case 16:
			return width*4;
			break;
		}
		break;
	case 6:   // Truecolor with alpha
		switch(bitDepth)
		{
		case 8:
			return width*4;
			break;
		case 16:
			return width*8;
			break;
		}
		break;
	}
	return 0;
}

int YsGenericPngEncoder::Encode(int width,int height,int bitDepth,int colorType,const unsigned char dat[])
{
	unsigned int totalByte,bytePerLine;
	bytePerLine=CalculateBytePerLine(width,bitDepth,colorType);
	if(bytePerLine==0)
	{
		printf("Unsupported color type and/or bitDepth\n");
		return YSERR;
	}
	totalByte=bytePerLine*height;


	WritePngSignature();
	if(WriteIHDRChunk(width,height,bitDepth,colorType)!=YSOK)
	{
		return YSERR;
	}

	WriteIDATChunk(height,bytePerLine,dat);

	WritetEXtChunk("Comment","YS PNG Encoder (http://www.ysflight.com)");
	WriteIENDChunk();

	return YSOK;
}

void YsGenericPngEncoder::SetDontCompress(int dontCompress)
{
	this->dontCompress=dontCompress;
}

////////////////////////////////////////////////////////////


int YsRawPngEncoder::StreamOut(int nByte,const unsigned char byteData[]) const
{
	fwrite(byteData,1,nByte,fp);
	return YSOK;
}

int YsRawPngEncoder::EncodeToFile(const char fn[],int width,int height,int bitDepth,int colorType,const unsigned char dat[])
{
	fp=fopen(fn,"wb");
	if(NULL!=fp)
	{
		int res=Encode(width,height,bitDepth,colorType,dat);
		fclose(fp);
		return res;
	}
	return YSERR;
}

int YsRawPngEncoder::EncodeToFile(FILE *fp,int width,int height,int bitDepth,int colorType,const unsigned char dat[])
{
	if(NULL!=fp)
	{
		this->fp=fp;
		return Encode(width,height,bitDepth,colorType,dat);
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

/* virtual */ int YsMemoryPngEncoder::StreamOut(int nIncoming,const unsigned char incoming[]) const
{
	const unsigned long long nRequired=nByte+nIncoming;
	unsigned long long nAllocNew=nAlloc;

	if(0==nAllocNew)
	{
		nAllocNew=1;
	}
	while(nAllocNew<nRequired)
	{
		nAllocNew*=2;
	}

	if(nAlloc!=nAllocNew)
	{
		unsigned char *byteNew=new unsigned char [(int)nAllocNew];
		// memcpy(byteNew,byte,nByte);
		for(long long int i=0; i<nByte; ++i)
		{
			byteNew[i]=byte[i];
		}
		delete [] byte;
		byte=byteNew;
		nAlloc=nAllocNew;
	}

	// memcpy(byte+nByte,incomingData,nIncoming);
	for(long long int i=0; i<nIncoming; ++i)
	{
		byte[nByte+i]=incoming[i];
	}
	nByte+=nIncoming;

	return YSOK;
}

YsMemoryPngEncoder::YsMemoryPngEncoder()
{
	nByte=0;
	nAlloc=0;
	byte=NULL;
}

YsMemoryPngEncoder::~YsMemoryPngEncoder()
{
	CleanUp();
}

void YsMemoryPngEncoder::CleanUp(void)
{
	if(NULL!=byte)
	{
		delete [] byte;
	}
	nByte=0;
	nAlloc=0;
	byte=NULL;
}

long long int YsMemoryPngEncoder::GetLength(void) const
{
	return nByte;
}

const unsigned char *YsMemoryPngEncoder::GetByteData(void) const
{
	return byte;
}

void YsMemoryPngEncoder::Detach(void)
{
	nByte=0;
	nAlloc=0;
	byte=NULL;
}

