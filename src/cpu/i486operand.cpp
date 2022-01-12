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



i486DX::Operand::Operand(int addressSize,int dataSize,const unsigned char operand[])
{
	Decode(addressSize,dataSize,operand);
}

unsigned int i486DX::Operand::Decode(int addressSize,int dataSize,const unsigned char operand[])
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

		static const unsigned char caseTable[256]=
		{
			3,3,3,3,3,3,0,3,3,3,3,3,3,3,0,3,3,3,3,3,3,3,0,3,3,3,3,3,3,3,0,3,
			3,3,3,3,3,3,0,3,3,3,3,3,3,3,0,3,3,3,3,3,3,3,0,3,3,3,3,3,3,3,0,3,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
			4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
		};
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

		switch(caseTable[operand[0]])
		{
		case 0:
			operandType=OPER_ADDR;
			baseReg=REG_NULL;
			indexReg=REG_NULL;
			// indexShift=0; Already cleared in Clear()
			offset=cpputil::GetSignedWord(operand+1);
			offsetBits=16;
			numBytes=3;
			break;
		case 1:
			operandType=OPER_ADDR;
			// indexShisft=0; Already cleared in Clear()
			baseReg=MODR_M_to_BaseIndex[MODR_M][0];
			indexReg=MODR_M_to_BaseIndex[MODR_M][1];
			offsetBits=8;
			offset=cpputil::GetSignedByte(operand[1]);
			numBytes=2;
			break;
		case 2:
			operandType=OPER_ADDR;
			// indexShisft=0; Already cleared in Clear()
			baseReg=MODR_M_to_BaseIndex[MODR_M][0];
			indexReg=MODR_M_to_BaseIndex[MODR_M][1];
			offsetBits=16;
			offset=cpputil::GetSignedWord(operand+1);
			numBytes=3;
			break;
		case 3:
			operandType=OPER_ADDR;
			// indexShisft=0; Already cleared in Clear()
			baseReg=MODR_M_to_BaseIndex[MODR_M][0];
			indexReg=MODR_M_to_BaseIndex[MODR_M][1];
			offset=0;  // Tentative
			offsetBits=16;
			numBytes=1;
			break;
		case 4:
			operandType=numBytesToRegisterOperandType[dataSize>>3];
			reg=R_M+(numBytesToBasicRegBase[dataSize>>3]);
			numBytes=1;
			break;
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

		#define A 0
		#define B 1
		#define C 2
		#define D 3
		#define E 4
		#define F 5
		#define G 6
		#define H 7

		static const unsigned char caseTable[256]=
		{
			E,E,E,E,B,A,E,E,E,E,E,E,B,A,E,E,E,E,E,E,B,A,E,E,E,E,E,E,B,A,E,E,
			E,E,E,E,B,A,E,E,E,E,E,E,B,A,E,E,E,E,E,E,B,A,E,E,E,E,E,E,B,A,E,E,
			C,C,C,C,G,C,C,C,C,C,C,C,G,C,C,C,C,C,C,C,G,C,C,C,C,C,C,C,G,C,C,C,
			C,C,C,C,G,C,C,C,C,C,C,C,G,C,C,C,C,C,C,C,G,C,C,C,C,C,C,C,G,C,C,C,
			D,D,D,D,H,D,D,D,D,D,D,D,H,D,D,D,D,D,D,D,H,D,D,D,D,D,D,D,H,D,D,D,
			D,D,D,D,H,D,D,D,D,D,D,D,H,D,D,D,D,D,D,D,H,D,D,D,D,D,D,D,H,D,D,D,
			F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,
			F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,F,
		};

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

		switch(caseTable[operand[0]])
		{
		case A:
			operandType=OPER_ADDR;
			baseReg=REG_NULL;
			indexReg=REG_NULL;
			// indexShift=0; Already cleared in Clear()
			offset=cpputil::GetSignedDword(operand+1);
			offsetBits=32;
			numBytes=5;
			break;
		case B: // MOD==0
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
		case G: // MOD==1
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
		case H: // MOD==2
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
		case C:
			operandType=OPER_ADDR;
			// indexShift=0; Already cleared in Clear()

			baseReg=REG_32BIT_REG_BASE+R_M;
			indexReg=REG_NULL;

			offsetBits=8;
			offset=cpputil::GetSignedByte(operand[1]);
			numBytes=2;
			break;
		case D:
			operandType=OPER_ADDR;
			// indexShift=0; Already cleared in Clear()

			baseReg=REG_32BIT_REG_BASE+R_M;
			indexReg=REG_NULL;

			offsetBits=32;
			offset=cpputil::GetSignedDword(operand+1);
			numBytes=5;
			break;
		case E:
			operandType=OPER_ADDR;
			// indexShift=0; Already cleared in Clear()
			offset=0;
			numBytes=1;

			baseReg=REG_32BIT_REG_BASE+R_M;
			indexReg=REG_NULL;
			break;
		case F:
			operandType=numBytesToRegisterOperandType[dataSize>>3];
			reg=R_M+(numBytesToBasicRegBase[dataSize>>3]);
			numBytes=1;
			break;
		}
	}

	return numBytes;
}
void i486DX::Operand::DecodeMODR_MForRegister(int dataSize,unsigned char MODR_M)
{
	auto REG_OPCODE=((MODR_M>>3)&7);
	MakeByRegisterNumber(dataSize,REG_OPCODE);
}
void i486DX::Operand::DecodeMODR_MForSegmentRegister(unsigned char MODR_M)
{
	auto REG_OPCODE=((MODR_M>>3)&7);
	operandType=OPER_SREG;
	reg=REG_SEGMENT_REG_BASE+REG_OPCODE;
}
void i486DX::Operand::DecodeMODR_MForCRRegister(unsigned char MODR_M)
{
	auto REG_OPCODE=((MODR_M>>3)&7);
	operandType=OPER_CR0+REG_OPCODE;
}
void i486DX::Operand::DecodeMODR_MForDRRegister(unsigned char MODR_M)
{
	auto REG_OPCODE=((MODR_M>>3)&7);
	operandType=OPER_DR0+REG_OPCODE;
}
void i486DX::Operand::DecodeMODR_MForTestRegister(unsigned char MODR_M)
{
	auto REG_OPCODE=((MODR_M>>3)&7);
	operandType=OPER_TEST0+REG_OPCODE;
}
void i486DX::Operand::MakeByRegisterNumber(int dataSize,int regNum)
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
void i486DX::Operand::MakeSimpleAddressOffsetFromImm(const Instruction &inst)
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

