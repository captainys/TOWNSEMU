/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <iostream>

#include "cpputil.h"
#include "i486.h"
#include "i486inst.h"
#include "i486symtable.h"



i486DXCommon::Operand::Operand(int addressSize,int dataSize,const unsigned char operand[])
{
	Decode(addressSize,dataSize,operand);
}

unsigned int i486DXCommon::Operand::Decode(int addressSize,int dataSize,const unsigned char operand[])
{
	NUM_BYTES_TO_BASIC_REG_BASE
	NUM_BYTES_TO_REGISTER_OPERAND_TYPE

	const auto MODR_M=operand[0];
	const auto MOD=((MODR_M>>6)&3);
	// const auto REG_OPCODE=((MODR_M>>3)&7);
	#define R_M (MODR_M&7)
	unsigned int numBytes=0;

	Clear();

	if(16==addressSize)
	{
		/* As Specification
		const auto R_M=(MODR_M&7);
		if(0b00==MOD && 0b110==R_M)                                     // CASE 0
		{
			operandType=OPER_ADDR;
			baseReg=REG_NULL;
			indexReg=REG_NULL;
			// indexShift=0; Already cleared in Clear()
			offset=cpputil::GetSignedWord(operand+1);
			offsetBits=16;
			numBytes=3;
		}
		else if(0b11!=MOD) // <=> if(0b00==MOD || 0b01==MOD || 0b10==MOD)
		{
			static const unsigned int R_M_to_BaseIndex[8][2]=
			{
				{REG_BX,REG_SI},
				{REG_BX,REG_DI},
				{REG_BP,REG_SI},
				{REG_BP,REG_DI},
				{REG_SI,REG_NULL},
				{REG_DI,REG_NULL},
				{REG_BP,REG_NULL},
				{REG_BX,REG_NULL},
			};

			operandType=OPER_ADDR;
			// indexShisft=0; Already cleared in Clear()

			baseReg=R_M_to_BaseIndex[R_M][0];
			indexReg=R_M_to_BaseIndex[R_M][1];

			if(0b01==MOD)                                             // CASE 1
			{
				offsetBits=8;
				offset=cpputil::GetSignedByte(operand[1]);
				numBytes=2;
			}
			else if(0b10==MOD)                                        // CASE 2
			{
				offsetBits=16;
				offset=cpputil::GetSignedWord(operand+1);
				numBytes=3;
			}
			else                                                      // CASE 3
			{
				offset=0;  // Tentative
				offsetBits=16;
				numBytes=1;
			}
		}
		else                                                          // CASE 4
		{
			operandType=OPER_REG;
			reg=R_M+(numBytesToBasicRegBase[dataSize>>3]);
			// Equivalent to:
			// if(8==dataSize)
			// {
			// 	reg=REG_8BIT_REG_BASE+R_M;
			// }
			// else if(16==dataSize)
			// {
			// 	reg=REG_16BIT_REG_BASE+R_M;
			// }
			// else if(32==dataSize)
			// {
			// 	reg=REG_32BIT_REG_BASE+R_M;
			// }
			numBytes=1;
		}
		*/

		// Original Case Table
		//static const unsigned char caseTable[256]=
		//{
		//	3,3,3,3,3,3,0,3,3,3,3,3,3,3,0,3,3,3,3,3,3,3,0,3,3,3,3,3,3,3,0,3,
		//	3,3,3,3,3,3,0,3,3,3,3,3,3,3,0,3,3,3,3,3,3,3,0,3,3,3,3,3,3,3,0,3,
		//	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		//	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		//	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		//	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		//	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
		//	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
		//};
		static const unsigned int MODR_M_to_BaseIndex[256][2]=
		{
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},

			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},

			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},

			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
			{REG_BX,REG_SI},{REG_BX,REG_DI},{REG_BP,REG_SI},{REG_BP,REG_DI},{REG_SI,REG_NULL},{REG_DI,REG_NULL},{REG_BP,REG_NULL},{REG_BX,REG_NULL},
		};

		switch(operand[0])
		{
		// Case 0
		case   6: case  14: case  22: case  30: case  38: case  46: case  54: case  62: 
			operandType=OPER_ADDR;
			baseReg=REG_NULL;
			indexReg=REG_NULL;
			// indexShift=0; Already cleared in Clear()
			offset=cpputil::GetSignedWord(operand+1);
			offsetBits=16;
			numBytes=3;
			break;
		// Case 1
		case  64: case  65: case  66: case  67: case  68: case  69: case  70: case  71: 
		case  72: case  73: case  74: case  75: case  76: case  77: case  78: case  79: 
		case  80: case  81: case  82: case  83: case  84: case  85: case  86: case  87: 
		case  88: case  89: case  90: case  91: case  92: case  93: case  94: case  95: 
		case  96: case  97: case  98: case  99: case 100: case 101: case 102: case 103: 
		case 104: case 105: case 106: case 107: case 108: case 109: case 110: case 111: 
		case 112: case 113: case 114: case 115: case 116: case 117: case 118: case 119: 
		case 120: case 121: case 122: case 123: case 124: case 125: case 126: case 127: 
			operandType=OPER_ADDR;
			// indexShisft=0; Already cleared in Clear()
			baseReg=MODR_M_to_BaseIndex[MODR_M][0];
			indexReg=MODR_M_to_BaseIndex[MODR_M][1];
			offsetBits=8;
			offset=cpputil::GetSignedByte(operand[1]);
			numBytes=2;
			break;
		// Case 2
		case 128: case 129: case 130: case 131: case 132: case 133: case 134: case 135: 
		case 136: case 137: case 138: case 139: case 140: case 141: case 142: case 143: 
		case 144: case 145: case 146: case 147: case 148: case 149: case 150: case 151: 
		case 152: case 153: case 154: case 155: case 156: case 157: case 158: case 159: 
		case 160: case 161: case 162: case 163: case 164: case 165: case 166: case 167: 
		case 168: case 169: case 170: case 171: case 172: case 173: case 174: case 175: 
		case 176: case 177: case 178: case 179: case 180: case 181: case 182: case 183: 
		case 184: case 185: case 186: case 187: case 188: case 189: case 190: case 191: 
			operandType=OPER_ADDR;
			// indexShisft=0; Already cleared in Clear()
			baseReg=MODR_M_to_BaseIndex[MODR_M][0];
			indexReg=MODR_M_to_BaseIndex[MODR_M][1];
			offsetBits=16;
			offset=cpputil::GetSignedWord(operand+1);
			numBytes=3;
			break;
		// Case 3
		case   0: case   1: case   2: case   3: case   4: case   5: case   7: case   8: 
		case   9: case  10: case  11: case  12: case  13: case  15: case  16: case  17: 
		case  18: case  19: case  20: case  21: case  23: case  24: case  25: case  26: 
		case  27: case  28: case  29: case  31: case  32: case  33: case  34: case  35: 
		case  36: case  37: case  39: case  40: case  41: case  42: case  43: case  44: 
		case  45: case  47: case  48: case  49: case  50: case  51: case  52: case  53: 
		case  55: case  56: case  57: case  58: case  59: case  60: case  61: case  63: 
			operandType=OPER_ADDR;
			// indexShisft=0; Already cleared in Clear()
			baseReg=MODR_M_to_BaseIndex[MODR_M][0];
			indexReg=MODR_M_to_BaseIndex[MODR_M][1];
			offset=0;  // Tentative
			offsetBits=16;
			numBytes=1;
			break;
		// Case 4
		case 192: case 193: case 194: case 195: case 196: case 197: case 198: case 199: 
		case 200: case 201: case 202: case 203: case 204: case 205: case 206: case 207: 
		case 208: case 209: case 210: case 211: case 212: case 213: case 214: case 215: 
		case 216: case 217: case 218: case 219: case 220: case 221: case 222: case 223: 
		case 224: case 225: case 226: case 227: case 228: case 229: case 230: case 231: 
		case 232: case 233: case 234: case 235: case 236: case 237: case 238: case 239: 
		case 240: case 241: case 242: case 243: case 244: case 245: case 246: case 247: 
		case 248: case 249: case 250: case 251: case 252: case 253: case 254: case 255: 
			operandType=numBytesToRegisterOperandType[dataSize>>3];
			reg=R_M+(numBytesToBasicRegBase[dataSize>>3]);
			numBytes=1;
			break;
		default:
			#ifdef _WIN32
				__assume(0);
			#elif defined(__clang__) || defined(__GNUC__)
				__builtin_unreachable();
			#else
				break;
			#endif
		}
	}
	else // if(32==addressSize)
	{
		/* As Specification
		if(0b00==MOD && 0b101==R_M)                                     // CASE 1
		{
			operandType=OPER_ADDR;
			baseReg=REG_NULL;
			indexReg=REG_NULL;
			// indexShift=0; Already cleared in Clear()
			offset=cpputil::GetSignedDword(operand+1);
			offsetBits=32;
			numBytes=5;
		}
		else if(0b11!=MOD) // <=> if(0b00==MOD || 0b01==MOD || 0b10==MOD)
		{
			if(0b100==R_M) // Depends on SIB                               CASE 2
			{
				operandType=OPER_ADDR;
				// indexShift=0; Already cleared in Clear()
				offset=0;
				numBytes=1;

				auto SIB=operand[1];
				auto SS=((SIB>>6)&3);
				auto INDEX=((SIB>>3)&7);
				auto BASE=(SIB&7);
				++numBytes;

				if(5!=BASE)
				{
					baseReg=REG_32BIT_REG_BASE+BASE;
				}
				else
				{
					if(0b00==MOD) // disp32[index]
					{
						baseReg=REG_NULL;
					}
					else if(0b01==MOD || 0b10==MOD) // disp[EBP][index]
					{
						baseReg=REG_EBP;
					}
				}
				if(0b100!=INDEX)
				{
					indexReg=REG_32BIT_REG_BASE+INDEX;
					indexShift=SS;
				}
				else
				{
					indexReg=REG_NULL;
				}

				if((0==MOD && 5==BASE) || 0b10==MOD)
				{
					offsetBits=32;
					offset=cpputil::GetSignedDword(operand+2);
					numBytes+=4;
				}
				else if(0b01==MOD)
				{
					offsetBits=8;
					offset=cpputil::GetSignedByte(operand[2]);
					++numBytes;
				}
			}
			else
			{
				if(0b01==MOD) // 8-bit offset                              CASE 3
				{
					operandType=OPER_ADDR;
					// indexShift=0; Already cleared in Clear()

					baseReg=REG_32BIT_REG_BASE+R_M;
					indexReg=REG_NULL;

					offsetBits=8;
					offset=cpputil::GetSignedByte(operand[1]);
					numBytes=2;
				}
				else if(0b10==MOD) // 32-bit offset                        CASE 4
				{
					operandType=OPER_ADDR;
					// indexShift=0; Already cleared in Clear()

					baseReg=REG_32BIT_REG_BASE+R_M;
					indexReg=REG_NULL;

					offsetBits=32;
					offset=cpputil::GetSignedDword(operand+1);
					numBytes=5;
				}
				else                                                    // CASE 5
				{
					operandType=OPER_ADDR;
					// indexShift=0; Already cleared in Clear()
					offset=0;
					numBytes=1;

					baseReg=REG_32BIT_REG_BASE+R_M;
					indexReg=REG_NULL;
				}
			}
		}
		else if(0b11==MOD)                                              // CASE 6
		{
			operandType=OPER_REG;
			reg=R_M+(numBytesToBasicRegBase[dataSize>>3]);
			//  Equivalent
			// if(8==dataSize)
			// {
			// 	reg=REG_8BIT_REG_BASE+R_M;
			// }
			// else if(16==dataSize)
			// {
			// 	reg=REG_16BIT_REG_BASE+R_M;
			// }
			// else if(32==dataSize)
			// {
			// 	reg=REG_32BIT_REG_BASE+R_M;
			// }
			numBytes=1;
		}
		// else                                                         // CASE 0
		*/

		// Original Case Table
		// #define A 0
		// #define B 1
		// #define C 2
		// #define D 3
		// #define E 4
		// #define F 5
		// #define G 6
		// #define H 7
		// static const unsigned char caseTable[256]=
		// {
		// 	E,E,E,E,B,A,E,E,E,E,E,E,B,A,E,E,E,E,E,E,B,A,E,E,E,E,E,E,B,A,E,E,
		// 	E,E,E,E,B,A,E,E,E,E,E,E,B,A,E,E,E,E,E,E,B,A,E,E,E,E,E,E,B,A,E,E,
		// 	C,C,C,C,G,C,C,C,C,C,C,C,G,C,C,C,C,C,C,C,G,C,C,C,C,C,C,C,G,C,C,C,
		// 	C,C,C,C,G,C,C,C,C,C,C,C,G,C,C,C,C,C,C,C,G,C,C,C,C,C,C,C,G,C,C,C,
		// 	D,D,D,D,H,D,D,D,D,D,D,D,H,D,D,D,D,D,D,D,H,D,D,D,D,D,D,D,H,D,D,D,
		// 	D,D,D,D,H,D,D,D,D,D,D,D,H,D,D,D,D,D,D,D,H,D,D,D,D,D,D,D,H,D,D,D,
		// 	F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,
		// 	F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,
		// };

		static const unsigned int SIB_INDEX[8]=
		{
			REG_EAX,
			REG_ECX,
			REG_EDX,
			REG_EBX,
			REG_NULL,
			REG_EBP,
			REG_ESI,
			REG_EDI,
		};

		switch(operand[0])
		{
		// Case A
		case   5: case  13: case  21: case  29: case  37: case  45: case  53: case  61: 
			operandType=OPER_ADDR;
			baseReg=REG_NULL;
			indexReg=REG_NULL;
			// indexShift=0; Already cleared in Clear()
			offset=cpputil::GetSignedDword(operand+1);
			offsetBits=32;
			numBytes=5;
			break;
		// Case B
		case   4: case  12: case  20: case  28: case  36: case  44: case  52: case  60: 
			// MOD==0
			{
				operandType=OPER_ADDR;
				// indexShift=0; Already cleared in Clear()
				offset=0;

				auto SIB=operand[1];
				auto SS=((SIB>>6)&3);
				auto INDEX=((SIB>>3)&7);
				auto BASE=(SIB&7);

				if(5!=BASE)
				{
					baseReg=REG_32BIT_REG_BASE+BASE;
					numBytes=2;
				}
				else
				{
					baseReg=REG_NULL; // 0b00==MOD && 5==BASE  disp32[index]
					offsetBits=32;
					offset=cpputil::GetSignedDword(operand+2);
					numBytes=6;
				}

				indexReg=SIB_INDEX[INDEX];
				indexShift=SS;
			}
			break;
		// Case C
		case  64: case  65: case  66: case  67: case  69: case  70: case  71: case  72: 
		case  73: case  74: case  75: case  77: case  78: case  79: case  80: case  81: 
		case  82: case  83: case  85: case  86: case  87: case  88: case  89: case  90: 
		case  91: case  93: case  94: case  95: case  96: case  97: case  98: case  99: 
		case 101: case 102: case 103: case 104: case 105: case 106: case 107: case 109: 
		case 110: case 111: case 112: case 113: case 114: case 115: case 117: case 118: 
		case 119: case 120: case 121: case 122: case 123: case 125: case 126: case 127: 
			operandType=OPER_ADDR;
			// indexShift=0; Already cleared in Clear()

			baseReg=REG_32BIT_REG_BASE+R_M;
			indexReg=REG_NULL;

			offsetBits=8;
			offset=cpputil::GetSignedByte(operand[1]);
			numBytes=2;
			break;
		// Case D
		case 128: case 129: case 130: case 131: case 133: case 134: case 135: case 136: 
		case 137: case 138: case 139: case 141: case 142: case 143: case 144: case 145: 
		case 146: case 147: case 149: case 150: case 151: case 152: case 153: case 154: 
		case 155: case 157: case 158: case 159: case 160: case 161: case 162: case 163: 
		case 165: case 166: case 167: case 168: case 169: case 170: case 171: case 173: 
		case 174: case 175: case 176: case 177: case 178: case 179: case 181: case 182: 
		case 183: case 184: case 185: case 186: case 187: case 189: case 190: case 191: 
			operandType=OPER_ADDR;
			// indexShift=0; Already cleared in Clear()

			baseReg=REG_32BIT_REG_BASE+R_M;
			indexReg=REG_NULL;

			offsetBits=32;
			offset=cpputil::GetSignedDword(operand+1);
			numBytes=5;
			break;
		// Case E
		case   0: case   1: case   2: case   3: case   6: case   7: case   8: case   9: 
		case  10: case  11: case  14: case  15: case  16: case  17: case  18: case  19: 
		case  22: case  23: case  24: case  25: case  26: case  27: case  30: case  31: 
		case  32: case  33: case  34: case  35: case  38: case  39: case  40: case  41: 
		case  42: case  43: case  46: case  47: case  48: case  49: case  50: case  51: 
		case  54: case  55: case  56: case  57: case  58: case  59: case  62: case  63: 
			operandType=OPER_ADDR;
			// indexShift=0; Already cleared in Clear()
			offset=0;
			numBytes=1;

			baseReg=REG_32BIT_REG_BASE+R_M;
			indexReg=REG_NULL;
			break;
		// Case F
		case 192: case 193: case 194: case 195: case 196: case 197: case 198: case 199: 
		case 200: case 201: case 202: case 203: case 204: case 205: case 206: case 207: 
		case 208: case 209: case 210: case 211: case 212: case 213: case 214: case 215: 
		case 216: case 217: case 218: case 219: case 220: case 221: case 222: case 223: 
		case 224: case 225: case 226: case 227: case 228: case 229: case 230: case 231: 
		case 232: case 233: case 234: case 235: case 236: case 237: case 238: case 239: 
		case 240: case 241: case 242: case 243: case 244: case 245: case 246: case 247: 
		case 248: case 249: case 250: case 251: case 252: case 253: case 254: case 255: 
			operandType=numBytesToRegisterOperandType[dataSize>>3];
			reg=R_M+(numBytesToBasicRegBase[dataSize>>3]);
			numBytes=1;
			break;
		// Case G
		case  68: case  76: case  84: case  92: case 100: case 108: case 116: case 124: 
			// MOD==1
			{
				operandType=OPER_ADDR;
				// indexShift=0; Already cleared in Clear()
				offset=0;

				auto SIB=operand[1];
				auto SS=((SIB>>6)&3);
				auto INDEX=((SIB>>3)&7);
				auto BASE=(SIB&7);

				if(5!=BASE)
				{
					baseReg=REG_32BIT_REG_BASE+BASE;
				}
				else
				{
					baseReg=REG_EBP;
				}

				indexReg=SIB_INDEX[INDEX];
				indexShift=SS;

				offsetBits=8;
				offset=cpputil::GetSignedByte(operand[2]);

				numBytes=3;
			}
			break;
		// Case H
		case 132: case 140: case 148: case 156: case 164: case 172: case 180: case 188: 
			// MOD==2
			{
				operandType=OPER_ADDR;
				// indexShift=0; Already cleared in Clear()
				offset=0;

				auto SIB=operand[1];
				auto SS=((SIB>>6)&3);
				auto INDEX=((SIB>>3)&7);
				auto BASE=(SIB&7);

				if(5!=BASE)
				{
					baseReg=REG_32BIT_REG_BASE+BASE;
				}
				else
				{
					baseReg=REG_EBP; // 0b10==MOD   disp[EBP][index]
				}

				indexReg=SIB_INDEX[INDEX];
				indexShift=SS;

				offsetBits=32;
				offset=cpputil::GetSignedDword(operand+2);

				numBytes=6;
			}
			break;

		default:
			#ifdef _WIN32
				__assume(0);
			#elif defined(__clang__) || defined(__GNUC__)
				__builtin_unreachable();
			#else
				break;
			#endif
		}
	}

	return numBytes;
}
void i486DXCommon::Operand::DecodeMODR_MForRegister(int dataSize,unsigned char MODR_M)
{
	auto REG_OPCODE=((MODR_M>>3)&7);
	MakeByRegisterNumber(dataSize,REG_OPCODE);
}
void i486DXCommon::Operand::DecodeMODR_MForSegmentRegister(unsigned char MODR_M)
{
	auto REG_OPCODE=((MODR_M>>3)&7);
	operandType=OPER_SREG;
	reg=REG_SEGMENT_REG_BASE+REG_OPCODE;
}
void i486DXCommon::Operand::DecodeMODR_MForCRRegister(unsigned char MODR_M)
{
	auto REG_OPCODE=((MODR_M>>3)&7);
	operandType=OPER_CR0+REG_OPCODE;
}
void i486DXCommon::Operand::DecodeMODR_MForDRRegister(unsigned char MODR_M)
{
	auto REG_OPCODE=((MODR_M>>3)&7);
	operandType=OPER_DR0+REG_OPCODE;
}
void i486DXCommon::Operand::DecodeMODR_MForTestRegister(unsigned char MODR_M)
{
	auto REG_OPCODE=((MODR_M>>3)&7);
	operandType=OPER_TEST0+REG_OPCODE;
}
void i486DXCommon::Operand::MakeByRegisterNumber(int dataSize,int regNum)
{
	NUM_BYTES_TO_BASIC_REG_BASE
	NUM_BYTES_TO_REGISTER_OPERAND_TYPE

	operandType=numBytesToRegisterOperandType[dataSize>>3];
	reg=regNum+numBytesToBasicRegBase[dataSize>>3];

	/* Equivalent
	switch(dataSize)
	{
	case 8:
		reg=REG_8BIT_REG_BASE+regNum;
		break;
	case 16:
		reg=REG_16BIT_REG_BASE+regNum;
		break;
	default:
		reg=REG_32BIT_REG_BASE+regNum;
		break;
	} */
}
void i486DXCommon::Operand::MakeSimpleAddressOffsetFromImm(const Instruction &inst)
{
	operandType=OPER_ADDR;
	baseReg=REG_NULL;
	indexReg=REG_NULL;
	indexShift=0;
	offsetBits=inst.addressSize;
	switch(inst.addressSize)
	{
	default:
	case 32:
		offset=inst.EvalSimm32();
		break;
	case 16:
		offset=inst.EvalSimm16();
		break;
	}
}

unsigned int i486DXCommon::Operand::DecodeFarAddr(int addressSize,int operandSize,const unsigned char operand[])
{
	operandType=OPER_FARADDR;
	switch(operandSize)
	{
	case 16:
		offset=cpputil::GetWord(operand);
		offsetBits=16;
		seg=cpputil::GetWord(operand+2);
		return 4;
	case 32:
	default:
		offset=cpputil::GetDword(operand);
		offsetBits=32;
		seg=cpputil::GetWord(operand+4);
		return 6;
	}
}

std::string i486DXCommon::Operand::Disassemble(uint32_t cs,uint32_t eip,const i486SymbolTable &symTable) const
{
	switch(operandType)
	{
	case OPER_ADDR:
		return DisassembleAsAddr(cs,eip,symTable);
	case OPER_FARADDR:
		return DisassembleAsFarAddr(cs,eip,symTable);
	case OPER_SREG:
	case OPER_REG8:
	case OPER_REG16:
	case OPER_REG32:
		return DisassembleAsReg();
	case OPER_CR0:
		return "CR0";
	case OPER_CR1:
		return "CR1";
	case OPER_CR2:
		return "CR2";
	case OPER_CR3:
		return "CR3";
	case OPER_DR0:
		return "DR0";
	case OPER_DR1:
		return "DR1";
	case OPER_DR2:
		return "DR2";
	case OPER_DR3:
		return "DR3";
	case OPER_DR4:
		return "DR4";
	case OPER_DR5:
		return "DR5";
	case OPER_DR6:
		return "DR6";
	case OPER_DR7:
		return "DR7";
	case OPER_TEST0:
		return "TEST0";
	case OPER_TEST1:
		return "TEST1";
	case OPER_TEST2:
		return "TEST2";
	case OPER_TEST3:
		return "TEST3";
	case OPER_TEST4:
		return "TEST4";
	case OPER_TEST5:
		return "TEST5";
	case OPER_TEST6:
		return "TEST6";
	case OPER_TEST7:
		return "TEST7";
	}
	return "(UndefinedOperandType?)";
}