unsigned int i486DX::Operand::DecodeFarAddr(int addressSize,int operandSize,const unsigned char operand[])
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

std::string i486DX::Operand::Disassemble(uint32_t cs,uint32_t eip,const i486SymbolTable &symTable) const
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

std::string i486DX::Operand::DisassembleAsAddr(uint32_t cs,uint32_t eip,const i486SymbolTable &symTable) const
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
		disasm+=symTable.FormatImmLabel(cs,eip,offset);
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
		disasm+=symTable.FormatImmLabel(cs,eip,offset);
	}

	disasm.push_back(']');
	return disasm;
}
std::string i486DX::Operand::DisassembleAsFarAddr(uint32_t cs,uint32_t eip,const i486SymbolTable &symTable) const
{
	// It doesn't add [] because may be used by JMP.
	std::string disasm;
	switch(offsetBits)
	{
	case 16:
		disasm=cpputil::Ustox(seg);
		disasm.push_back(':');
		disasm+=cpputil::Ustox(offset);
		disasm+=symTable.FormatImmLabel(cs,eip,offset);
		break;
	case 32:
	default:
		disasm=cpputil::Ustox(seg);
		disasm.push_back(':');
		disasm+=cpputil::Uitox(offset);
		disasm+=symTable.FormatImmLabel(cs,eip,offset);
		break;
	}
	return disasm;
}
std::string i486DX::Operand::DisassembleAsReg(void) const
{
	return RegToStr[reg];
}
/* static */ std::string i486DX::Operand::GetSizeQualifierToDisassembly(const Operand &op,int operandSize)
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

/* static */ std::string i486DX::Operand::GetSegmentQualifierToDisassembly(int segOverride,const Operand &op)
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

unsigned int i486DX::Operand::GetSize(void) const
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
		return i486DX::GetRegisterSize(reg);
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