std::string i486DXCommon::Operand::DisassembleAsAddr(uint32_t cs,uint32_t eip,const i486SymbolTable &symTable) const
{
	bool empty=true;

	std::string disasm;
	disasm.push_back('[');

	if(REG_NULL!=baseReg)
	{
		disasm+=RegToStr[baseReg];
		empty=false;
	}

	if(REG_NULL!=indexReg)
	{
		if(true!=empty)
		{
			disasm.push_back('+');
		}
		disasm+=RegToStr[indexReg];
		if(0!=indexShift)
		{
			disasm.push_back('*');
			disasm.push_back('0'+(1<<indexShift));
		}
		empty=false;
	}

	if(REG_NULL==baseReg && REG_NULL==indexReg)
	{
		switch(offsetBits)
		{
		case 8:
			disasm+=cpputil::Ubtox(offset);
			break;
		case 16:
			disasm+=cpputil::Ustox(offset);
			break;
		default:
			disasm+=cpputil::Uitox(offset);
			break;
		}
		disasm.push_back('H');
		disasm+=symTable.FormatOffsetLabel(cs,eip,offset);
	}
	else if(0!=offset)
	{
		if(true!=empty && 0<=offset)
		{
			disasm.push_back('+');
		}
		switch(offsetBits)
		{
		case 8:
			disasm+=cpputil::Btox(offset);
			break;
		case 16:
			disasm+=cpputil::Stox(offset);
			break;
		default:
			disasm+=cpputil::Itox(offset);
			break;
		}
		disasm.push_back('H');
		disasm+=symTable.FormatOffsetLabel(cs,eip,offset);
	}

	disasm.push_back(']');
	return disasm;
}
std::string i486DXCommon::Operand::DisassembleAsFarAddr(uint32_t cs,uint32_t eip,const i486SymbolTable &symTable) const
{
	// It doesn't add [] because may be used by JMP.
	std::string disasm;
	switch(offsetBits)
	{
	case 16:
		disasm=cpputil::Ustox(seg);
		disasm.push_back(':');
		disasm+=cpputil::Ustox(offset);
		disasm+=symTable.FormatOffsetLabel(cs,eip,offset);
		break;
	case 32:
	default:
		disasm=cpputil::Ustox(seg);
		disasm.push_back(':');
		disasm+=cpputil::Uitox(offset);
		disasm+=symTable.FormatOffsetLabel(cs,eip,offset);
		break;
	}
	return disasm;
}
std::string i486DXCommon::Operand::DisassembleAsReg(void) const
{
	return RegToStr[reg];
}
/* static */ std::string i486DXCommon::Operand::GetSizeQualifierToDisassembly(const Operand &op,int operandSize)
{
	if(op.operandType==OPER_ADDR)
	{
		switch(operandSize)
		{
		case 8:
			return "BYTE PTR ";
		case 16:
			return "WORD PTR ";
		case 32:
			return "DWORD PTR ";
		case 48:
			return "FWORD PTR ";
		}
	}
	return "";
}

/* static */ std::string i486DXCommon::Operand::GetSegmentQualifierToDisassembly(int segOverride,const Operand &op)
{
	if(0!=segOverride && OPER_ADDR==op.operandType)
	{
		switch(segOverride)
		{
		case SEG_OVERRIDE_CS: //  0x2E,
			return "CS:";
		case SEG_OVERRIDE_SS: //  0x36,
			return "SS:";
		case SEG_OVERRIDE_DS: //  0x3E,
			return "DS:";
		case SEG_OVERRIDE_ES: //  0x26,
			return "ES:";
		case SEG_OVERRIDE_FS: //  0x64,
			return "FS:";
		case SEG_OVERRIDE_GS: //  0x65,
			return "GS:";
		}
	}
	return "";
}

unsigned int i486DXCommon::Operand::GetSize(void) const
{
	switch(operandType)
	{
	case OPER_ADDR:
		return 0;
	case OPER_FARADDR:
		return 0;
	case OPER_SREG:
	case OPER_REG8:
	case OPER_REG16:
	case OPER_REG32:
		return i486DXCommon::GetRegisterSize(reg);
	case OPER_CR0:
	case OPER_CR1:
	case OPER_CR2:
	case OPER_CR3:
	case OPER_DR0:
	case OPER_DR1:
	case OPER_DR2:
	case OPER_DR3:
	case OPER_DR4:
	case OPER_DR5:
	case OPER_DR6:
	case OPER_DR7:
	case OPER_TEST0:
	case OPER_TEST1:
	case OPER_TEST2:
	case OPER_TEST3:
	case OPER_TEST4:
	case OPER_TEST5:
	case OPER_TEST6:
	case OPER_TEST7:
		return 4;
	}
	return 0;
}
