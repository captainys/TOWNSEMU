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
#include "i486debug.h"
#include "i486symtable.h"


// #define VERIFY_MEMORY_WINDOW
// #define BREAK_ON_FPU_INST


/*static*/ unsigned short i486DX::opCodeRenumberTable[I486_OPCODE_MAX+1];

void i486DX::MakeOpCodeRenumberTable(void)
{
	for(auto &renum : opCodeRenumberTable)
	{
		renum=~0;
	}

	opCodeRenumberTable[I486_OPCODE_UNDEFINED_SHOOT_INT6]=I486_RENUMBER_UNDEFINED_SHOOT_INT6;
	opCodeRenumberTable[I486_OPCODE_C0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_I8]=I486_RENUMBER_C0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_I8;
	opCodeRenumberTable[I486_OPCODE_C1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_I8]=I486_RENUMBER_C1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_I8;
	opCodeRenumberTable[I486_OPCODE_D0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_1]=I486_RENUMBER_D0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_1;
	opCodeRenumberTable[I486_OPCODE_D1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_1]=I486_RENUMBER_D1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_1;
	opCodeRenumberTable[I486_OPCODE_D2_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_CL]=I486_RENUMBER_D2_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_CL;
	opCodeRenumberTable[I486_OPCODE_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_CL]=I486_RENUMBER_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_CL;
	opCodeRenumberTable[I486_OPCODE_F6_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV]=I486_RENUMBER_F6_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV;
	opCodeRenumberTable[I486_OPCODE_F7_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV]=I486_RENUMBER_F7_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV;
	opCodeRenumberTable[I486_OPCODE_INC_DEC_R_M8]=I486_RENUMBER_INC_DEC_R_M8;
	opCodeRenumberTable[I486_OPCODE_INC_DEC_CALL_CALLF_JMP_JMPF_PUSH ]=I486_RENUMBER_INC_DEC_CALL_CALLF_JMP_JMPF_PUSH;
	opCodeRenumberTable[I486_OPCODE_LGDT_LIDT_SGDT_SIDT]=I486_RENUMBER_LGDT_LIDT_SGDT_SIDT;
	opCodeRenumberTable[I486_OPCODE_AAA]=I486_RENUMBER_AAA;
	opCodeRenumberTable[I486_OPCODE_AAM]=I486_RENUMBER_AAM;
	opCodeRenumberTable[I486_OPCODE_AAD]=I486_RENUMBER_AAD;
	opCodeRenumberTable[I486_OPCODE_AAS]=I486_RENUMBER_AAS;
	opCodeRenumberTable[I486_OPCODE_ARPL]=I486_RENUMBER_ARPL;
	opCodeRenumberTable[I486_OPCODE_BOUND]=I486_RENUMBER_BOUND;
	opCodeRenumberTable[I486_OPCODE_BT_BTS_BTR_BTC_RM_I8]=I486_RENUMBER_BT_BTS_BTR_BTC_RM_I8;
	opCodeRenumberTable[I486_OPCODE_BSF_R_RM]=I486_RENUMBER_BSF_R_RM;
	opCodeRenumberTable[I486_OPCODE_BSR_R_RM]=I486_RENUMBER_BSR_R_RM;
	opCodeRenumberTable[I486_OPCODE_BT_R_RM]=I486_RENUMBER_BT_R_RM;
	opCodeRenumberTable[I486_OPCODE_BTC_RM_R]=I486_RENUMBER_BTC_RM_R;
	opCodeRenumberTable[I486_OPCODE_BTS_RM_R]=I486_RENUMBER_BTS_RM_R;
	opCodeRenumberTable[I486_OPCODE_BTR_RM_R]=I486_RENUMBER_BTR_RM_R;
	opCodeRenumberTable[I486_OPCODE_LAR]=I486_RENUMBER_LAR;
	opCodeRenumberTable[I486_OPCODE_CALL_REL]=I486_RENUMBER_CALL_REL;
	opCodeRenumberTable[I486_OPCODE_CALL_FAR]=I486_RENUMBER_CALL_FAR;
	opCodeRenumberTable[I486_OPCODE_CLC]=I486_RENUMBER_CLC;
	opCodeRenumberTable[I486_OPCODE_CLD]=I486_RENUMBER_CLD;
	opCodeRenumberTable[I486_OPCODE_CLI]=I486_RENUMBER_CLI;
	opCodeRenumberTable[I486_OPCODE_CLTS]=I486_RENUMBER_CLTS;
	opCodeRenumberTable[I486_OPCODE_CMC]=I486_RENUMBER_CMC;
	opCodeRenumberTable[I486_OPCODE_ADC_AL_FROM_I8]=I486_RENUMBER_ADC_AL_FROM_I8;
	opCodeRenumberTable[I486_OPCODE_ADC_A_FROM_I]=I486_RENUMBER_ADC_A_FROM_I;
	opCodeRenumberTable[I486_OPCODE_ADC_RM8_FROM_R8]=I486_RENUMBER_ADC_RM8_FROM_R8;
	opCodeRenumberTable[I486_OPCODE_ADC_RM_FROM_R]=I486_RENUMBER_ADC_RM_FROM_R;
	opCodeRenumberTable[I486_OPCODE_ADC_R8_FROM_RM8]=I486_RENUMBER_ADC_R8_FROM_RM8;
	opCodeRenumberTable[I486_OPCODE_ADC_R_FROM_RM]=I486_RENUMBER_ADC_R_FROM_RM;
	opCodeRenumberTable[I486_OPCODE_ADD_AL_FROM_I8]=I486_RENUMBER_ADD_AL_FROM_I8;
	opCodeRenumberTable[I486_OPCODE_ADD_A_FROM_I]=I486_RENUMBER_ADD_A_FROM_I;
	opCodeRenumberTable[I486_OPCODE_ADD_RM8_FROM_R8]=I486_RENUMBER_ADD_RM8_FROM_R8;
	opCodeRenumberTable[I486_OPCODE_ADD_RM_FROM_R]=I486_RENUMBER_ADD_RM_FROM_R;
	opCodeRenumberTable[I486_OPCODE_ADD_R8_FROM_RM8]=I486_RENUMBER_ADD_R8_FROM_RM8;
	opCodeRenumberTable[I486_OPCODE_ADD_R_FROM_RM]=I486_RENUMBER_ADD_R_FROM_RM;
	opCodeRenumberTable[I486_OPCODE_AND_AL_FROM_I8]=I486_RENUMBER_AND_AL_FROM_I8;
	opCodeRenumberTable[I486_OPCODE_AND_A_FROM_I]=I486_RENUMBER_AND_A_FROM_I;
	opCodeRenumberTable[I486_OPCODE_AND_RM8_FROM_R8]=I486_RENUMBER_AND_RM8_FROM_R8;
	opCodeRenumberTable[I486_OPCODE_AND_RM_FROM_R]=I486_RENUMBER_AND_RM_FROM_R;
	opCodeRenumberTable[I486_OPCODE_AND_R8_FROM_RM8]=I486_RENUMBER_AND_R8_FROM_RM8;
	opCodeRenumberTable[I486_OPCODE_AND_R_FROM_RM]=I486_RENUMBER_AND_R_FROM_RM;
	opCodeRenumberTable[I486_OPCODE_CBW_CWDE]=I486_RENUMBER_CBW_CWDE;
	opCodeRenumberTable[I486_OPCODE_CWD_CDQ]=I486_RENUMBER_CWD_CDQ;
	opCodeRenumberTable[I486_OPCODE_CMP_AL_FROM_I8]=I486_RENUMBER_CMP_AL_FROM_I8;
	opCodeRenumberTable[I486_OPCODE_CMP_A_FROM_I]=I486_RENUMBER_CMP_A_FROM_I;
	opCodeRenumberTable[I486_OPCODE_CMP_RM8_FROM_R8]=I486_RENUMBER_CMP_RM8_FROM_R8;
	opCodeRenumberTable[I486_OPCODE_CMP_RM_FROM_R]=I486_RENUMBER_CMP_RM_FROM_R;
	opCodeRenumberTable[I486_OPCODE_CMP_R8_FROM_RM8]=I486_RENUMBER_CMP_R8_FROM_RM8;
	opCodeRenumberTable[I486_OPCODE_CMP_R_FROM_RM]=I486_RENUMBER_CMP_R_FROM_RM;
	opCodeRenumberTable[I486_OPCODE_CMPSB]=I486_RENUMBER_CMPSB;
	opCodeRenumberTable[I486_OPCODE_CMPS]=I486_RENUMBER_CMPS;
	opCodeRenumberTable[I486_OPCODE_DAA]=I486_RENUMBER_DAA;
	opCodeRenumberTable[I486_OPCODE_DAS]=I486_RENUMBER_DAS;
	opCodeRenumberTable[I486_OPCODE_DEC_EAX]=I486_RENUMBER_DEC_EAX;
	opCodeRenumberTable[I486_OPCODE_DEC_ECX]=I486_RENUMBER_DEC_ECX;
	opCodeRenumberTable[I486_OPCODE_DEC_EDX]=I486_RENUMBER_DEC_EDX;
	opCodeRenumberTable[I486_OPCODE_DEC_EBX]=I486_RENUMBER_DEC_EBX;
	opCodeRenumberTable[I486_OPCODE_DEC_ESP]=I486_RENUMBER_DEC_ESP;
	opCodeRenumberTable[I486_OPCODE_DEC_EBP]=I486_RENUMBER_DEC_EBP;
	opCodeRenumberTable[I486_OPCODE_DEC_ESI]=I486_RENUMBER_DEC_ESI;
	opCodeRenumberTable[I486_OPCODE_DEC_EDI]=I486_RENUMBER_DEC_EDI;
	opCodeRenumberTable[I486_OPCODE_ENTER]=I486_RENUMBER_ENTER;
	opCodeRenumberTable[I486_OPCODE_FWAIT]=I486_RENUMBER_FWAIT;
	opCodeRenumberTable[I486_OPCODE_FPU_D8_FADD]=I486_RENUMBER_FPU_D8_FADD;
	opCodeRenumberTable[I486_OPCODE_FPU_D9_FNSTCW_M16_FNSTENV_F2XM1_FXAM_FXCH_FXTRACT_FYL2X_FYL2XP1_FABS_]=I486_RENUMBER_FPU_D9_FNSTCW_M16_FNSTENV_F2XM1_FXAM_FXCH_FXTRACT_FYL2X_FYL2XP1_FABS_;
	opCodeRenumberTable[I486_OPCODE_FPU_DB_FNINIT_FRSTOR]=I486_RENUMBER_FPU_DB_FNINIT_FRSTOR;
	opCodeRenumberTable[I486_OPCODE_FPU_DC_FADD]=I486_RENUMBER_FPU_DC_FADD;
	opCodeRenumberTable[I486_OPCODE_FPU_DD_FLD_FSAVE_FST_FNSTSW_M16_FFREE_FUCOM]=I486_RENUMBER_FPU_DD_FLD_FSAVE_FST_FNSTSW_M16_FFREE_FUCOM;
	opCodeRenumberTable[I486_OPCODE_FPU_DE]=I486_RENUMBER_FPU_DE;
	opCodeRenumberTable[I486_OPCODE_FPU_DF_FNSTSW_AX]=I486_RENUMBER_FPU_DF_FNSTSW_AX;
	opCodeRenumberTable[I486_OPCODE_HLT]=I486_RENUMBER_HLT;
	opCodeRenumberTable[I486_OPCODE_IMUL_R_RM_I8]=I486_RENUMBER_IMUL_R_RM_I8;
	opCodeRenumberTable[I486_OPCODE_IMUL_R_RM_IMM]=I486_RENUMBER_IMUL_R_RM_IMM;
	opCodeRenumberTable[I486_OPCODE_IMUL_R_RM]=I486_RENUMBER_IMUL_R_RM;
	opCodeRenumberTable[I486_OPCODE_INSB]=I486_RENUMBER_INSB;
	opCodeRenumberTable[I486_OPCODE_IN_AL_I8]=I486_RENUMBER_IN_AL_I8;
	opCodeRenumberTable[I486_OPCODE_IN_A_I8]=I486_RENUMBER_IN_A_I8;
	opCodeRenumberTable[I486_OPCODE_IN_AL_DX]=I486_RENUMBER_IN_AL_DX;
	opCodeRenumberTable[I486_OPCODE_IN_A_DX]=I486_RENUMBER_IN_A_DX;
	opCodeRenumberTable[I486_OPCODE_INC_EAX]=I486_RENUMBER_INC_EAX;
	opCodeRenumberTable[I486_OPCODE_INC_ECX]=I486_RENUMBER_INC_ECX;
	opCodeRenumberTable[I486_OPCODE_INC_EDX]=I486_RENUMBER_INC_EDX;
	opCodeRenumberTable[I486_OPCODE_INC_EBX]=I486_RENUMBER_INC_EBX;
	opCodeRenumberTable[I486_OPCODE_INC_ESP]=I486_RENUMBER_INC_ESP;
	opCodeRenumberTable[I486_OPCODE_INC_EBP]=I486_RENUMBER_INC_EBP;
	opCodeRenumberTable[I486_OPCODE_INC_ESI]=I486_RENUMBER_INC_ESI;
	opCodeRenumberTable[I486_OPCODE_INC_EDI]=I486_RENUMBER_INC_EDI;
	opCodeRenumberTable[I486_OPCODE_INT3]=I486_RENUMBER_INT3;
	opCodeRenumberTable[I486_OPCODE_INT]=I486_RENUMBER_INT;
	opCodeRenumberTable[I486_OPCODE_INTO]=I486_RENUMBER_INTO;
	opCodeRenumberTable[I486_OPCODE_IRET]=I486_RENUMBER_IRET;
	opCodeRenumberTable[I486_OPCODE_JECXZ_REL8]=I486_RENUMBER_JECXZ_REL8;
	opCodeRenumberTable[I486_OPCODE_JA_REL8]=I486_RENUMBER_JA_REL8;
	opCodeRenumberTable[I486_OPCODE_JAE_REL8]=I486_RENUMBER_JAE_REL8;
	opCodeRenumberTable[I486_OPCODE_JB_REL8]=I486_RENUMBER_JB_REL8;
	opCodeRenumberTable[I486_OPCODE_JBE_REL8]=I486_RENUMBER_JBE_REL8;
	opCodeRenumberTable[I486_OPCODE_JE_REL8]=I486_RENUMBER_JE_REL8;
	opCodeRenumberTable[I486_OPCODE_JG_REL8]=I486_RENUMBER_JG_REL8;
	opCodeRenumberTable[I486_OPCODE_JGE_REL8]=I486_RENUMBER_JGE_REL8;
	opCodeRenumberTable[I486_OPCODE_JL_REL8]=I486_RENUMBER_JL_REL8;
	opCodeRenumberTable[I486_OPCODE_JLE_REL8]=I486_RENUMBER_JLE_REL8;
	opCodeRenumberTable[I486_OPCODE_JNE_REL8]=I486_RENUMBER_JNE_REL8;
	opCodeRenumberTable[I486_OPCODE_JNO_REL8]=I486_RENUMBER_JNO_REL8;
	opCodeRenumberTable[I486_OPCODE_JNP_REL8]=I486_RENUMBER_JNP_REL8;
	opCodeRenumberTable[I486_OPCODE_JNS_REL8]=I486_RENUMBER_JNS_REL8;
	opCodeRenumberTable[I486_OPCODE_JO_REL8]=I486_RENUMBER_JO_REL8;
	opCodeRenumberTable[I486_OPCODE_JP_REL8]=I486_RENUMBER_JP_REL8;
	opCodeRenumberTable[I486_OPCODE_JS_REL8]=I486_RENUMBER_JS_REL8;
	opCodeRenumberTable[I486_OPCODE_JA_REL]=I486_RENUMBER_JA_REL;
	opCodeRenumberTable[I486_OPCODE_JAE_REL]=I486_RENUMBER_JAE_REL;
	opCodeRenumberTable[I486_OPCODE_JB_REL]=I486_RENUMBER_JB_REL;
	opCodeRenumberTable[I486_OPCODE_JBE_REL]=I486_RENUMBER_JBE_REL;
	opCodeRenumberTable[I486_OPCODE_JE_REL]=I486_RENUMBER_JE_REL;
	opCodeRenumberTable[I486_OPCODE_JG_REL]=I486_RENUMBER_JG_REL;
	opCodeRenumberTable[I486_OPCODE_JGE_REL]=I486_RENUMBER_JGE_REL;
	opCodeRenumberTable[I486_OPCODE_JL_REL]=I486_RENUMBER_JL_REL;
	opCodeRenumberTable[I486_OPCODE_JLE_REL]=I486_RENUMBER_JLE_REL;
	opCodeRenumberTable[I486_OPCODE_JNE_REL]=I486_RENUMBER_JNE_REL;
	opCodeRenumberTable[I486_OPCODE_JNO_REL]=I486_RENUMBER_JNO_REL;
	opCodeRenumberTable[I486_OPCODE_JNP_REL]=I486_RENUMBER_JNP_REL;
	opCodeRenumberTable[I486_OPCODE_JNS_REL]=I486_RENUMBER_JNS_REL;
	opCodeRenumberTable[I486_OPCODE_JO_REL]=I486_RENUMBER_JO_REL;
	opCodeRenumberTable[I486_OPCODE_JP_REL]=I486_RENUMBER_JP_REL;
	opCodeRenumberTable[I486_OPCODE_JS_REL]=I486_RENUMBER_JS_REL;
	opCodeRenumberTable[I486_OPCODE_JMP_REL8]=I486_RENUMBER_JMP_REL8;
	opCodeRenumberTable[I486_OPCODE_JMP_REL]=I486_RENUMBER_JMP_REL;
	opCodeRenumberTable[I486_OPCODE_JMP_FAR]=I486_RENUMBER_JMP_FAR;
	opCodeRenumberTable[I486_OPCODE_BINARYOP_RM8_FROM_I8]=I486_RENUMBER_BINARYOP_RM8_FROM_I8;
	opCodeRenumberTable[I486_OPCODE_BINARYOP_R_FROM_I]=I486_RENUMBER_BINARYOP_R_FROM_I;
	opCodeRenumberTable[I486_OPCODE_BINARYOP_RM8_FROM_I8_ALIAS]=I486_RENUMBER_BINARYOP_RM8_FROM_I8_ALIAS;
	opCodeRenumberTable[I486_OPCODE_BINARYOP_RM_FROM_SXI8]=I486_RENUMBER_BINARYOP_RM_FROM_SXI8;
	opCodeRenumberTable[I486_OPCODE_LAHF]=I486_RENUMBER_LAHF;
	opCodeRenumberTable[I486_OPCODE_LEA]=I486_RENUMBER_LEA;
	opCodeRenumberTable[I486_OPCODE_LDS]=I486_RENUMBER_LDS;
	opCodeRenumberTable[I486_OPCODE_LSS]=I486_RENUMBER_LSS;
	opCodeRenumberTable[I486_OPCODE_LES]=I486_RENUMBER_LES;
	opCodeRenumberTable[I486_OPCODE_LFS]=I486_RENUMBER_LFS;
	opCodeRenumberTable[I486_OPCODE_LGS]=I486_RENUMBER_LGS;
	opCodeRenumberTable[I486_OPCODE_LEAVE]=I486_RENUMBER_LEAVE;
	opCodeRenumberTable[I486_OPCODE_LODSB]=I486_RENUMBER_LODSB;
	opCodeRenumberTable[I486_OPCODE_LODS]=I486_RENUMBER_LODS;
	opCodeRenumberTable[I486_OPCODE_LOOP]=I486_RENUMBER_LOOP;
	opCodeRenumberTable[I486_OPCODE_LOOPE]=I486_RENUMBER_LOOPE;
	opCodeRenumberTable[I486_OPCODE_LOOPNE]=I486_RENUMBER_LOOPNE;
	opCodeRenumberTable[I486_OPCODE_LSL]=I486_RENUMBER_LSL;
	opCodeRenumberTable[I486_OPCODE_MOV_FROM_R8]=I486_RENUMBER_MOV_FROM_R8;
	opCodeRenumberTable[I486_OPCODE_MOV_FROM_R]=I486_RENUMBER_MOV_FROM_R;
	opCodeRenumberTable[I486_OPCODE_MOV_TO_R8]=I486_RENUMBER_MOV_TO_R8;
	opCodeRenumberTable[I486_OPCODE_MOV_TO_R]=I486_RENUMBER_MOV_TO_R;
	opCodeRenumberTable[I486_OPCODE_MOV_FROM_SEG]=I486_RENUMBER_MOV_FROM_SEG;
	opCodeRenumberTable[I486_OPCODE_MOV_TO_SEG]=I486_RENUMBER_MOV_TO_SEG;
	opCodeRenumberTable[I486_OPCODE_MOV_M_TO_AL]=I486_RENUMBER_MOV_M_TO_AL;
	opCodeRenumberTable[I486_OPCODE_MOV_M_TO_EAX]=I486_RENUMBER_MOV_M_TO_EAX;
	opCodeRenumberTable[I486_OPCODE_MOV_M_FROM_AL]=I486_RENUMBER_MOV_M_FROM_AL;
	opCodeRenumberTable[I486_OPCODE_MOV_M_FROM_EAX]=I486_RENUMBER_MOV_M_FROM_EAX;
	opCodeRenumberTable[I486_OPCODE_MOV_I8_TO_AL]=I486_RENUMBER_MOV_I8_TO_AL;
	opCodeRenumberTable[I486_OPCODE_MOV_I8_TO_CL]=I486_RENUMBER_MOV_I8_TO_CL;
	opCodeRenumberTable[I486_OPCODE_MOV_I8_TO_DL]=I486_RENUMBER_MOV_I8_TO_DL;
	opCodeRenumberTable[I486_OPCODE_MOV_I8_TO_BL]=I486_RENUMBER_MOV_I8_TO_BL;
	opCodeRenumberTable[I486_OPCODE_MOV_I8_TO_AH]=I486_RENUMBER_MOV_I8_TO_AH;
	opCodeRenumberTable[I486_OPCODE_MOV_I8_TO_CH]=I486_RENUMBER_MOV_I8_TO_CH;
	opCodeRenumberTable[I486_OPCODE_MOV_I8_TO_DH]=I486_RENUMBER_MOV_I8_TO_DH;
	opCodeRenumberTable[I486_OPCODE_MOV_I8_TO_BH]=I486_RENUMBER_MOV_I8_TO_BH;
	opCodeRenumberTable[I486_OPCODE_MOV_I_TO_EAX]=I486_RENUMBER_MOV_I_TO_EAX;
	opCodeRenumberTable[I486_OPCODE_MOV_I_TO_ECX]=I486_RENUMBER_MOV_I_TO_ECX;
	opCodeRenumberTable[I486_OPCODE_MOV_I_TO_EDX]=I486_RENUMBER_MOV_I_TO_EDX;
	opCodeRenumberTable[I486_OPCODE_MOV_I_TO_EBX]=I486_RENUMBER_MOV_I_TO_EBX;
	opCodeRenumberTable[I486_OPCODE_MOV_I_TO_ESP]=I486_RENUMBER_MOV_I_TO_ESP;
	opCodeRenumberTable[I486_OPCODE_MOV_I_TO_EBP]=I486_RENUMBER_MOV_I_TO_EBP;
	opCodeRenumberTable[I486_OPCODE_MOV_I_TO_ESI]=I486_RENUMBER_MOV_I_TO_ESI;
	opCodeRenumberTable[I486_OPCODE_MOV_I_TO_EDI]=I486_RENUMBER_MOV_I_TO_EDI;
	opCodeRenumberTable[I486_OPCODE_MOV_I8_TO_RM8]=I486_RENUMBER_MOV_I8_TO_RM8;
	opCodeRenumberTable[I486_OPCODE_MOV_I_TO_RM]=I486_RENUMBER_MOV_I_TO_RM;
	opCodeRenumberTable[I486_OPCODE_MOV_TO_CR]=I486_RENUMBER_MOV_TO_CR;
	opCodeRenumberTable[I486_OPCODE_MOV_FROM_CR]=I486_RENUMBER_MOV_FROM_CR;
	opCodeRenumberTable[I486_OPCODE_MOV_FROM_DR]=I486_RENUMBER_MOV_FROM_DR;
	opCodeRenumberTable[I486_OPCODE_MOV_TO_DR]=I486_RENUMBER_MOV_TO_DR;
	opCodeRenumberTable[I486_OPCODE_MOV_FROM_TR]=I486_RENUMBER_MOV_FROM_TR;
	opCodeRenumberTable[I486_OPCODE_MOV_TO_TR]=I486_RENUMBER_MOV_TO_TR;
	opCodeRenumberTable[I486_OPCODE_MOVSB]=I486_RENUMBER_MOVSB;
	opCodeRenumberTable[I486_OPCODE_MOVS]=I486_RENUMBER_MOVS;
	opCodeRenumberTable[I486_OPCODE_MOVSX_R_RM8]=I486_RENUMBER_MOVSX_R_RM8;
	opCodeRenumberTable[I486_OPCODE_MOVSX_R32_RM16]=I486_RENUMBER_MOVSX_R32_RM16;
	opCodeRenumberTable[I486_OPCODE_MOVZX_R_RM8]=I486_RENUMBER_MOVZX_R_RM8;
	opCodeRenumberTable[I486_OPCODE_MOVZX_R32_RM16]=I486_RENUMBER_MOVZX_R32_RM16;
	opCodeRenumberTable[I486_OPCODE_NOP]=I486_RENUMBER_NOP;
	opCodeRenumberTable[I486_OPCODE_OUT_I8_AL]=I486_RENUMBER_OUT_I8_AL;
	opCodeRenumberTable[I486_OPCODE_OUT_I8_A]=I486_RENUMBER_OUT_I8_A;
	opCodeRenumberTable[I486_OPCODE_OUT_DX_AL]=I486_RENUMBER_OUT_DX_AL;
	opCodeRenumberTable[I486_OPCODE_OUT_DX_A]=I486_RENUMBER_OUT_DX_A;
	opCodeRenumberTable[I486_OPCODE_OUTSB]=I486_RENUMBER_OUTSB;
	opCodeRenumberTable[I486_OPCODE_OUTS]=I486_RENUMBER_OUTS;
	opCodeRenumberTable[I486_OPCODE_PUSHA]=I486_RENUMBER_PUSHA;
	opCodeRenumberTable[I486_OPCODE_PUSHF]=I486_RENUMBER_PUSHF;
	opCodeRenumberTable[I486_OPCODE_PUSH_EAX]=I486_RENUMBER_PUSH_EAX;
	opCodeRenumberTable[I486_OPCODE_PUSH_ECX]=I486_RENUMBER_PUSH_ECX;
	opCodeRenumberTable[I486_OPCODE_PUSH_EDX]=I486_RENUMBER_PUSH_EDX;
	opCodeRenumberTable[I486_OPCODE_PUSH_EBX]=I486_RENUMBER_PUSH_EBX;
	opCodeRenumberTable[I486_OPCODE_PUSH_ESP]=I486_RENUMBER_PUSH_ESP;
	opCodeRenumberTable[I486_OPCODE_PUSH_EBP]=I486_RENUMBER_PUSH_EBP;
	opCodeRenumberTable[I486_OPCODE_PUSH_ESI]=I486_RENUMBER_PUSH_ESI;
	opCodeRenumberTable[I486_OPCODE_PUSH_EDI]=I486_RENUMBER_PUSH_EDI;
	opCodeRenumberTable[I486_OPCODE_PUSH_I8]=I486_RENUMBER_PUSH_I8;
	opCodeRenumberTable[I486_OPCODE_PUSH_I]=I486_RENUMBER_PUSH_I;
	opCodeRenumberTable[I486_OPCODE_PUSH_CS]=I486_RENUMBER_PUSH_CS;
	opCodeRenumberTable[I486_OPCODE_PUSH_SS]=I486_RENUMBER_PUSH_SS;
	opCodeRenumberTable[I486_OPCODE_PUSH_DS]=I486_RENUMBER_PUSH_DS;
	opCodeRenumberTable[I486_OPCODE_PUSH_ES]=I486_RENUMBER_PUSH_ES;
	opCodeRenumberTable[I486_OPCODE_PUSH_FS]=I486_RENUMBER_PUSH_FS;
	opCodeRenumberTable[I486_OPCODE_PUSH_GS]=I486_RENUMBER_PUSH_GS;
	opCodeRenumberTable[I486_OPCODE_POP_M]=I486_RENUMBER_POP_M;
	opCodeRenumberTable[I486_OPCODE_POP_EAX]=I486_RENUMBER_POP_EAX;
	opCodeRenumberTable[I486_OPCODE_POP_ECX]=I486_RENUMBER_POP_ECX;
	opCodeRenumberTable[I486_OPCODE_POP_EDX]=I486_RENUMBER_POP_EDX;
	opCodeRenumberTable[I486_OPCODE_POP_EBX]=I486_RENUMBER_POP_EBX;
	opCodeRenumberTable[I486_OPCODE_POP_ESP]=I486_RENUMBER_POP_ESP;
	opCodeRenumberTable[I486_OPCODE_POP_EBP]=I486_RENUMBER_POP_EBP;
	opCodeRenumberTable[I486_OPCODE_POP_ESI]=I486_RENUMBER_POP_ESI;
	opCodeRenumberTable[I486_OPCODE_POP_EDI]=I486_RENUMBER_POP_EDI;
	opCodeRenumberTable[I486_OPCODE_POP_SS]=I486_RENUMBER_POP_SS;
	opCodeRenumberTable[I486_OPCODE_POP_DS]=I486_RENUMBER_POP_DS;
	opCodeRenumberTable[I486_OPCODE_POP_ES]=I486_RENUMBER_POP_ES;
	opCodeRenumberTable[I486_OPCODE_POP_FS]=I486_RENUMBER_POP_FS;
	opCodeRenumberTable[I486_OPCODE_POP_GS]=I486_RENUMBER_POP_GS;
	opCodeRenumberTable[I486_OPCODE_POPA]=I486_RENUMBER_POPA;
	opCodeRenumberTable[I486_OPCODE_POPF]=I486_RENUMBER_POPF;
	opCodeRenumberTable[I486_OPCODE_OR_AL_FROM_I8]=I486_RENUMBER_OR_AL_FROM_I8;
	opCodeRenumberTable[I486_OPCODE_OR_A_FROM_I]=I486_RENUMBER_OR_A_FROM_I;
	opCodeRenumberTable[I486_OPCODE_OR_RM8_FROM_R8]=I486_RENUMBER_OR_RM8_FROM_R8;
	opCodeRenumberTable[I486_OPCODE_OR_RM_FROM_R]=I486_RENUMBER_OR_RM_FROM_R;
	opCodeRenumberTable[I486_OPCODE_OR_R8_FROM_RM8]=I486_RENUMBER_OR_R8_FROM_RM8;
	opCodeRenumberTable[I486_OPCODE_OR_R_FROM_RM]=I486_RENUMBER_OR_R_FROM_RM;
	opCodeRenumberTable[I486_OPCODE_RET]=I486_RENUMBER_RET;
	opCodeRenumberTable[I486_OPCODE_RETF]=I486_RENUMBER_RETF;
	opCodeRenumberTable[I486_OPCODE_RET_I16]=I486_RENUMBER_RET_I16;
	opCodeRenumberTable[I486_OPCODE_RETF_I16]=I486_RENUMBER_RETF_I16;
	opCodeRenumberTable[I486_OPCODE_SAHF]=I486_RENUMBER_SAHF;
	opCodeRenumberTable[I486_OPCODE_SHLD_RM_I8]=I486_RENUMBER_SHLD_RM_I8;
	opCodeRenumberTable[I486_OPCODE_SHLD_RM_CL]=I486_RENUMBER_SHLD_RM_CL;
	opCodeRenumberTable[I486_OPCODE_SHRD_RM_I8]=I486_RENUMBER_SHRD_RM_I8;
	opCodeRenumberTable[I486_OPCODE_SHRD_RM_CL]=I486_RENUMBER_SHRD_RM_CL;
	opCodeRenumberTable[I486_OPCODE_SCASB]=I486_RENUMBER_SCASB;
	opCodeRenumberTable[I486_OPCODE_SCAS]=I486_RENUMBER_SCAS;
	opCodeRenumberTable[I486_OPCODE_SETA]=I486_RENUMBER_SETA;
	opCodeRenumberTable[I486_OPCODE_SETAE]=I486_RENUMBER_SETAE;
	opCodeRenumberTable[I486_OPCODE_SETB]=I486_RENUMBER_SETB;
	opCodeRenumberTable[I486_OPCODE_SETBE]=I486_RENUMBER_SETBE;
	opCodeRenumberTable[I486_OPCODE_SETE]=I486_RENUMBER_SETE;
	opCodeRenumberTable[I486_OPCODE_SETG]=I486_RENUMBER_SETG;
	opCodeRenumberTable[I486_OPCODE_SETGE]=I486_RENUMBER_SETGE;
	opCodeRenumberTable[I486_OPCODE_SETL]=I486_RENUMBER_SETL;
	opCodeRenumberTable[I486_OPCODE_SETLE]=I486_RENUMBER_SETLE;
	opCodeRenumberTable[I486_OPCODE_SETNE]=I486_RENUMBER_SETNE;
	opCodeRenumberTable[I486_OPCODE_SETNO]=I486_RENUMBER_SETNO;
	opCodeRenumberTable[I486_OPCODE_SETNP]=I486_RENUMBER_SETNP;
	opCodeRenumberTable[I486_OPCODE_SETNS]=I486_RENUMBER_SETNS;
	opCodeRenumberTable[I486_OPCODE_SETO]=I486_RENUMBER_SETO;
	opCodeRenumberTable[I486_OPCODE_SETP]=I486_RENUMBER_SETP;
	opCodeRenumberTable[I486_OPCODE_SETS]=I486_RENUMBER_SETS;
	opCodeRenumberTable[I486_OPCODE_SLDT_STR_LLDT_LTR_VERR_VERW]=I486_RENUMBER_SLDT_STR_LLDT_LTR_VERR_VERW;
	opCodeRenumberTable[I486_OPCODE_STC]=I486_RENUMBER_STC;
	opCodeRenumberTable[I486_OPCODE_STD]=I486_RENUMBER_STD;
	opCodeRenumberTable[I486_OPCODE_STI]=I486_RENUMBER_STI;
	opCodeRenumberTable[I486_OPCODE_STOSB]=I486_RENUMBER_STOSB;
	opCodeRenumberTable[I486_OPCODE_STOS]=I486_RENUMBER_STOS;
	opCodeRenumberTable[I486_OPCODE_SBB_AL_FROM_I8]=I486_RENUMBER_SBB_AL_FROM_I8;
	opCodeRenumberTable[I486_OPCODE_SBB_A_FROM_I]=I486_RENUMBER_SBB_A_FROM_I;
	opCodeRenumberTable[I486_OPCODE_SBB_RM8_FROM_R8]=I486_RENUMBER_SBB_RM8_FROM_R8;
	opCodeRenumberTable[I486_OPCODE_SBB_RM_FROM_R]=I486_RENUMBER_SBB_RM_FROM_R;
	opCodeRenumberTable[I486_OPCODE_SBB_R8_FROM_RM8]=I486_RENUMBER_SBB_R8_FROM_RM8;
	opCodeRenumberTable[I486_OPCODE_SBB_R_FROM_RM]=I486_RENUMBER_SBB_R_FROM_RM;
	opCodeRenumberTable[I486_OPCODE_SUB_AL_FROM_I8]=I486_RENUMBER_SUB_AL_FROM_I8;
	opCodeRenumberTable[I486_OPCODE_SUB_A_FROM_I]=I486_RENUMBER_SUB_A_FROM_I;
	opCodeRenumberTable[I486_OPCODE_SUB_RM8_FROM_R8]=I486_RENUMBER_SUB_RM8_FROM_R8;
	opCodeRenumberTable[I486_OPCODE_SUB_RM_FROM_R]=I486_RENUMBER_SUB_RM_FROM_R;
	opCodeRenumberTable[I486_OPCODE_SUB_R8_FROM_RM8]=I486_RENUMBER_SUB_R8_FROM_RM8;
	opCodeRenumberTable[I486_OPCODE_SUB_R_FROM_RM]=I486_RENUMBER_SUB_R_FROM_RM;
	opCodeRenumberTable[I486_OPCODE_TEST_AL_FROM_I8]=I486_RENUMBER_TEST_AL_FROM_I8;
	opCodeRenumberTable[I486_OPCODE_TEST_A_FROM_I]=I486_RENUMBER_TEST_A_FROM_I;
	opCodeRenumberTable[I486_OPCODE_TEST_RM8_FROM_R8]=I486_RENUMBER_TEST_RM8_FROM_R8;
	opCodeRenumberTable[I486_OPCODE_TEST_RM_FROM_R]=I486_RENUMBER_TEST_RM_FROM_R;
	opCodeRenumberTable[I486_OPCODE_XCHG_EAX_ECX]=I486_RENUMBER_XCHG_EAX_ECX;
	opCodeRenumberTable[I486_OPCODE_XCHG_EAX_EDX]=I486_RENUMBER_XCHG_EAX_EDX;
	opCodeRenumberTable[I486_OPCODE_XCHG_EAX_EBX]=I486_RENUMBER_XCHG_EAX_EBX;
	opCodeRenumberTable[I486_OPCODE_XCHG_EAX_ESP]=I486_RENUMBER_XCHG_EAX_ESP;
	opCodeRenumberTable[I486_OPCODE_XCHG_EAX_EBP]=I486_RENUMBER_XCHG_EAX_EBP;
	opCodeRenumberTable[I486_OPCODE_XCHG_EAX_ESI]=I486_RENUMBER_XCHG_EAX_ESI;
	opCodeRenumberTable[I486_OPCODE_XCHG_EAX_EDI]=I486_RENUMBER_XCHG_EAX_EDI;
	opCodeRenumberTable[I486_OPCODE_XCHG_RM8_R8]=I486_RENUMBER_XCHG_RM8_R8;
	opCodeRenumberTable[I486_OPCODE_XCHG_RM_R]=I486_RENUMBER_XCHG_RM_R;
	opCodeRenumberTable[I486_OPCODE_XLAT]=I486_RENUMBER_XLAT;
	opCodeRenumberTable[I486_OPCODE_XOR_AL_FROM_I8]=I486_RENUMBER_XOR_AL_FROM_I8;
	opCodeRenumberTable[I486_OPCODE_XOR_A_FROM_I]=I486_RENUMBER_XOR_A_FROM_I;
	opCodeRenumberTable[I486_OPCODE_XOR_RM8_FROM_R8]=I486_RENUMBER_XOR_RM8_FROM_R8;
	opCodeRenumberTable[I486_OPCODE_XOR_RM_FROM_R]=I486_RENUMBER_XOR_RM_FROM_R;
	opCodeRenumberTable[I486_OPCODE_XOR_R8_FROM_RM8]=I486_RENUMBER_XOR_R8_FROM_RM8;
	opCodeRenumberTable[I486_OPCODE_XOR_R_FROM_RM]=I486_RENUMBER_XOR_R_FROM_RM;
}



class i486DX::DebugFetchInstructionFunctions
{
public:
	typedef const i486DX CPUCLASS;

	inline static void GetConstMemoryWindow(
		CPUCLASS &cpu,
		MemoryAccess::ConstMemoryWindow &memWin,
		unsigned int codeAddressSize,
		const i486DX::SegmentRegister &CS,
		unsigned int offset,
		const Memory &mem)
	{
		memWin=cpu.DebugGetConstMemoryWindow(codeAddressSize,CS,offset,mem);
	}
	inline static unsigned int FetchInstructionByte(
		CPUCLASS &cpu,
		MemoryAccess::ConstPointer &ptr,
		unsigned int addressSize,
		const SegmentRegister &seg,
		unsigned int offset,const Memory &mem)
	{
		return cpu.DebugFetchByte(addressSize,seg,offset,mem);
	}

	inline static void FetchOperand8(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		cpu.DebugFetchOperand8(inst,ptr,seg,offset,mem);
	}
	inline static void PeekOperand8(CPUCLASS &cpu,unsigned int &operand,const Instruction &inst,const MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		cpu.DebugPeekOperand8(operand,inst,ptr,seg,offset,mem);
	}
	inline static void FetchOperand16(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		cpu.DebugFetchOperand16(inst,ptr,seg,offset,mem);
	}
	inline static void FetchOperand32(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		cpu.DebugFetchOperand32(inst,ptr,seg,offset,mem);
	}
	inline static unsigned int FetchOperand16or32(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		return cpu.DebugFetchOperand16or32(inst,ptr,seg,offset,mem);
	}
	inline static void FetchImm8(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		cpu.DebugFetchImm8(inst,ptr,seg,offset,mem);
	}
	inline static void FetchImm16(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		cpu.DebugFetchImm16(inst,ptr,seg,offset,mem);
	}
	inline static void FetchImm32(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		cpu.DebugFetchImm32(inst,ptr,seg,offset,mem);
	}
	inline static unsigned int FetchImm16or32(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		return cpu.DebugFetchImm16or32(inst,ptr,seg,offset,mem);
	}
};
class i486DX::RealFetchInstructionFunctions
{
public:
	typedef i486DX CPUCLASS;

	inline static void GetConstMemoryWindow(
		CPUCLASS &cpu,
		MemoryAccess::ConstMemoryWindow &memWin,
		unsigned int codeAddressSize,
		const i486DX::SegmentRegister &CS,
		unsigned int offset,
		const Memory &mem)
	{
		memWin=cpu.GetConstMemoryWindow(codeAddressSize,CS,offset,mem);
	}
	inline static unsigned int FetchInstructionByte(
		CPUCLASS &cpu,
		MemoryAccess::ConstPointer &ptr,
		unsigned int addressSize,
		const SegmentRegister &seg,
		unsigned int offset,const Memory &mem)
	{
		return cpu.FetchInstructionByte(ptr,addressSize,seg,offset,mem);
	}

	inline static void FetchOperand8(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		cpu.FetchOperand8(inst,ptr,seg,offset,mem);
	}
	inline static void PeekOperand8(CPUCLASS &cpu,unsigned int &operand,const Instruction &inst,const MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		cpu.PeekOperand8(operand,inst,ptr,seg,offset,mem);
	}
	inline static void FetchOperand16(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		cpu.FetchOperand16(inst,ptr,seg,offset,mem);
	}
	inline static void FetchOperand32(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		cpu.FetchOperand32(inst,ptr,seg,offset,mem);
	}
	inline static unsigned int FetchOperand16or32(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		return cpu.FetchOperand16or32(inst,ptr,seg,offset,mem);
	}
	inline static void FetchImm8(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		cpu.FetchImm8(inst,ptr,seg,offset,mem);
	}
	inline static void FetchImm16(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		cpu.FetchImm16(inst,ptr,seg,offset,mem);
	}
	inline static void FetchImm32(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		cpu.FetchImm32(inst,ptr,seg,offset,mem);
	}
	inline static unsigned int FetchImm16or32(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
	{
		return cpu.FetchImm16or32(inst,ptr,seg,offset,mem);
	}
};
template <class CPUCLASS,class FUNCCLASS>
class i486DX::FetchInstructionClass
{
public:
	static inline void FetchInstruction(
	   CPUCLASS &cpu,
	   MemoryAccess::ConstMemoryWindow &memWin,
	   Instruction &inst,Operand &op1,Operand &op2,
	   const SegmentRegister &CS,unsigned int offset,const Memory &mem,unsigned int defOperSize,unsigned int defAddrSize)
	{
		inst.Clear();
		inst.operandSize=defOperSize;
		inst.addressSize=defAddrSize;
		inst.codeAddressSize=defAddrSize;

		// Question: Do prefixes need to be in the specific order INST_PREFIX->ADDRSIZE_OVERRIDE->OPSIZE_OVERRIDE->SEG_OVERRIDE?

		auto CSEIPLinear=CS.baseLinearAddr+offset;
		if(nullptr==memWin.ptr || true!=memWin.IsLinearAddressInRange(CSEIPLinear))
		{
			FUNCCLASS::GetConstMemoryWindow(cpu,memWin,inst.codeAddressSize,CS,offset,mem);
		}
		auto ptr=memWin.GetReadAccessPointer(CSEIPLinear);

	#ifdef VERIFY_MEMORY_WINDOW
		MemoryAccess::ConstMemoryWindow testMemWin;
		FUNCCLASS::GetConstMemoryWindow(cpu,testMemWin,inst.codeAddressSize,CS,offset,mem);
		if(testMemWin.linearBaseAddr!=memWin.linearBaseAddr || testMemWin.ptr!=memWin.ptr)
		{
			std::cout << "Memory Window Test failed!" << std::endl;
			std::cout << "Retained Memory Window Linear Addr: " << cpputil::Uitox(memWin.linearBaseAddr) << std::endl;
			std::cout << "CS:EIP Memory Window Linear Addr:   " << cpputil::Uitox(testMemWin.linearBaseAddr) << std::endl;
			std::cout << "Pointer Diff " << cpputil::Uitox(memWin.ptr-testMemWin.ptr) << std::endl;
			Abort("Memory Cache Failed.");
			return;
		}
	#endif

		unsigned int lastByte=FUNCCLASS::FetchInstructionByte(cpu,ptr,inst.codeAddressSize,CS,offset+inst.numBytes++,mem);
		inst.opCode=0;
		while(true==i486DX::IsPrefix[lastByte])
		{
			switch(lastByte)
			{
			case INST_PREFIX_REP: // REP/REPE/REPZ
			case INST_PREFIX_REPNE:
			case INST_PREFIX_LOCK:
				inst.instPrefix=lastByte;
				break;

			case SEG_OVERRIDE_CS:
			case SEG_OVERRIDE_SS:
			case SEG_OVERRIDE_DS:
			case SEG_OVERRIDE_ES:
			case SEG_OVERRIDE_FS:
			case SEG_OVERRIDE_GS:
				inst.segOverride=lastByte;
				break;

			case OPSIZE_OVERRIDE:
				inst.operandSize=defOperSize^48;
				break;
			case ADDRSIZE_OVERRIDE:
				inst.addressSize=defAddrSize^48;
				break;

			case FPU_FWAIT:
				inst.fwait=lastByte;
				break;

			case I486_OPCODE_NEED_SECOND_BYTE: //0x0F
				inst.opCode=(I486_OPCODE_NEED_SECOND_BYTE<<8);
				lastByte=FUNCCLASS::FetchInstructionByte(cpu,ptr,inst.codeAddressSize,CS,offset+inst.numBytes++,mem);
				goto PREFIX_DONE;

			default:
				goto PREFIX_DONE;
			}
			lastByte=FUNCCLASS::FetchInstructionByte(cpu,ptr,inst.codeAddressSize,CS,offset+inst.numBytes++,mem);
		}
	PREFIX_DONE:
		inst.opCode|=lastByte;
		CPUCLASS::template FetchOperand<CPUCLASS,FUNCCLASS>(cpu,inst,op1,op2,ptr,CS,offset+inst.numBytes,mem);
	}
};



void i486DX::FetchInstruction(
   MemoryAccess::ConstMemoryWindow &memWin,
   Instruction &inst,Operand &op1,Operand &op2,
   const SegmentRegister &CS,unsigned int offset,const Memory &mem,unsigned int defOperSize,unsigned int defAddrSize)
{
	FetchInstructionClass<i486DX,RealFetchInstructionFunctions>::FetchInstruction(
	    *this,memWin,inst,op1,op2,CS,offset,mem,defOperSize,defAddrSize);
}

void i486DX::DebugFetchInstruction(
   MemoryAccess::ConstMemoryWindow &memWin,
   Instruction &inst,Operand &op1,Operand &op2,
   const SegmentRegister &CS,unsigned int offset,const Memory &mem,unsigned int defOperSize,unsigned int defAddrSize) const
{
	FetchInstructionClass<const i486DX,DebugFetchInstructionFunctions>::FetchInstruction(
	    *this,memWin,inst,op1,op2,CS,offset,mem,defOperSize,defAddrSize);
}

inline void i486DX::FetchOperand8(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
{
	inst.operand[inst.operandLen++]=FetchInstructionByte(ptr,inst.codeAddressSize,seg,offset,mem);
	++inst.numBytes;
}
inline void i486DX::PeekOperand8(unsigned int &operand,const Instruction &inst,const MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
{
	operand=PeekInstructionByte(ptr,inst.codeAddressSize,seg,offset,mem);
}
inline void i486DX::FetchOperand16(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
{
	FetchInstructionTwoBytes(inst.operand+inst.operandLen,ptr,inst.codeAddressSize,seg,offset,mem);
	offset+=2;
	inst.operandLen+=2;
	inst.numBytes+=2;
}
inline void i486DX::FetchOperand32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
{
	FetchInstructionFourBytes(inst.operand+inst.operandLen,ptr,inst.codeAddressSize,seg,offset,mem);
	offset+=4;
	inst.operandLen+=4;
	inst.numBytes+=4;
}

inline unsigned int i486DX::FetchOperand16or32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
{
	if(16==inst.operandSize)
	{
		FetchOperand16(inst,ptr,seg,offset,mem);
		return 2;
	}
	else // if(32==inst.operandSize)
	{
		FetchOperand32(inst,ptr,seg,offset,mem);
		return 4;
	}
}

inline void i486DX::DebugFetchOperand8(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	inst.operand[inst.operandLen++]=DebugFetchByte(inst.codeAddressSize,seg,offset,mem);
	++inst.numBytes;
}
inline void i486DX::DebugPeekOperand8(unsigned int &operand,const Instruction &inst,const MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	operand=DebugFetchByte(inst.codeAddressSize,seg,offset,mem);
}
inline void i486DX::DebugFetchOperand16(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	inst.operand[inst.operandLen  ]=DebugFetchByte(inst.codeAddressSize,seg,offset  ,mem);
	inst.operand[inst.operandLen+1]=DebugFetchByte(inst.codeAddressSize,seg,offset+1,mem);
	offset+=2;
	inst.operandLen+=2;
	inst.numBytes+=2;
}
inline void i486DX::DebugFetchOperand32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	inst.operand[inst.operandLen  ]=DebugFetchByte(inst.codeAddressSize,seg,offset  ,mem);
	inst.operand[inst.operandLen+1]=DebugFetchByte(inst.codeAddressSize,seg,offset+1,mem);
	inst.operand[inst.operandLen+2]=DebugFetchByte(inst.codeAddressSize,seg,offset+2,mem);
	inst.operand[inst.operandLen+3]=DebugFetchByte(inst.codeAddressSize,seg,offset+3,mem);
	offset+=4;
	inst.operandLen+=4;
	inst.numBytes+=4;
}

inline unsigned int i486DX::DebugFetchOperand16or32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	if(16==inst.operandSize)
	{
		DebugFetchOperand16(inst,ptr,seg,offset,mem);
		return 2;
	}
	else // if(32==inst.operandSize)
	{
		DebugFetchOperand32(inst,ptr,seg,offset,mem);
		return 4;
	}
}



inline void i486DX::FetchImm8(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
{
	inst.imm[0]=FetchInstructionByte(ptr,inst.codeAddressSize,seg,offset,mem);
	++inst.numBytes;
}
inline void i486DX::FetchImm16(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
{
	FetchInstructionTwoBytes(inst.imm,ptr,inst.codeAddressSize,seg,offset,mem);
	inst.numBytes+=2;
}
inline void i486DX::FetchImm32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
{
	FetchInstructionFourBytes(inst.imm,ptr,inst.codeAddressSize,seg,offset,mem);
	inst.numBytes+=4;
}
inline unsigned int i486DX::FetchImm16or32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
{
	if(16==inst.operandSize)
	{
		FetchImm16(inst,ptr,seg,offset,mem);
		return 2;
	}
	else // if(32==inst.operandSize)
	{
		FetchImm32(inst,ptr,seg,offset,mem);
		return 4;
	}
}

inline void i486DX::DebugFetchImm8(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	inst.imm[0]=DebugFetchByte(inst.codeAddressSize,seg,offset,mem);
	++inst.numBytes;
}
inline void i486DX::DebugFetchImm16(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	inst.imm[0]=DebugFetchByte(inst.codeAddressSize,seg,offset  ,mem);
	inst.imm[1]=DebugFetchByte(inst.codeAddressSize,seg,offset+1,mem);
	inst.numBytes+=2;
}
inline void i486DX::DebugFetchImm32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	inst.imm[0]=DebugFetchByte(inst.codeAddressSize,seg,offset  ,mem);
	inst.imm[1]=DebugFetchByte(inst.codeAddressSize,seg,offset+1,mem);
	inst.imm[2]=DebugFetchByte(inst.codeAddressSize,seg,offset+2,mem);
	inst.imm[3]=DebugFetchByte(inst.codeAddressSize,seg,offset+3,mem);
	inst.numBytes+=4;
}
inline unsigned int i486DX::DebugFetchImm16or32(Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem) const
{
	if(16==inst.operandSize)
	{
		DebugFetchImm16(inst,ptr,seg,offset,mem);
		return 2;
	}
	else // if(32==inst.operandSize)
	{
		DebugFetchImm32(inst,ptr,seg,offset,mem);
		return 4;
	}
}






template <class CPUCLASS,class FUNCCLASS>
unsigned int i486DX::FetchOperandRM(CPUCLASS &cpu,Instruction &inst,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,unsigned int offset,const Memory &mem)
{
	FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset++,mem);

	// [1] Table 26-1, 26-2, 26-3, pp. 26-4,26-5,26-6
	if(16==inst.addressSize)
	{
		/* As Specification
		unsigned int numBytesFetched=1;
		auto MODR_M=inst.operand[inst.operandLen-1];
		auto MOD=(MODR_M>>6)&3;
		auto R_M=(MODR_M)&7;
		if(0b00==MOD && 0b110==R_M) // disp16             CASE 2
		{
			FUNCCLASS::FetchOperand16(cpu,inst,ptr,seg,offset,mem);
			numBytesFetched+=2;
		}
		else if(0b01==MOD)         //                     CASE 1
		{
			FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset,mem);
			++numBytesFetched;
		}
		else if(0b10==MOD)         //                     CASE 2
		{
			FUNCCLASS::FetchOperand16(cpu,inst,ptr,seg,offset,mem);
			numBytesFetched+=2;
		}
		return numBytesFetched;
		*/

		static const char table[256]=
		{
			0,0,0,0,0,0,2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,2,0,
			0,0,0,0,0,0,2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,2,0,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		};
		if(2==table[inst.operand[inst.operandLen-1]])
		{
			FUNCCLASS::FetchOperand16(cpu,inst,ptr,seg,offset,mem);
			return 3; // Fetched 3 bytes
		}
		else if(1==table[inst.operand[inst.operandLen-1]])
		{
			FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset,mem);
			return 2; // Fetched 2 bytes
		}
		return 1; // Fetched 1 bytes
	}
	else // if(32==inst.addressSize)
	{
		/* As Specification
		unsigned int numBytesFetched=1;
		auto MODR_M=inst.operand[inst.operandLen-1];
		auto MOD=(MODR_M>>6)&3;
		auto R_M=(MODR_M)&7;
		if(0b00==MOD)
		{
			if(0b100==R_M) // SIB                         // CASE 1
			{
				FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset,mem);
				++numBytesFetched;
				++offset;

				auto SIB=inst.operand[inst.operandLen-1];
				auto BASE=(SIB&7);
				// Special case MOD=0b00 && BASE==5 [1] Table 26-4 pp.26-7
				// No base, [disp32+scaled_index]
				if(5==BASE)
				{
					FUNCCLASS::FetchOperand32(cpu,inst,ptr,seg,offset,mem);
					numBytesFetched+=4;
				}
			}
			else if(0b101==R_M) // disp32                    CASE 2
			{
				FUNCCLASS::FetchOperand32(cpu,inst,ptr,seg,offset,mem);
				numBytesFetched+=4;
			}
			// else                                          CASE 0
		}
		else if(0b01==MOD)
		{
			if(0b100==R_M) // SIB+disp8                      CASE 3
			{
				FUNCCLASS::FetchOperand16(cpu,inst,ptr,seg,offset,mem);
				numBytesFetched+=2;
			}
			else                                          // CASE 4
			{
				FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset,mem);
				++numBytesFetched;
			}
		}
		else if(0b10==MOD)
		{
			if(0b100==R_M) // SIB+disp32                     CASE 5
			{
				FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset,mem);
				FUNCCLASS::FetchOperand32(cpu,inst,ptr,seg,offset+1,mem);
				numBytesFetched+=5;
			}
			else                                          // CASE 6
			{
				FUNCCLASS::FetchOperand32(cpu,inst,ptr,seg,offset,mem);
				numBytesFetched+=4;
			}
		}
		// else                                              CASE 0
		return numBytesFetched;
		*/

		const static unsigned char table[256]=
		{
			0,0,0,0,1,2,0,0,0,0,0,0,1,2,0,0,0,0,0,0,1,2,0,0,0,0,0,0,1,2,0,0,
			0,0,0,0,1,2,0,0,0,0,0,0,1,2,0,0,0,0,0,0,1,2,0,0,0,0,0,0,1,2,0,0,
			4,4,4,4,3,4,4,4,4,4,4,4,3,4,4,4,4,4,4,4,3,4,4,4,4,4,4,4,3,4,4,4,
			4,4,4,4,3,4,4,4,4,4,4,4,3,4,4,4,4,4,4,4,3,4,4,4,4,4,4,4,3,4,4,4,
			6,6,6,6,5,6,6,6,6,6,6,6,5,6,6,6,6,6,6,6,5,6,6,6,6,6,6,6,5,6,6,6,
			6,6,6,6,5,6,6,6,6,6,6,6,5,6,6,6,6,6,6,6,5,6,6,6,6,6,6,6,5,6,6,6,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		};
		switch(table[inst.operand[inst.operandLen-1]])
		{
		case 1:
			{
				FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset,mem);
				++offset;

				auto SIB=inst.operand[inst.operandLen-1];
				auto BASE=(SIB&7);
				// Special case MOD=0b00 && BASE==5 [1] Table 26-4 pp.26-7
				// No base, [disp32+scaled_index]
				if(5==BASE)
				{
					FUNCCLASS::FetchOperand32(cpu,inst,ptr,seg,offset,mem);
					return 6;
				}
				return 2;
			}
			break;
		case 2:
			FUNCCLASS::FetchOperand32(cpu,inst,ptr,seg,offset,mem);
			return 5;
		case 3:
			FUNCCLASS::FetchOperand16(cpu,inst,ptr,seg,offset,mem);
			return 3;
		case 4:
			FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset,mem);
			return 2;
		case 5:
			FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset,mem);
			FUNCCLASS::FetchOperand32(cpu,inst,ptr,seg,offset+1,mem);
			return 6;
		case 6:
			FUNCCLASS::FetchOperand32(cpu,inst,ptr,seg,offset,mem);
			return 5;
		}
		return 1;
	}
	return 1;
}

std::string i486DX::Instruction::SegmentOverrideString(int segOverridePrefix)
{
	switch(segOverridePrefix)
	{
	case SEG_OVERRIDE_CS://  0x2E,
		return "CS:";
	case SEG_OVERRIDE_SS://  0x36,
		return "SS:";
	case SEG_OVERRIDE_DS://  0x3E,
		return "DS:";
	case SEG_OVERRIDE_ES://  0x26,
		return "ES:";
	case SEG_OVERRIDE_FS://  0x64,
		return "FS:";
	case SEG_OVERRIDE_GS://  0x65,
		return "GS:";
	}
	return "";
}

/* static */ std::string i486DX::Instruction::SegmentOverrideSIorESIString(int segOverridePrefix,int addressSize)
{
	if(0==segOverridePrefix)
	{
		return "";
	}
	auto str=SegmentOverrideString(segOverridePrefix);
	switch(addressSize)
	{
	case 16:
		str+="[SI]";
		break;
	case 32:
	default:
		str+="[ESI]";
		break;
	}
	return str;
}

template <class CPUCLASS,class FUNCCLASS>
void i486DX::FetchOperand(CPUCLASS &cpu,Instruction &inst,Operand &op1,Operand &op2,MemoryAccess::ConstPointer &ptr,const SegmentRegister &seg,int offset,const Memory &mem)
{
	op1.Clear();
	op2.Clear();

	switch(opCodeRenumberTable[inst.opCode])
	{
	case I486_RENUMBER_UNDEFINED_SHOOT_INT6:
		break;

	case I486_RENUMBER_C0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_I8://0xC0,// ::ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		offset+=FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		FUNCCLASS::FetchImm8(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,8,inst.operand);
		break;
	case I486_RENUMBER_C1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_I8:// 0xC1, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		offset+=FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		FUNCCLASS::FetchImm8(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;
	case I486_RENUMBER_D0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_1://0xD0, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_RENUMBER_D2_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_CL://0xD2,// ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		offset+=FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,8,inst.operand);
		break;
	case I486_RENUMBER_D1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_1://0xD1, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_RENUMBER_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_CL://0xD3, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		offset+=FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;


	case I486_RENUMBER_F6_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV: //=0xF6
		offset+=FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		if(0==inst.GetREG()) // TEST RM8,I8
		{
			FUNCCLASS::FetchImm8(cpu,inst,ptr,seg,offset,mem);
		}
		inst.operandSize=8;

		op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;
	case I486_RENUMBER_F7_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV: //=0xF7,
		offset+=FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		if(0==inst.GetREG()) // TEST RM8,I8
		{
			FUNCCLASS::FetchImm16or32(cpu,inst,ptr,seg,offset,mem);
		}

		op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;


	case I486_RENUMBER_AAA: // 0x37
		break;

	case I486_RENUMBER_AAD://    0xD5,
	case I486_RENUMBER_AAM://    0xD4,
		FUNCCLASS::FetchImm8(cpu,inst,ptr,seg,offset,mem);
		break;
	case I486_RENUMBER_AAS:
		break;
	

	case I486_RENUMBER_ARPL://       0x63,
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		inst.operandSize=16;
		op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
		op2.DecodeMODR_MForRegister(inst.operandSize,inst.operand[0]);
		break;


	case I486_RENUMBER_BOUND: // 0x62
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op2.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;


	case I486_RENUMBER_BT_BTS_BTR_BTC_RM_I8:// 0FBA
		offset+=FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		FUNCCLASS::FetchImm8(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;

	case I486_RENUMBER_BSF_R_RM://   0x0FBC,
	case I486_RENUMBER_BSR_R_RM://   0x0FBD,
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.DecodeMODR_MForRegister(inst.operandSize,inst.operand[0]);
		op2.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;
	case I486_RENUMBER_BT_R_RM://    0x0FA3,
	case I486_RENUMBER_BTC_RM_R://   0x0FBB,
	case I486_RENUMBER_BTS_RM_R://   0x0FAB,
	case I486_RENUMBER_BTR_RM_R://   0x0FB3,
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
		op2.DecodeMODR_MForRegister(inst.operandSize,inst.operand[0]);
		break;

	case I486_RENUMBER_LAR:
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.DecodeMODR_MForRegister(inst.operandSize,inst.operand[0]);
		op2.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;

	case I486_RENUMBER_CALL_REL://   0xE8,
	case I486_RENUMBER_JMP_REL://          0xE9,   // cw or cd
		FUNCCLASS::FetchImm16or32(cpu,inst,ptr,seg,offset,mem);
		break;
	case I486_RENUMBER_CALL_FAR://   0x9A,
	case I486_RENUMBER_JMP_FAR:
		offset+=FUNCCLASS::FetchOperand16or32(cpu,inst,ptr,seg,offset,mem);
		FUNCCLASS::FetchOperand16(cpu,inst,ptr,seg,offset,mem);
		op1.DecodeFarAddr(inst.addressSize,inst.operandSize,inst.operand);
		break;


	case I486_RENUMBER_CBW_CWDE://        0x98,
	case I486_RENUMBER_CWD_CDQ://         0x99,
	case I486_RENUMBER_CLC:
	case I486_RENUMBER_CLD:
	case I486_RENUMBER_CLI:
	case I486_RENUMBER_CMC://        0xF5,
	case I486_RENUMBER_CLTS: // 0x0F06
		break;


	case I486_RENUMBER_CMPSB://           0xA6,
		inst.operandSize=8;
		break;
	case I486_RENUMBER_CMPS://            0xA7,
		break;


	case I486_RENUMBER_DAA://             0x27,
	case I486_RENUMBER_DAS://             0x2F,
		break;


	case I486_RENUMBER_DEC_EAX:
	case I486_RENUMBER_DEC_ECX:
	case I486_RENUMBER_DEC_EDX:
	case I486_RENUMBER_DEC_EBX:
	case I486_RENUMBER_DEC_ESP:
	case I486_RENUMBER_DEC_EBP:
	case I486_RENUMBER_DEC_ESI:
	case I486_RENUMBER_DEC_EDI:
		break;


	case I486_RENUMBER_ENTER://      0xC8,
		FUNCCLASS::FetchOperand16(cpu,inst,ptr,seg,offset,mem);
		offset+=2;
		FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset,mem);
		break;


	case I486_RENUMBER_FWAIT://      0x9B,
		break;
	case I486_RENUMBER_FPU_D8_FADD: // 0xD8
		{
			unsigned int MODR_M;
			FUNCCLASS::PeekOperand8(cpu,MODR_M,inst,ptr,seg,offset,mem);
			if(0xD1==MODR_M || // FCOM
			   0xD9==MODR_M || // FCOMP
			   (0xC0<=MODR_M && MODR_M<=0xC7)) // FADD ST,STi
			{
				FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset,mem);
			}
			else
			{
				FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
				op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
			}
		}
		break;
	case I486_RENUMBER_FPU_D9_FNSTCW_M16_FNSTENV_F2XM1_FXAM_FXCH_FXTRACT_FYL2X_FYL2XP1_FABS_:// 0xD9,
		{
			unsigned int MODR_M;
			FUNCCLASS::PeekOperand8(cpu,MODR_M,inst,ptr,seg,offset,mem);
			if((0xC0<=MODR_M && MODR_M<=0xC7) ||
			   0xE0==MODR_M ||
			   0xE5==MODR_M || // FXAM
			   0xE8==MODR_M ||
			   0xE9==MODR_M ||
			   0xEE==MODR_M ||
			   (0xF0<=MODR_M && MODR_M<=0xFF))
			{
				FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset,mem);
			}
			else
			{
				FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
				op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
			}
		}
		break;
	case I486_RENUMBER_FPU_DB_FNINIT_FRSTOR://     0xDB, 
		{
			unsigned int MODR_M;
			FUNCCLASS::PeekOperand8(cpu,MODR_M,inst,ptr,seg,offset,mem);
			if(0xE3==MODR_M) // FNINIT
			{
				FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset,mem);
			}
			else
			{
				switch(Instruction::GetREG(MODR_M))
				{
				case 3: // FISTP m32int
				case 5: // FLD m80real
				case 7: // FSTP m80real
					FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
					op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
					break;
				case 0:
				case 1:
				case 2:
				case 4:
				case 6:
				default:
					FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset,mem);
					break;
				}
			}
		}
		break;
	case I486_RENUMBER_FPU_DC_FADD:
		{
			unsigned int MODR_M;
			FUNCCLASS::PeekOperand8(cpu,MODR_M,inst,ptr,seg,offset,mem);
			{
				switch(Instruction::GetREG(MODR_M))
				{
				case 0: // FADD m64real
				case 1: // FMUL m64real
				case 3: // FCOMP m64real
				case 5: // FSUBR m64real
				case 7: // FDIVR m64real
					FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
					op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
					break;
				default:
					FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset,mem);
					break;
				}
			}
		}
		break;
	case I486_RENUMBER_FPU_DD_FLD_FSAVE_FST_FNSTSW_M16_FFREE_FUCOM:
		{
			unsigned int MODR_M;
			FUNCCLASS::PeekOperand8(cpu,MODR_M,inst,ptr,seg,offset,mem);
			if(0xD0==(MODR_M&0xF8)) // D0 11010xxx    [1] pp.151  0<=i<=7
			{
				FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset,mem);   // FST
			}
			else if(0xD8==(MODR_M&0xF8)) // D8 11011xxx
			{
				FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset,mem);   // FSTP
			}
			else if(0xC0==(MODR_M&0xF8)) // C0 11000xxx
			{
				FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset,mem);   // FFREE
			}
			else if(0xE0==(MODR_M&0xF8) || 0xE1==(MODR_M&0xF8) || 0xE8==(MODR_M&0xF8) || 0xE9==(MODR_M&0xF8))
			{
				FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset,mem);   // FUCOM
			}
			else
			{
				switch(Instruction::GetREG(MODR_M))
				{
				default:
					FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset,mem);
					break;
				// case 2: // FST m64real
				// case 6: // FSAVE m94/108byte
					break;
				case 0:	// FLD m64real
				case 3: // FSTP m64real
				case 7: // FNSTSW m2byte
					FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
					op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
					break;
				}
			}
		}
		break;

	case I486_RENUMBER_FPU_DE:
		{
			unsigned int MODR_M;
			FUNCCLASS::PeekOperand8(cpu,MODR_M,inst,ptr,seg,offset,mem);
			if(0xC9==MODR_M ||
			   0xF1==MODR_M ||
			  (0xF8<=MODR_M && MODR_M<=0xFF) ||
			   0xD9==MODR_M ||
			  (0xC0<=MODR_M && MODR_M<=0xC7))
			{
				// 0xD9:FCOMPP
				// 0xF9:FDIV
				// 0xC0 to 0xC7:FADDP
				FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset,mem);
			}
			else
			{
				FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
				op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
			}
		}
		break;

	case I486_RENUMBER_FPU_DF_FNSTSW_AX://  0xDF,
		{
			unsigned int MODR_M;
			FUNCCLASS::PeekOperand8(cpu,MODR_M,inst,ptr,seg,offset,mem);
			if(0xE0==MODR_M) // FNSTSW
			{
				FUNCCLASS::FetchOperand8(cpu,inst,ptr,seg,offset,mem);
			}
			else
			{
				FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
				op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
				switch(Instruction::GetREG(MODR_M))
				{
				case 0:
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				default:
					break;
				}
			}
		}
		break;


	case I486_RENUMBER_INSB://     0x6C,
		break;


	case I486_RENUMBER_IN_AL_I8://=        0xE4,
	case I486_RENUMBER_IN_A_I8://=         0xE5,
		FUNCCLASS::FetchImm8(cpu,inst,ptr,seg,offset,mem);
		break;
	case I486_RENUMBER_IN_AL_DX://=        0xEC,
	case I486_RENUMBER_IN_A_DX://=         0xED,
		break;


	case I486_RENUMBER_IMUL_R_RM_I8://0x6B,
		offset+=FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		FUNCCLASS::FetchImm8(cpu,inst,ptr,seg,offset,mem);
		op1.DecodeMODR_MForRegister(inst.operandSize,inst.operand[0]);
		op2.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;
	case I486_RENUMBER_IMUL_R_RM_IMM://0x69,
		offset+=FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		if(16==inst.operandSize)
		{
			FUNCCLASS::FetchImm16(cpu,inst,ptr,seg,offset,mem);
		}
		else
		{
			FUNCCLASS::FetchImm32(cpu,inst,ptr,seg,offset,mem);
		}
		op1.DecodeMODR_MForRegister(inst.operandSize,inst.operand[0]);
		op2.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;
	case I486_RENUMBER_IMUL_R_RM://       0x0FAF,
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.DecodeMODR_MForRegister(inst.operandSize,inst.operand[0]);
		op2.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;


	case I486_RENUMBER_LEAVE://            0xC9,
		break;


	case I486_RENUMBER_HLT://        0xF4,
		break;


	case I486_RENUMBER_INC_DEC_R_M8:
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,8,inst.operand);
		break;
	case I486_RENUMBER_INC_DEC_CALL_CALLF_JMP_JMPF_PUSH:
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;
	case I486_RENUMBER_INC_EAX://    0x40, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_RENUMBER_INC_ECX://    0x41, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_RENUMBER_INC_EDX://    0x42, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_RENUMBER_INC_EBX://    0x43, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_RENUMBER_INC_ESP://    0x44, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_RENUMBER_INC_EBP://    0x45, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_RENUMBER_INC_ESI://    0x46, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_RENUMBER_INC_EDI://    0x47, // 16/32 depends on OPSIZE_OVERRIDE
		break;


	case I486_RENUMBER_IRET://   0xCF,
		break;


	case I486_RENUMBER_INT3://       0xCC,
		break;
	case I486_RENUMBER_INT://        0xCD,
		FUNCCLASS::FetchImm8(cpu, inst, ptr, seg, offset, mem);
		break;
	case I486_RENUMBER_INTO://       0xCE,
		break;


	case I486_RENUMBER_JMP_REL8://         0xEB,   // cb
	case I486_RENUMBER_JO_REL8:   // 0x70,
	case I486_RENUMBER_JNO_REL8:  // 0x71,
	case I486_RENUMBER_JB_REL8:   // 0x72,
	case I486_RENUMBER_JAE_REL8:  // 0x73,
	case I486_RENUMBER_JE_REL8:   // 0x74,
	case I486_RENUMBER_JECXZ_REL8:// 0xE3,  // Depending on the operand size
	case I486_RENUMBER_JNE_REL8:  // 0x75,
	case I486_RENUMBER_JBE_REL8:  // 0x76,
	case I486_RENUMBER_JA_REL8:   // 0x77,
	case I486_RENUMBER_JS_REL8:   // 0x78,
	case I486_RENUMBER_JNS_REL8:  // 0x79,
	case I486_RENUMBER_JP_REL8:   // 0x7A,
	case I486_RENUMBER_JNP_REL8:  // 0x7B,
	case I486_RENUMBER_JL_REL8:   // 0x7C,
	case I486_RENUMBER_JGE_REL8:  // 0x7D,
	case I486_RENUMBER_JLE_REL8:  // 0x7E,
	case I486_RENUMBER_JG_REL8:   // 0x7F,
		FUNCCLASS::FetchImm8(cpu,inst,ptr,seg,offset,mem);
		break;


	case I486_RENUMBER_JA_REL://    0x0F87,
	case I486_RENUMBER_JAE_REL://   0x0F83,
	case I486_RENUMBER_JB_REL://    0x0F82,
	case I486_RENUMBER_JBE_REL://   0x0F86,
	// case I486_RENUMBER_JC_REL://    0x0F82, Same as JB_REL
	case I486_RENUMBER_JE_REL://    0x0F84,
	// case I486_RENUMBER_JZ_REL://    0x0F84, Same as JZ_REL
	case I486_RENUMBER_JG_REL://    0x0F8F,
	case I486_RENUMBER_JGE_REL://   0x0F8D,
	case I486_RENUMBER_JL_REL://    0x0F8C,
	case I486_RENUMBER_JLE_REL://   0x0F8E,
	// case I486_RENUMBER_JNA_REL://   0x0F86, Same as JBE_REL
	// case I486_RENUMBER_JNAE_REL://  0x0F82, Same as JB_REL
	// case I486_RENUMBER_JNB_REL://   0x0F83, Same as JAE_REL
	// case I486_RENUMBER_JNBE_REL://  0x0F87, Same as JA_REL
	// case I486_RENUMBER_JNC_REL://   0x0F83, Same as JAE_REL
	case I486_RENUMBER_JNE_REL://   0x0F85,
	// case I486_RENUMBER_JNG_REL://   0x0F8E, Same as JLE_REL
	// case I486_RENUMBER_JNGE_REL://  0x0F8C, Same as JL_REL
	// case I486_RENUMBER_JNL_REL://   0x0F8D, Same as JGE_REL
	// case I486_RENUMBER_JNLE_REL://  0x0F8F, Same as JG_REL
	case I486_RENUMBER_JNO_REL://   0x0F81,
	case I486_RENUMBER_JNP_REL://   0x0F8B,
	case I486_RENUMBER_JNS_REL://   0x0F89,
	// case I486_RENUMBER_JNZ_REL://   0x0F85, Same as JNE_REL
	case I486_RENUMBER_JO_REL://    0x0F80,
	case I486_RENUMBER_JP_REL://    0x0F8A,
	// case I486_RENUMBER_JPE_REL://   0x0F8A, Same as JP_REL
	// case I486_RENUMBER_JPO_REL://   0x0F8B, Same as JNP_REL
	case I486_RENUMBER_JS_REL://    0x0F88,
		FUNCCLASS::FetchImm16or32(cpu,inst,ptr,seg,offset,mem);
		break;


	case I486_RENUMBER_BINARYOP_RM8_FROM_I8:
	case I486_RENUMBER_BINARYOP_RM8_FROM_I8_ALIAS:
		offset+=FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		FUNCCLASS::FetchImm8(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,8,inst.operand);
		break;
	case I486_RENUMBER_BINARYOP_R_FROM_I:
		offset+=FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		FUNCCLASS::FetchImm16or32(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;
	case I486_RENUMBER_BINARYOP_RM_FROM_SXI8:
		offset+=FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		FUNCCLASS::FetchImm8(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;


	case I486_RENUMBER_LGDT_LIDT_SGDT_SIDT:
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		if(4==inst.GetREG() || 6==inst.GetREG())
		{
			inst.operandSize=16;
		}
		op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;


	case I486_RENUMBER_LAHF://=             0x9F,
		break;


	case I486_RENUMBER_LEA://=              0x8D,
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.DecodeMODR_MForRegister(inst.operandSize,inst.operand[0]);
		op2.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;


	case I486_RENUMBER_LDS://              0xC5,
	case I486_RENUMBER_LSS://              0x0FB2,
	case I486_RENUMBER_LES://              0xC4,
	case I486_RENUMBER_LFS://              0x0FB4,
	case I486_RENUMBER_LGS://              0x0FB5,
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.DecodeMODR_MForRegister(inst.operandSize,inst.operand[0]);
		op2.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;


	case I486_RENUMBER_LODSB://            0xAC,
	case I486_RENUMBER_LODS://             0xAD,
		break;


	case I486_RENUMBER_LOOP://             0xE2,
	case I486_RENUMBER_LOOPE://            0xE1,
	case I486_RENUMBER_LOOPNE://           0xE0,
		FUNCCLASS::FetchImm8(cpu,inst,ptr,seg,offset,mem);
		break;


	case I486_RENUMBER_LSL://              0x0F03,
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.DecodeMODR_MForRegister(inst.operandSize,inst.operand[0]);
		op2.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;


	case I486_RENUMBER_MOV_FROM_R8: //      0x88,
		// Example:  88 4c ff        MOV CL,[SI-1]     In Real Mode
		// Example:  88 10           MOV DL,[BX+SI]    In Real Mode
		// Example:  88 36 21 00     MOV DH,[021H]     In Real Mode
		// Example:  67 88 26 61 10  MOV [1061],AH     In Protected Mode -> disp16 may become disp32, and vise-versa

		// Example:  8D 04 C1        LEA EAX,[ECX+EAX*8] In Protected Mode
		// Example:  8D 04 41        LEA EAX,[ECX+EAX*2] In Protected Mode
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,8,inst.operand);
		break;
	case I486_RENUMBER_MOV_FROM_R: //       0x89, // 16/32 depends on OPSIZE_OVERRIDE
		// Example:  89 26 3e 00     MOV [003EH],SP
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;
	case I486_RENUMBER_MOV_TO_R8: //        0x8A,
		// Example:  8a 0e 16 00     MOV CL,[0016H]
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op2.Decode(inst.addressSize,8,inst.operand);
		break;
	case I486_RENUMBER_MOV_TO_R: //         0x8B, // 16/32 depends on OPSIZE_OVERRIDE
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op2.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;


	case I486_RENUMBER_MOV_FROM_SEG: //     0x8C,
		// Example:  8c c6           MOV SI,ES
		// Sreg: ES=0, CS=1, SS=2, DS=3, FD=4, GS=5 (OPCODE part of MODR_M)  [1] pp.26-10
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		// From observation on 2020/04/22, if the upper-16 bit of the destination register is undefined
		// F-BASIC386 booted from CD may fail to open windows.
		// From observation on 2020/10/24, if the operandSize is 32, upper 32-bit of the destination
		// register must be clear instead of being preserved.
		// Therefore, MOV AX,DS and MOV EAX,DS probably should behave differently.
		// Hence the following clause "Force it to be 16-bit" is reverted and commented out.
		// inst.operandSize=16; // Force it to be 16-bit
		op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
		op2.DecodeMODR_MForSegmentRegister(inst.operand[0]);
		break;
	case I486_RENUMBER_MOV_TO_SEG: //       0x8E,
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		inst.operandSize=16; // Force it to be 16-bit
		op2.Decode(inst.addressSize,inst.operandSize,inst.operand);
		op1.DecodeMODR_MForSegmentRegister(inst.operand[0]);
		break;

	case I486_RENUMBER_MOV_M_TO_AL: //      0xA0, // 16/32 depends on ADDRESSSIZE_OVERRIDE
		switch(inst.addressSize)
		{
		default:
			FUNCCLASS::FetchImm32(cpu,inst,ptr,seg,offset,mem);
			break;
		case 16:
			FUNCCLASS::FetchImm16(cpu,inst,ptr,seg,offset,mem);
			break;
		}
		break;
	case I486_RENUMBER_MOV_M_TO_EAX: //     0xA1, // 16/32 depends on ADDRESSSIZE_OVERRIDE
		switch(inst.addressSize)
		{
		default:
			FUNCCLASS::FetchImm32(cpu,inst,ptr,seg,offset,mem);
			break;
		case 16:
			FUNCCLASS::FetchImm16(cpu,inst,ptr,seg,offset,mem);
			break;
		}
		break;
	case I486_RENUMBER_MOV_M_FROM_AL: //    0xA2, // 16/32 depends on ADDRESSSIZE_OVERRIDE
		switch(inst.addressSize)
		{
		default:
			FUNCCLASS::FetchImm32(cpu,inst,ptr,seg,offset,mem);
			break;
		case 16:
			FUNCCLASS::FetchImm16(cpu,inst,ptr,seg,offset,mem);
			break;
		}
		break;
	case I486_RENUMBER_MOV_M_FROM_EAX: //   0xA3, // 16/32 depends on ADDRESSSIZE_OVERRIDE
		switch(inst.addressSize)
		{
		default:
			FUNCCLASS::FetchImm32(cpu,inst,ptr,seg,offset,mem);
			break;
		case 16:
			FUNCCLASS::FetchImm16(cpu,inst,ptr,seg,offset,mem);
			break;
		}
		break;

	case I486_RENUMBER_MOV_I8_TO_AL: //     0xB0,
	case I486_RENUMBER_MOV_I8_TO_CL: //     0xB1,
	case I486_RENUMBER_MOV_I8_TO_DL: //     0xB2,
	case I486_RENUMBER_MOV_I8_TO_BL: //     0xB3,
	case I486_RENUMBER_MOV_I8_TO_AH: //     0xB4,
	case I486_RENUMBER_MOV_I8_TO_CH: //     0xB5,
	case I486_RENUMBER_MOV_I8_TO_DH: //     0xB6,
	case I486_RENUMBER_MOV_I8_TO_BH: //     0xB7,
		FUNCCLASS::FetchImm8(cpu,inst,ptr,seg,offset,mem);
		break;
	case I486_RENUMBER_MOV_I_TO_EAX: //   0xB8, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_RENUMBER_MOV_I_TO_ECX: //   0xB9, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_RENUMBER_MOV_I_TO_EDX: //   0xBA, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_RENUMBER_MOV_I_TO_EBX: //   0xBB, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_RENUMBER_MOV_I_TO_ESP: //   0xBC, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_RENUMBER_MOV_I_TO_EBP: //   0xBD, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_RENUMBER_MOV_I_TO_ESI: //   0xBE, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_RENUMBER_MOV_I_TO_EDI: //   0xBF, // 16/32 depends on OPSIZE_OVERRIDE
		FUNCCLASS::FetchImm16or32(cpu,inst,ptr,seg,offset,mem);
		break;
	case I486_RENUMBER_MOV_I8_TO_RM8: //    0xC6,
		offset+=FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		FUNCCLASS::FetchImm8(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,8,inst.operand);
		break;
	case I486_RENUMBER_MOV_I_TO_RM: //      0xC7, // 16/32 depends on OPSIZE_OVERRIDE
		offset+=FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		FUNCCLASS::FetchImm16or32(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;


	case I486_RENUMBER_MOV_TO_CR://        0x0F22,
		inst.operandSize=32; // [1] pp.26-213 32bit operands are always used with these instructions, 
		                     //      regardless of the opreand-size attribute.
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.DecodeMODR_MForCRRegister(inst.operand[0]);
		op2.Decode(inst.addressSize,32,inst.operand);
		break;
	case I486_RENUMBER_MOV_TO_DR://        0x0F23,
		inst.operandSize=32; // [1] pp.26-213 32bit operands are always used with these instructions, 
		                     //      regardless of the opreand-size attribute.
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.DecodeMODR_MForDRRegister(inst.operand[0]);
		op2.Decode(inst.addressSize,32,inst.operand);
		break;
	case I486_RENUMBER_MOV_TO_TR://        0x0F26,
		inst.operandSize=32; // [1] pp.26-213 32bit operands are always used with these instructions, 
		                     //      regardless of the opreand-size attribute.
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.DecodeMODR_MForTestRegister(inst.operand[0]);
		op2.Decode(inst.addressSize,32,inst.operand);
		break;
	case I486_RENUMBER_MOV_FROM_CR://      0x0F20,
		inst.operandSize=32; // [1] pp.26-213 32bit operands are always used with these instructions, 
		                     //      regardless of the opreand-size attribute.
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,32,inst.operand);
		op2.DecodeMODR_MForCRRegister(inst.operand[0]);
		break;
	case I486_RENUMBER_MOV_FROM_DR://      0x0F21,
		inst.operandSize=32; // [1] pp.26-213 32bit operands are always used with these instructions, 
		                     //      regardless of the opreand-size attribute.
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,32,inst.operand);
		op2.DecodeMODR_MForDRRegister(inst.operand[0]);
		break;
	case I486_RENUMBER_MOV_FROM_TR://      0x0F24,
		inst.operandSize=32; // [1] pp.26-213 32bit operands are always used with these instructions, 
		                     //      regardless of the opreand-size attribute.
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,32,inst.operand);
		op2.DecodeMODR_MForTestRegister(inst.operand[0]);
		break;


	case I486_RENUMBER_MOVSB://            0xA4,
	case I486_RENUMBER_MOVS://             0xA5,
		break;


	case I486_RENUMBER_MOVSX_R_RM8://=      0x0FBE,
	case I486_RENUMBER_MOVZX_R_RM8://=      0x0FB6,
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.DecodeMODR_MForRegister(inst.operandSize,inst.operand[0]);
		op2.Decode(inst.addressSize,8,inst.operand);
		break;
	case I486_RENUMBER_MOVSX_R32_RM16://=   0x0FBF,
	case I486_RENUMBER_MOVZX_R32_RM16://=   0x0FB7,
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.DecodeMODR_MForRegister(32,inst.operand[0]);
		op2.Decode(inst.addressSize,16,inst.operand);
		break;


	case I486_RENUMBER_NOP://              0x90,
		break;


	case I486_RENUMBER_OUT_I8_AL: //        0xE6,
	case I486_RENUMBER_OUT_I8_A: //         0xE7,
		FUNCCLASS::FetchImm8(cpu,inst,ptr,seg,offset,mem);
		break;
	case I486_RENUMBER_OUT_DX_AL: //        0xEE,
	case I486_RENUMBER_OUT_DX_A: //         0xEF,
		break;


	case I486_RENUMBER_OUTSB://            0x6E,
	case I486_RENUMBER_OUTS://             0x6F,
		break;


	case I486_RENUMBER_PUSHA://            0x60,
	case I486_RENUMBER_PUSHF://            0x9C,
		break;


	case I486_RENUMBER_PUSH_EAX://         0x50,
	case I486_RENUMBER_PUSH_ECX://         0x51,
	case I486_RENUMBER_PUSH_EDX://         0x52,
	case I486_RENUMBER_PUSH_EBX://         0x53,
	case I486_RENUMBER_PUSH_ESP://         0x54,
	case I486_RENUMBER_PUSH_EBP://         0x55,
	case I486_RENUMBER_PUSH_ESI://         0x56,
	case I486_RENUMBER_PUSH_EDI://         0x57,
		break;
	case I486_RENUMBER_PUSH_I8://          0x6A,
		FUNCCLASS::FetchImm8(cpu,inst,ptr,seg,offset,mem);
		break;
	case I486_RENUMBER_PUSH_I://           0x68,
		FUNCCLASS::FetchImm16or32(cpu,inst,ptr,seg,offset,mem);
		break;
	case I486_RENUMBER_PUSH_CS://          0x0E,
	case I486_RENUMBER_PUSH_SS://          0x16,
	case I486_RENUMBER_PUSH_DS://          0x1E,
	case I486_RENUMBER_PUSH_ES://          0x06,
	case I486_RENUMBER_PUSH_FS://          0x0FA0,
	case I486_RENUMBER_PUSH_GS://          0x0FA8,
		break;


	case I486_RENUMBER_POP_M://            0x8F,
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;
	case I486_RENUMBER_POP_EAX://          0x58,
	case I486_RENUMBER_POP_ECX://          0x59,
	case I486_RENUMBER_POP_EDX://          0x5A,
	case I486_RENUMBER_POP_EBX://          0x5B,
	case I486_RENUMBER_POP_ESP://          0x5C,
	case I486_RENUMBER_POP_EBP://          0x5D,
	case I486_RENUMBER_POP_ESI://          0x5E,
	case I486_RENUMBER_POP_EDI://          0x5F,
	case I486_RENUMBER_POP_SS://           0x17,
	case I486_RENUMBER_POP_DS://           0x1F,
	case I486_RENUMBER_POP_ES://           0x07,
	case I486_RENUMBER_POP_FS://           0x0FA1,
	case I486_RENUMBER_POP_GS://           0x0FA9,

	case I486_RENUMBER_POPA://             0x61,
	case I486_RENUMBER_POPF://             0x9D,
		break;


	case I486_RENUMBER_RET://              0xC3,
	case I486_RENUMBER_RETF://             0xCB,
		break;
	case I486_RENUMBER_RET_I16://          0xC2,
	case I486_RENUMBER_RETF_I16://         0xCA,
		FUNCCLASS::FetchImm16(cpu,inst,ptr,seg,offset,mem);
		break;


	case I486_RENUMBER_SAHF://=             0x9E,
		break;


	case I486_RENUMBER_SHLD_RM_I8://       0x0FA4,
	case I486_RENUMBER_SHRD_RM_I8://       0x0FAC,
		offset+=FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		FUNCCLASS::FetchImm8(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
		op2.DecodeMODR_MForRegister(inst.operandSize,inst.operand[0]);
		break;
	case I486_RENUMBER_SHLD_RM_CL://       0x0FA5,
	case I486_RENUMBER_SHRD_RM_CL://       0x0FAD,
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
		op2.DecodeMODR_MForRegister(inst.operandSize,inst.operand[0]);
		break;


	case I486_RENUMBER_SCASB://            0xAE,
	case I486_RENUMBER_SCAS://             0xAF,
		break;


	case I486_RENUMBER_SETA://             0x0F97,
	case I486_RENUMBER_SETAE://            0x0F93,
	case I486_RENUMBER_SETB://             0x0F92,
	case I486_RENUMBER_SETBE://            0x0F96,
	// I486_OPCODE_SETC://             0x0F92,
	case I486_RENUMBER_SETE://             0x0F94,
	case I486_RENUMBER_SETG://             0x0F9F,
	case I486_RENUMBER_SETGE://            0x0F9D,
	case I486_RENUMBER_SETL://             0x0F9C,
	case I486_RENUMBER_SETLE://            0x0F9E,
	//I486_OPCODE_SETNA://            0x0F96,
	//I486_OPCODE_SETNAE://           0x0F92,
	//I486_OPCODE_SETNB://            0x0F93,
	//I486_OPCODE_SETNBE://           0x0F97,
	//I486_OPCODE_SETNC://            0x0F93,
	case I486_RENUMBER_SETNE://            0x0F95,
	//I486_OPCODE_SETNG://            0x0F9E,
	//I486_OPCODE_SETNGE://           0x0F9C,
	//I486_OPCODE_SETNL://            0x0F9D,
	//I486_OPCODE_SETNLE://           0x0F9F,
	case I486_RENUMBER_SETNO://            0x0F91,
	case I486_RENUMBER_SETNP://            0x0F9B,
	case I486_RENUMBER_SETNS://            0x0F99,
	// I486_OPCODE_SETNZ://            0x0F95,
	case I486_RENUMBER_SETO://             0x0F90,
	case I486_RENUMBER_SETP://             0x0F9A,
	//I486_OPCODE_SETPE://            0x0F9A,
	//I486_OPCODE_SETPO://            0x0F9B,
	case I486_RENUMBER_SETS://             0x0F98,
	// I486_OPCODE_SETZ://             0x0F94,
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		inst.operandSize=8;
		op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;


	case I486_RENUMBER_SLDT_STR_LLDT_LTR_VERR_VERW://             0x0F00,
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		inst.operandSize=16;
		op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;


	case I486_RENUMBER_STC://              0xF9,
	case I486_RENUMBER_STD://              0xFD,
	case I486_RENUMBER_STI://              0xFB,
		break;


	case I486_RENUMBER_STOSB://            0xAA,
	case I486_RENUMBER_STOS://             0xAB,
		break;


	case I486_RENUMBER_ADC_AL_FROM_I8://  0x14,
	case I486_RENUMBER_ADD_AL_FROM_I8://  0x04,
	case I486_RENUMBER_AND_AL_FROM_I8://  0x24,
	case I486_RENUMBER_CMP_AL_FROM_I8://  0x3C,
	case I486_RENUMBER_OR_AL_FROM_I8://  0x0C,
	case I486_RENUMBER_SBB_AL_FROM_I8://  0x1C,
	case I486_RENUMBER_SUB_AL_FROM_I8://  0x2C,
	case I486_RENUMBER_TEST_AL_FROM_I8://  0xA8,
	case I486_RENUMBER_XOR_AL_FROM_I8:
		FUNCCLASS::FetchImm8(cpu,inst,ptr,seg,offset,mem);
		break;
	case I486_RENUMBER_ADC_A_FROM_I://    0x15,
	case I486_RENUMBER_ADD_A_FROM_I://    0x05,
	case I486_RENUMBER_AND_A_FROM_I://    0x25,
	case I486_RENUMBER_CMP_A_FROM_I://    0x3D,
	case I486_RENUMBER_OR_A_FROM_I://    0x0D,
	case I486_RENUMBER_SBB_A_FROM_I://    0x1D,
	case I486_RENUMBER_SUB_A_FROM_I://    0x2D,
	case I486_RENUMBER_TEST_A_FROM_I://    0xA9,
	case I486_RENUMBER_XOR_A_FROM_I:
		FUNCCLASS::FetchImm16or32(cpu,inst,ptr,seg,offset,mem);
		break;
	case I486_RENUMBER_ADC_RM8_FROM_R8:// 0x10,
	case I486_RENUMBER_ADD_RM8_FROM_R8:// 0x00,
	case I486_RENUMBER_AND_RM8_FROM_R8:// 0x20,
	case I486_RENUMBER_CMP_RM8_FROM_R8:// 0x38,
	case I486_RENUMBER_OR_RM8_FROM_R8:// 0x08,
	case I486_RENUMBER_SBB_RM8_FROM_R8:// 0x18,
	case I486_RENUMBER_SUB_RM8_FROM_R8:// 0x28,
	case I486_RENUMBER_TEST_RM8_FROM_R8:// 0x84,
	case I486_RENUMBER_XOR_RM8_FROM_R8:
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		// op2.DecodeMODR_MForRegister(8,inst.operand[0]);
		op1.Decode(inst.addressSize,8,inst.operand);
		break;

	case I486_RENUMBER_ADC_RM_FROM_R://   0x11,
	case I486_RENUMBER_ADD_RM_FROM_R://   0x01,
	case I486_RENUMBER_AND_RM_FROM_R://   0x21,
	case I486_RENUMBER_CMP_RM_FROM_R://   0x39,
	case I486_RENUMBER_OR_RM_FROM_R://   0x09,
	case I486_RENUMBER_SBB_RM_FROM_R://   0x19,
	case I486_RENUMBER_SUB_RM_FROM_R://   0x29,
	case I486_RENUMBER_TEST_RM_FROM_R://   0x85,
	case I486_RENUMBER_XOR_RM_FROM_R:
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op2.DecodeMODR_MForRegister(inst.operandSize,inst.operand[0]);
		op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;

	case I486_RENUMBER_ADC_R8_FROM_RM8:// 0x12,
	case I486_RENUMBER_ADD_R8_FROM_RM8:// 0x02,
	case I486_RENUMBER_AND_R8_FROM_RM8:// 0x22,
	case I486_RENUMBER_CMP_R8_FROM_RM8:// 0x3A,
	case I486_RENUMBER_OR_R8_FROM_RM8:// 0x0A,
	case I486_RENUMBER_SBB_R8_FROM_RM8:// 0x1A,
	case I486_RENUMBER_SUB_R8_FROM_RM8:// 0x2A,
	case I486_RENUMBER_XOR_R8_FROM_RM8:
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		// op1.DecodeMODR_MForRegister(8,inst.operand[0]);
		op2.Decode(inst.addressSize,8,inst.operand);
		break;

	case I486_RENUMBER_ADC_R_FROM_RM://   0x13,
	case I486_RENUMBER_ADD_R_FROM_RM://   0x03,
	case I486_RENUMBER_AND_R_FROM_RM://   0x23,
	case I486_RENUMBER_CMP_R_FROM_RM://   0x3B,
	case I486_RENUMBER_OR_R_FROM_RM://   0x0B,
	case I486_RENUMBER_SBB_R_FROM_RM://   0x1B,
	case I486_RENUMBER_SUB_R_FROM_RM://   0x2B,
	case I486_RENUMBER_XOR_R_FROM_RM:
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.DecodeMODR_MForRegister(inst.operandSize,inst.operand[0]);
		op2.Decode(inst.addressSize,inst.operandSize,inst.operand);
		break;


	case I486_RENUMBER_XCHG_EAX_ECX://     0x91,
	case I486_RENUMBER_XCHG_EAX_EDX://     0x92,
	case I486_RENUMBER_XCHG_EAX_EBX://     0x93,
	case I486_RENUMBER_XCHG_EAX_ESP://     0x94,
	case I486_RENUMBER_XCHG_EAX_EBP://     0x95,
	case I486_RENUMBER_XCHG_EAX_ESI://     0x96,
	case I486_RENUMBER_XCHG_EAX_EDI://     0x97,
		// No operand
		break;
	case I486_RENUMBER_XCHG_RM8_R8://      0x86,
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,8,inst.operand);
		op2.DecodeMODR_MForRegister(8,inst.operand[0]);
		break;
	case I486_RENUMBER_XCHG_RM_R://        0x87,
		FetchOperandRM<CPUCLASS,FUNCCLASS>(cpu,inst,ptr,seg,offset,mem);
		op1.Decode(inst.addressSize,inst.operandSize,inst.operand);
		op2.DecodeMODR_MForRegister(inst.operandSize,inst.operand[0]);
		break;


	case I486_RENUMBER_XLAT://             0xD7,
		break;


	default:
		// Undefined operand, or probably not implemented yet.
		break;
	}
}

std::string i486DX::Instruction::Disassemble(const Operand &op1In,const Operand &op2In,SegmentRegister cs,unsigned int eip,const i486SymbolTable &symTable,const std::map <unsigned int,std::string> &ioTable) const
{
	std::string disasm;
	Operand op1=op1In,op2=op2In;
	std::string op1SizeQual,op2SizeQual;
	std::string op1SegQual,op2SegQual;

	switch(opCode)
	{
	case I486_OPCODE_UNDEFINED_SHOOT_INT6:
		disasm="UNDEFINED_INSTRUCTION_SHOOT_INT6";
		break;

	case I486_OPCODE_C0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_I8://=0xC0,// ::ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_OPCODE_C1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_I8:// =0xC1, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_OPCODE_D0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_1://=0xD0, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_OPCODE_D1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_1://=0xD1, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_OPCODE_D2_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_CL://0xD2,// ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	case I486_OPCODE_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_CL://0xD3, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		switch(GetREG())
		{
		case 0:
			disasm="ROL";
			break;
		case 1:
			disasm="ROR";
			break;
		case 2:
			disasm="RCL";
			break;
		case 3:
			disasm="RCR";
			break;
		case 4:
			disasm="SHL";
			break;
		case 5:
			disasm="SHR";
			break;
		case 6:
			disasm=cpputil::Ubtox(opCode)+"?";
			break;
		case 7:
			disasm="SAR";
			break;
		}
		switch(opCode)
		{
		case I486_OPCODE_C0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_I8://=0xC0,// ::ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
			disasm=DisassembleTypicalRM8_I8(disasm,op1,EvalUimm8());
			break;
		case I486_OPCODE_C1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_I8:// =0xC1, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
			disasm=DisassembleTypicalRM_I8(disasm,op1,EvalUimm8());
			break;
		case I486_OPCODE_D0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_1://=0xD0, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
			disasm = DisassembleTypicalOneOperand(disasm, op1, 8) + ",1";
			break;
		case I486_OPCODE_D1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_1://=0xD1, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
			disasm=DisassembleTypicalOneOperand(disasm,op1,operandSize)+",1";
			break;
		case I486_OPCODE_D2_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_CL://0xD2,// ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
			disasm = DisassembleTypicalOneOperand(disasm, op1, 8) + ",CL";
			break;
		case I486_OPCODE_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_CL://0xD3, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
			disasm=DisassembleTypicalOneOperand(disasm,op1,operandSize)+",CL";
			break;
		}
		break;

	case I486_OPCODE_AAA: // 0x37
		disasm="AAA";
		break;

	case I486_OPCODE_AAD://    0xD5,
		if(0x0A==EvalUimm8())
		{
			disasm="AAD";
		}
		else
		{
			disasm=DisassembleTypicalOneImm("AAD",EvalUimm8(),8);
		}
		break;
	case I486_OPCODE_AAM://    0xD4,
		if(0x0A==EvalUimm8())
		{
			disasm="AAM";
		}
		else
		{
			disasm=DisassembleTypicalOneImm("AAM",EvalUimm8(),8);
		}
		break;
	case I486_OPCODE_AAS:
		disasm="AAS";
		break;

	case I486_OPCODE_ARPL://       0x63,
		disasm=DisassembleTypicalTwoOperands("ARPL",op1,op2);
		break;

	case I486_OPCODE_BOUND: // 0x62
		op1.DecodeMODR_MForRegister(operandSize,operand[0]);
		disasm=DisassembleTypicalTwoOperands("BOUND",op1,op2);
		break;


	case I486_OPCODE_BT_BTS_BTR_BTC_RM_I8:// 0FBA
		switch(GetREG())
		{
		case 4:
			disasm="BT";
			break;
		case 5:
			disasm="BTS";
			break;
		case 6:
			disasm="BTR";
			break;
		case 7:
			disasm="BTC";
			break;
		default:
			disasm="?";
			break;
		}
		disasm=DisassembleTypicalRM_I8(disasm,op1,EvalUimm8());
		break;


	case I486_OPCODE_BSF_R_RM://   0x0FBC,
		disasm=DisassembleTypicalTwoOperands("BSF",op1,op2);
		break;
	case I486_OPCODE_BSR_R_RM://   0x0FBD,
		disasm=DisassembleTypicalTwoOperands("BSR",op1,op2);
		break;
	case I486_OPCODE_BT_R_RM://    0x0FA3,
		disasm=DisassembleTypicalTwoOperands("BT",op1,op2);
		break;
	case I486_OPCODE_BTC_RM_R://   0x0FBB,
		disasm=DisassembleTypicalTwoOperands("BTC",op1,op2);
		break;
	case I486_OPCODE_BTS_RM_R://   0x0FAB,
		disasm=DisassembleTypicalTwoOperands("BTS",op1,op2);
		break;
	case I486_OPCODE_BTR_RM_R://   0x0FB3,
		disasm=DisassembleTypicalTwoOperands("BTR",op1,op2);
		break;

	case I486_OPCODE_LAR:  // 0x0F02
		disasm=DisassembleTypicalTwoOperands("LAR",op1,op2);
		break;

	case I486_OPCODE_CALL_REL://   0xE8,
	case I486_OPCODE_JMP_REL://          0xE9,   // cw or cd
		disasm=(I486_OPCODE_JMP_REL==opCode ? "JMP" : "CALL");
		cpputil::ExtendString(disasm,8);
		{
			auto offset=EvalSimm16or32(operandSize);
			auto destin=eip+offset+numBytes;
			disasm+=cpputil::Uitox(destin);

			auto *sym=symTable.Find(cs.value,destin);
			if(nullptr!=sym)
			{
				disasm+=" {"+sym->Format(true,true,true)+"}";
			}
		}
		break;
	case I486_OPCODE_CALL_FAR://   0x9A,
	case I486_OPCODE_JMP_FAR:
		disasm=(I486_OPCODE_JMP_FAR==opCode ? "JMPF" : "CALLF");
		cpputil::ExtendString(disasm,8);
		disasm+=op1.Disassemble();
		{
			auto *sym=symTable.Find(op1.seg,op1.offset);
			if(nullptr!=sym)
			{
				disasm+=" {"+sym->Format(true,true,true)+"}";
			}
		}
		break;


	case I486_OPCODE_F6_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV: //=0xF6
	case I486_OPCODE_F7_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV: //=0xF7,
		switch(GetREG())
		{
		case 0:
			disasm=DisassembleTypicalOneOperandAndImm("TEST",op1,EvalUimm8or16or32(operandSize),operandSize);
			break;
		case 2:
			disasm=DisassembleTypicalOneOperand("NOT",op1,operandSize);
			break;
		case 3:
			disasm=DisassembleTypicalOneOperand("NEG",op1,operandSize);
			break;
		case 4:
			disasm=DisassembleTypicalOneOperand("MUL",op1,operandSize);
			break;
		case 5:
			disasm=DisassembleTypicalOneOperand("IMUL",op1,operandSize);
			break;
		case 6:
			disasm=DisassembleTypicalOneOperand("DIV",op1,operandSize);
			break;
		case 7:
			disasm=DisassembleTypicalOneOperand("IDIV",op1,operandSize);
			break;
		default:
			disasm=DisassembleTypicalOneOperand(cpputil::Ubtox(opCode)+"?",op1,operandSize);
			break;
		}
		break;


	case I486_OPCODE_CBW_CWDE://        0x98,
		disasm=(16==operandSize ? "CBW" : "CWDE");
		break;
	case I486_OPCODE_CWD_CDQ://         0x99,
		disasm=(16==operandSize ? "CWD" : "CDQ");
		break;
	case I486_OPCODE_CLC:
		disasm="CLC";
		break;
	case I486_OPCODE_CLD:
		disasm="CLD";
		break;
	case I486_OPCODE_CLI:
		disasm="CLI";
		break;
	case I486_OPCODE_CLTS:
		disasm="CLTS";
		break;
	case I486_OPCODE_CMC://        0xF5,
		disasm="CMC";
		break;


	case I486_OPCODE_CMPSB://           0xA6,
		disasm="CMPSB   ";
		if(instPrefix==INST_PREFIX_REPE)
		{
			disasm="REPE "+disasm;
		}
		else if(instPrefix==INST_PREFIX_REPNE)
		{
			disasm="REPNE "+disasm;
		}
		disasm+=SegmentOverrideSIorESIString(segOverride,addressSize);
		break;
	case I486_OPCODE_CMPS://            0xA7,
		disasm=(16==operandSize ? "CMPSW   " : "CMPSD   ");
		if(instPrefix==INST_PREFIX_REPE)
		{
			disasm="REPE "+disasm;
		}
		else if(instPrefix==INST_PREFIX_REPNE)
		{
			disasm="REPNE "+disasm;
		}
		disasm+=SegmentOverrideSIorESIString(segOverride,addressSize);
		break;


	case I486_OPCODE_DAA://             0x27,
		disasm="DAA";
		break;
	case I486_OPCODE_DAS://             0x2F,
		disasm="DAS";
		break;


	case I486_OPCODE_ENTER://      0xC8,
		disasm="ENTER   ";
		{
			// Weird operand.
			unsigned int frameSize=operand[0]|(((unsigned int)operand[1])<<8);
			unsigned int level=operand[2];
			disasm+=cpputil::Ustox(frameSize)+"H,";
			disasm+=cpputil::Ubtox(level)+"H";
		}
		break;


	case I486_OPCODE_FWAIT://      0x9B,
		disasm="FWAIT";
		break;

	case I486_OPCODE_FPU_D8_FADD: // 0xD8,
		{
			auto MODR_M=operand[0];
			if(0xC0<=MODR_M && MODR_M<=0xC7)
			{
				disasm="FADD  ST,ST(";
				disasm.push_back('0'+(operand[0]&7));
				disasm+=")";
			}
			else if(0xD1==MODR_M || // FCOM
			        0xD9==MODR_M)   // FCOMP
			{
				disasm="?FPUINST"+cpputil::Ubtox(opCode)+" "+cpputil::Ubtox(operand[0]);
			}
			else
			{
				switch(GetREG())
				{
				case 3:
					disasm=DisassembleTypicalOneOperand("FCOMP(m32real)  ",op1,operandSize);
					break;
				default:
					disasm="?FPUINST"+cpputil::Ubtox(opCode)+" "+cpputil::Ubtox(operand[0])+" REG="+cpputil::Ubtox(GetREG());
					break;
				}
			}
		}
		break;

	case I486_OPCODE_FPU_D9_FNSTCW_M16_FNSTENV_F2XM1_FXAM_FXCH_FXTRACT_FYL2X_FYL2XP1_FABS_:// 0xD9,
		if(0xF0<=operand[0] && operand[0]<=0xFF)
		{
			disasm="?FPUINST"+cpputil::Ubtox(opCode)+" "+cpputil::Ubtox(operand[0]);
		}
		else if(0xC0<=operand[0] && operand[0]<=0xC7)
		{
			disasm="FLD ";
			disasm+="ST(";
			disasm.push_back('0'+operand[0]-0xC0);
			disasm+=")";
		}
		else if(0xE0==operand[0])
		{
			disasm="FCHS";
		}
		else if(0xE5==operand[0])
		{
			disasm="FXAM";
		}
		else if(0xE8==operand[0])
		{
			disasm="FLD1";
		}
		else if(0xE9==operand[0])
		{
			disasm="FLDL2T";
		}
		else if(0xEE==operand[0])
		{
			disasm="FLDZ";
		}
		else
		{
			switch(GetREG())
			{
			case 0:
				disasm=DisassembleTypicalOneOperand("FLD(m32real)",op1,operandSize);
				break;
			case 5:
				disasm=DisassembleTypicalOneOperand("FLDCW",op1,operandSize);
				break;
			case 7:
				disasm=DisassembleTypicalOneOperand("FNSTCW",op1,operandSize);
				break;
			default:
				disasm="?FPUINST"+cpputil::Ubtox(opCode)+" "+cpputil::Ubtox(operand[0])+" REG="+cpputil::Ubtox(GetREG());
				break;
			}
		}
		if(FPU_FWAIT==fwait)
		{
			disasm="FWAIT "+disasm;
		}
		break;
	case I486_OPCODE_FPU_DB_FNINIT_FRSTOR://     0xDB, 
		if(0xE3==operand[0])
		{
			disasm="FNINIT";
		}
		else
		{
			unsigned int MODR_M=operand[0];
			{
				switch(Instruction::GetREG(MODR_M))
				{
				case 3:
					disasm=DisassembleTypicalOneOperand("FISTP(m32int)",op1,operandSize);
					break;
				case 5:
					disasm=DisassembleTypicalOneOperand("FLD(m80real)",op1,operandSize);
					break;
				case 7:
					disasm=DisassembleTypicalOneOperand("FSTP(m80real)",op1,operandSize);
					break;
				default:
					disasm="?FPUINST"+cpputil::Ubtox(opCode)+" "+cpputil::Ubtox(operand[0])+" REG="+cpputil::Ubtox(Instruction::GetREG(MODR_M));
					break;
				}
			}
		}
		if(FPU_FWAIT==fwait)
		{
			disasm="FWAIT "+disasm;
		}
		break;
	case I486_OPCODE_FPU_DC_FADD:
		{
			unsigned int MODR_M=operand[0];
			{
				switch(Instruction::GetREG(MODR_M))
				{
				case 0:	// FADD m64real
					disasm=DisassembleTypicalOneOperand("FADD(m64real)  ",op1,operandSize);
					break;
				case 1:
					disasm=DisassembleTypicalOneOperand("FMUL(m64real)  ",op1,operandSize);
					break;
				case 2: //
					disasm="?FPUINST REG=2";
					break;
				case 3: //
					disasm=DisassembleTypicalOneOperand("FCOMP(m64real)  ",op1,operandSize);
					break;
				case 4:
					disasm="?FPUINST REG=4";
					break;
				case 5:
					disasm=DisassembleTypicalOneOperand("FSUBR(m64real)  ",op1,operandSize);
					break;
				case 6: //
					disasm="?FPUINST REG=6";
					break;
				case 7: //
					disasm=DisassembleTypicalOneOperand("FDIVR(m64real)  ",op1,operandSize);
					break;
				default:
					disasm="?FPUINST";
					break;
				}
			}
		}
		break;
	case I486_OPCODE_FPU_DD_FLD_FSAVE_FST_FNSTSW_M16_FFREE_FUCOM:
		{
			unsigned int MODR_M=operand[0];
			if(0xD0==(MODR_M&0xF8)) // D0 11010xxx    [1] pp.151  0<=i<=7
			{
				disasm="?FPUINST";
			}
			else if(0xD8==(MODR_M&0xF8)) // D8 11011xxx
			{
				disasm="FSTP ST(";
				disasm+=cpputil::Ubtox(MODR_M&7);
				disasm+=")";
			}
			else if(0xC0==(MODR_M&0xF8)) // C0 11000xxx
			{
				disasm="?FPUINST";
			}
			else if(0xE0==(MODR_M&0xF8) || 0xE1==(MODR_M&0xF8) || 0xE8==(MODR_M&0xF8) || 0xE9==(MODR_M&0xF8))
			{
				disasm="?FPUINST";
			}
			else
			{
				switch(Instruction::GetREG(MODR_M))
				{
				case 0:	// FLD m64real
					disasm=DisassembleTypicalOneOperand("FLD(m64real)",op1,operandSize);
					break;
				case 1:
					disasm="?FPUINST REG=1";
					break;
				case 2: // FST m64real
					disasm="?FPUINST REG=2";
					break;
				case 3: // FSTP m64real
					disasm=DisassembleTypicalOneOperand("FSTP(m64real)",op1,operandSize);
					break;
				case 4:
					disasm="?FPUINST REG=4";
					break;
				case 5:
					disasm="?FPUINST REG=5";
					break;
				case 6: // FSAVE m94/108byte
					disasm="?FPUINST REG=6";
					break;
				case 7: // FNSTSW m2byte
					disasm=DisassembleTypicalOneOperand("FNSTSW",op1,operandSize);
					break;
				default:
					disasm="?FPUINST";
					break;
				}
			}
		}
		break;
	case I486_OPCODE_FPU_DE:
		if(0xC9==operand[0])
		{
			disasm="FMUL";
		}
		else if(0xD9==operand[0])
		{
			disasm="FCOMPP";
		}
		else if(0xF8<=operand[0] && operand[0]<=0xFF)
		{
			disasm="FDIV    ST(";
			disasm.push_back('0'+(operand[0]&7));
			disasm+="),ST";
		}
		else if(0xC0<=operand[0] && operand[0]<=0xC7)
		{
			disasm="FADDP   ST(";
			disasm.push_back('0'+(operand[0]&7));
			disasm+="),ST";
		}
		else if(0xF0<=operand[0] && operand[0]<=0xF7)
		{
			disasm="FDIVRP  ST(";
			disasm.push_back('0'+(operand[0]&7));
			disasm+="),ST";
		}
		else
		{
			disasm="?FPUINST";
		}
		break;
	case I486_OPCODE_FPU_DF_FNSTSW_AX://  0xDF,
		if(0xE0==operand[0])
		{
			disasm="FNSTSW  AX";
		}
		else
		{
			switch(Instruction::GetREG(operand[0]))
			{
			case 4:
				disasm=DisassembleTypicalOneOperand("FBLD(m80dec)  ",op1,operandSize);
				break;
			case 6: // FBSTP m80dec
				disasm=DisassembleTypicalOneOperand("FBSTP(m80dec)  ",op1,operandSize);
				break;
			case 7: // FISTP m64int
				disasm=DisassembleTypicalOneOperand("FISTP(m64int)  ",op1,operandSize);
				break;
			default:
				disasm="?FPUINST REG="+cpputil::Ubtox(Instruction::GetREG(operand[0]));;
				break;
			}
		}
		if(FPU_FWAIT==fwait)
		{
			disasm="FWAIT "+disasm;
		}
		break;


	case I486_OPCODE_ADC_AL_FROM_I8:
		disasm="ADC     AL,"+cpputil::Ubtox(EvalUimm8())+"H";
		break;
	case I486_OPCODE_ADC_A_FROM_I:
		if(16==operandSize)
		{
			disasm="ADC     AX,"+cpputil::Ustox(EvalUimm16())+"H";
		}
		else
		{
			disasm="ADC     EAX,"+cpputil::Uitox(EvalUimm32())+"H";
		}
		break;
	case I486_OPCODE_ADC_RM8_FROM_R8:
		op2.DecodeMODR_MForRegister(8,operand[0]);
		disasm=DisassembleTypicalTwoOperands("ADC",op1,op2);
		break;
	case I486_OPCODE_ADC_R8_FROM_RM8:
		op1.DecodeMODR_MForRegister(8,operand[0]);
		disasm=DisassembleTypicalTwoOperands("ADC",op1,op2);
		break;
	case I486_OPCODE_ADC_RM_FROM_R:
	case I486_OPCODE_ADC_R_FROM_RM:
		disasm=DisassembleTypicalTwoOperands("ADC",op1,op2);
		break;


	case I486_OPCODE_ADD_AL_FROM_I8:
		disasm="ADD     AL,"+cpputil::Ubtox(EvalUimm8())+"H";
		break;
	case I486_OPCODE_ADD_A_FROM_I:
		if(16==operandSize)
		{
			disasm="ADD     AX,"+cpputil::Ustox(EvalUimm16())+"H";
		}
		else
		{
			disasm="ADD     EAX,"+cpputil::Uitox(EvalUimm32())+"H";
		}
		break;
	case I486_OPCODE_ADD_RM8_FROM_R8:
		op2.DecodeMODR_MForRegister(8,operand[0]);
		disasm=DisassembleTypicalTwoOperands("ADD",op1,op2);
		break;
	case I486_OPCODE_ADD_R8_FROM_RM8:
		op1.DecodeMODR_MForRegister(8,operand[0]);
		disasm=DisassembleTypicalTwoOperands("ADD",op1,op2);
		break;
	case I486_OPCODE_ADD_RM_FROM_R:
	case I486_OPCODE_ADD_R_FROM_RM:
		disasm=DisassembleTypicalTwoOperands("ADD",op1,op2);
		break;


	case I486_OPCODE_AND_AL_FROM_I8:
		disasm="AND     AL,"+cpputil::Ubtox(EvalUimm8())+"H";
		break;
	case I486_OPCODE_AND_A_FROM_I:
		if(16==operandSize)
		{
			disasm="AND     AX,"+cpputil::Ustox(EvalUimm16())+"H";
		}
		else
		{
			disasm="AND     EAX,"+cpputil::Uitox(EvalUimm32())+"H";
		}
		break;
	case I486_OPCODE_AND_RM8_FROM_R8:
		op2.DecodeMODR_MForRegister(8,operand[0]);
		disasm=DisassembleTypicalTwoOperands("AND",op1,op2);
		break;
	case I486_OPCODE_AND_R8_FROM_RM8:
		op1.DecodeMODR_MForRegister(8,operand[0]);
		disasm=DisassembleTypicalTwoOperands("AND",op1,op2);
		break;
	case I486_OPCODE_AND_RM_FROM_R:
	case I486_OPCODE_AND_R_FROM_RM:
		disasm=DisassembleTypicalTwoOperands("AND",op1,op2);
		break;


	case I486_OPCODE_CMP_AL_FROM_I8:
		disasm="CMP     AL,"+cpputil::Ubtox(EvalUimm8())+"H";
		break;
	case I486_OPCODE_CMP_A_FROM_I:
		if(16==operandSize)
		{
			disasm="CMP     AX,"+cpputil::Ustox(EvalUimm16())+"H";
		}
		else
		{
			disasm="CMP     EAX,"+cpputil::Uitox(EvalUimm32())+"H";
		}
		break;
	case I486_OPCODE_CMP_RM8_FROM_R8:
		op2.DecodeMODR_MForRegister(8,operand[0]);
		disasm=DisassembleTypicalTwoOperands("CMP",op1,op2);
		break;
	case I486_OPCODE_CMP_R8_FROM_RM8:
		op1.DecodeMODR_MForRegister(8,operand[0]);
		disasm=DisassembleTypicalTwoOperands("CMP",op1,op2);
		break;
	case I486_OPCODE_CMP_RM_FROM_R:
	case I486_OPCODE_CMP_R_FROM_RM:
		disasm=DisassembleTypicalTwoOperands("CMP",op1,op2);
		break;


	case I486_OPCODE_SBB_AL_FROM_I8:
		disasm="SBB     AL,"+cpputil::Ubtox(EvalUimm8())+"H";
		break;
	case I486_OPCODE_SBB_A_FROM_I:
		if(16==operandSize)
		{
			disasm="SBB     AX,"+cpputil::Ustox(EvalUimm16())+"H";
		}
		else
		{
			disasm="SBB     EAX,"+cpputil::Uitox(EvalUimm32())+"H";
		}
		break;
	case I486_OPCODE_SBB_RM8_FROM_R8:
		op2.DecodeMODR_MForRegister(8,operand[0]);
		disasm=DisassembleTypicalTwoOperands("SBB",op1,op2);
		break;
	case I486_OPCODE_SBB_R8_FROM_RM8:
		op1.DecodeMODR_MForRegister(8,operand[0]);
		disasm=DisassembleTypicalTwoOperands("SBB",op1,op2);
		break;
	case I486_OPCODE_SBB_RM_FROM_R:
	case I486_OPCODE_SBB_R_FROM_RM:
		disasm=DisassembleTypicalTwoOperands("SBB",op1,op2);
		break;


	case I486_OPCODE_SUB_AL_FROM_I8:
		disasm="SUB     AL,"+cpputil::Ubtox(EvalUimm8())+"H";
		break;
	case I486_OPCODE_SUB_A_FROM_I:
		if(16==operandSize)
		{
			disasm="SUB     AX,"+cpputil::Ustox(EvalUimm16())+"H";
		}
		else
		{
			disasm="SUB     EAX,"+cpputil::Uitox(EvalUimm32())+"H";
		}
		break;
	case I486_OPCODE_SUB_RM8_FROM_R8:
		op2.DecodeMODR_MForRegister(8,operand[0]);
		disasm=DisassembleTypicalTwoOperands("SUB",op1,op2);
		break;
	case I486_OPCODE_SUB_R8_FROM_RM8:
		op1.DecodeMODR_MForRegister(8,operand[0]);
		disasm=DisassembleTypicalTwoOperands("SUB",op1,op2);
		break;
	case I486_OPCODE_SUB_RM_FROM_R:
	case I486_OPCODE_SUB_R_FROM_RM:
		disasm=DisassembleTypicalTwoOperands("SUB",op1,op2);
		break;


	case I486_OPCODE_TEST_AL_FROM_I8:
		disasm="TEST    AL,"+cpputil::Ubtox(EvalUimm8())+"H";
		break;
	case I486_OPCODE_TEST_A_FROM_I:
		if(16==operandSize)
		{
			disasm="TEST    AX,"+cpputil::Ustox(EvalUimm16())+"H";
		}
		else
		{
			disasm="TEST    EAX,"+cpputil::Uitox(EvalUimm32())+"H";
		}
		break;
	case I486_OPCODE_TEST_RM8_FROM_R8:
		op2.DecodeMODR_MForRegister(8,operand[0]);
		disasm=DisassembleTypicalTwoOperands("TEST",op1,op2);
		break;
	case I486_OPCODE_TEST_RM_FROM_R:
		disasm=DisassembleTypicalTwoOperands("TEST",op1,op2);
		break;


	case I486_OPCODE_DEC_EAX:
	case I486_OPCODE_DEC_ECX:
	case I486_OPCODE_DEC_EDX:
	case I486_OPCODE_DEC_EBX:
	case I486_OPCODE_DEC_ESP:
	case I486_OPCODE_DEC_EBP:
	case I486_OPCODE_DEC_ESI:
	case I486_OPCODE_DEC_EDI:
		disasm="DEC";
		cpputil::ExtendString(disasm,8);
		if(16==operandSize)
		{
			disasm+=Reg16Str[opCode&7];
		}
		else
		{
			disasm+=Reg32Str[opCode&7];
		}
		break;


	case I486_OPCODE_INSB://     0x6C,
		disasm="INSB";
		if(INST_PREFIX_REP==instPrefix)
		{
			disasm="REP "+disasm;
		}
		else if(INST_PREFIX_REPNE==instPrefix)
		{
			disasm="REPNE(!) "+disasm;
		}
		break;


	case I486_OPCODE_IN_AL_I8://=        0xE4,
		disasm="IN";
		cpputil::ExtendString(disasm,8);
		disasm+="AL,";
		disasm+=cpputil::Ubtox(EvalUimm8())+"H";
		{
			auto found=ioTable.find(EvalUimm8());
			if(ioTable.end()!=found)
			{
				disasm+=" (";
				disasm+=found->second;
				disasm+=")";
			}
		}
		break;
	case I486_OPCODE_IN_A_I8://=         0xE5,
		disasm="IN";
		cpputil::ExtendString(disasm,8);
		if(16==operandSize)
		{
			disasm+="AX,";
		}
		else
		{
			disasm+="EAX,";
		}
		disasm+=cpputil::Ubtox(EvalUimm8())+"H";
		{
			auto found=ioTable.find(EvalUimm8());
			if(ioTable.end()!=found)
			{
				disasm+=" (";
				disasm+=found->second;
				disasm+=")";
			}
		}
		break;
	case I486_OPCODE_IN_AL_DX://=        0xEC,
		disasm="IN      AL,DX";
		break;
	case I486_OPCODE_IN_A_DX://=         0xED,
		if(16==operandSize)
		{
			disasm="IN      AX,DX";
		}
		else
		{
			disasm="IN      EAX,DX";
		}
		break;


	case I486_OPCODE_IMUL_R_RM_I8://0x6B,
		disasm="IMUL    ";
		disasm+=op1.Disassemble()+",";
		disasm+=op2.Disassemble()+",";
		disasm+=cpputil::Itox(EvalSimm8());
		break;
	case I486_OPCODE_IMUL_R_RM_IMM://0x69,
		disasm="IMUL    ";
		disasm+=op1.Disassemble()+",";
		disasm+=op2.Disassemble()+",";
		disasm+=cpputil::Itox(EvalSimm16or32(operandSize));
		break;
	case I486_OPCODE_IMUL_R_RM://       0x0FAF,
		disasm=DisassembleTypicalTwoOperands("IMUL",op1,op2);
		break;


	case I486_OPCODE_LEAVE://            0xC9,
		disasm="LEAVE";
		break;


	case I486_OPCODE_HLT://        0xF4,
		disasm="HLT";
		break;


	case I486_OPCODE_INC_DEC_R_M8:
		switch(GetREG())
		{
		case 0:
			disasm=DisassembleTypicalOneOperand("INC",op1,8);
			break;
		case 1:
			disasm=DisassembleTypicalOneOperand("DEC",op1,8);
			break;
		}
		break;
	case I486_OPCODE_INC_DEC_CALL_CALLF_JMP_JMPF_PUSH:
		switch(GetREG())
		{
		case 0:
			disasm=DisassembleTypicalOneOperand("INC",op1,operandSize);
			break;
		case 1:
			disasm=DisassembleTypicalOneOperand("DEC",op1,operandSize);
			break;
		case 2:
			disasm=DisassembleTypicalOneOperand("CALL",op1,operandSize);
			break;
		case 3:
			disasm=DisassembleTypicalOneOperand("CALLF",op1,operandSize);
			break;
		case 4:
			disasm=DisassembleTypicalOneOperand("JMP",op1,operandSize);
			break;
		case 5:
			disasm=DisassembleTypicalOneOperand("JMPF",op1,operandSize);
			break;
		case 6:
			disasm=DisassembleTypicalOneOperand("PUSH",op1,operandSize);
			break;
		}
		break;
	case I486_OPCODE_INC_EAX://    0x40, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_ECX://    0x41, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_EDX://    0x42, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_EBX://    0x43, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_ESP://    0x44, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_EBP://    0x45, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_ESI://    0x46, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_INC_EDI://    0x47, // 16/32 depends on OPSIZE_OVERRIDE
		disasm="INC";
		cpputil::ExtendString(disasm,8);
		if(16==operandSize)
		{
			disasm+=Reg16Str[opCode&7];
		}
		else
		{
			disasm+=Reg32Str[opCode&7];
		}
		break;


	case I486_OPCODE_INT3://       0xCC,
		disasm="INT3";
		break;
	case I486_OPCODE_INT://        0xCD,
		disasm = "INT";
		disasm=DisassembleTypicalOneImm(disasm,EvalUimm8(),8);
		if(I486_OPCODE_INT==opCode)
		{
			auto label=symTable.GetINTLabel(EvalUimm8());
			if(""!=label)
			{
				disasm.push_back(' ');
				disasm.push_back('(');
				disasm+=label;
				disasm.push_back(')');
			}
		}
		break;
	case I486_OPCODE_INTO://        0xCE,
		disasm = "INTO";
		break;


	case I486_OPCODE_IRET://   0xCF,
		disasm=(16==operandSize ? "IRET" : "IRETD");
		break;


	case I486_OPCODE_JMP_REL8://         0xEB,   // cb
	case I486_OPCODE_JO_REL8:   // 0x70,
	case I486_OPCODE_JNO_REL8:  // 0x71,
	case I486_OPCODE_JB_REL8:   // 0x72,
	case I486_OPCODE_JAE_REL8:  // 0x73,
	case I486_OPCODE_JE_REL8:   // 0x74,
	case I486_OPCODE_JECXZ_REL8:// 0xE3,  // Depending on the operand size
	case I486_OPCODE_JNE_REL8:  // 0x75,
	case I486_OPCODE_JBE_REL8:  // 0x76,
	case I486_OPCODE_JA_REL8:   // 0x77,
	case I486_OPCODE_JS_REL8:   // 0x78,
	case I486_OPCODE_JNS_REL8:  // 0x79,
	case I486_OPCODE_JP_REL8:   // 0x7A,
	case I486_OPCODE_JNP_REL8:  // 0x7B,
	case I486_OPCODE_JL_REL8:   // 0x7C,
	case I486_OPCODE_JGE_REL8:  // 0x7D,
	case I486_OPCODE_JLE_REL8:  // 0x7E,
	case I486_OPCODE_JG_REL8:   // 0x7F,
	case I486_OPCODE_LOOP://             0xE2,
	case I486_OPCODE_LOOPE://            0xE1,
	case I486_OPCODE_LOOPNE://           0xE0,
		switch(opCode)
		{
		case I486_OPCODE_JMP_REL8://         0xEB,   // cb
			disasm="JMP";
			break;
		case I486_OPCODE_JO_REL8:   // 0x70,
			disasm="JO";
			break;
		case I486_OPCODE_JNO_REL8:  // 0x71,
			disasm="JNO";
			break;
		case I486_OPCODE_JB_REL8:   // 0x72,
			disasm="JB";
			break;
		case I486_OPCODE_JAE_REL8:  // 0x73,
			disasm="JAE";
			break;
		case I486_OPCODE_JE_REL8:   // 0x74,
			disasm="JE";
			break;
		case I486_OPCODE_JECXZ_REL8:// 0xE3,  // Depending on the operand size
			disasm=(16==operandSize ? "JCXZ" : "JECXZ");
			break;
		case I486_OPCODE_JNE_REL8:  // 0x75,
			disasm="JNE";
			break;
		case I486_OPCODE_JBE_REL8:  // 0x76,
			disasm="JBE";
			break;
		case I486_OPCODE_JA_REL8:   // 0x77,
			disasm="JA";
			break;
		case I486_OPCODE_JS_REL8:   // 0x78,
			disasm="JS";
			break;
		case I486_OPCODE_JNS_REL8:  // 0x79,
			disasm="JNS";
			break;
		case I486_OPCODE_JP_REL8:   // 0x7A,
			disasm="JP";
			break;
		case I486_OPCODE_JNP_REL8:  // 0x7B,
			disasm="JNP";
			break;
		case I486_OPCODE_JL_REL8:   // 0x7C,
			disasm="JL";
			break;
		case I486_OPCODE_JGE_REL8:  // 0x7D,
			disasm="JGE";
			break;
		case I486_OPCODE_JLE_REL8:  // 0x7E,
			disasm="JLE";
			break;
		case I486_OPCODE_JG_REL8:   // 0x7F,
			disasm="JG";
			break;
		case I486_OPCODE_LOOP://             0xE2,
			disasm="LOOP";
			break;
		case I486_OPCODE_LOOPE://            0xE1,
			disasm="LOOPE";
			break;
		case I486_OPCODE_LOOPNE://           0xE0,
			disasm="LOOPNE";
			break;
		}
		cpputil::ExtendString(disasm,8);
		{
			auto offset=EvalSimm8();
			auto destin=eip+offset+numBytes;
			disasm+=cpputil::Uitox(destin);

			auto *sym=symTable.Find(cs.value,destin);
			if(nullptr!=sym)
			{
				disasm+=" {"+sym->Format(true,true,true)+"}";
			}
		}
		break;


	case I486_OPCODE_JA_REL://    0x0F87,
	case I486_OPCODE_JAE_REL://   0x0F83,
	case I486_OPCODE_JB_REL://    0x0F82,
	case I486_OPCODE_JBE_REL://   0x0F86,
	// case I486_OPCODE_JC_REL://    0x0F82, Same as JB_REL
	case I486_OPCODE_JE_REL://    0x0F84,
	// case I486_OPCODE_JZ_REL://    0x0F84, Same as JZ_REL
	case I486_OPCODE_JG_REL://    0x0F8F,
	case I486_OPCODE_JGE_REL://   0x0F8D,
	case I486_OPCODE_JL_REL://    0x0F8C,
	case I486_OPCODE_JLE_REL://   0x0F8E,
	// case I486_OPCODE_JNA_REL://   0x0F86, Same as JBE_REL
	// case I486_OPCODE_JNAE_REL://  0x0F82, Same as JB_REL
	// case I486_OPCODE_JNB_REL://   0x0F83, Same as JAE_REL
	// case I486_OPCODE_JNBE_REL://  0x0F87, Same as JA_REL
	// case I486_OPCODE_JNC_REL://   0x0F83, Same as JAE_REL
	case I486_OPCODE_JNE_REL://   0x0F85,
	// case I486_OPCODE_JNG_REL://   0x0F8E, Same as JLE_REL
	// case I486_OPCODE_JNGE_REL://  0x0F8C, Same as JL_REL
	// case I486_OPCODE_JNL_REL://   0x0F8D, Same as JGE_REL
	// case I486_OPCODE_JNLE_REL://  0x0F8F, Same as JG_REL
	case I486_OPCODE_JNO_REL://   0x0F81,
	case I486_OPCODE_JNP_REL://   0x0F8B,
	case I486_OPCODE_JNS_REL://   0x0F89,
	// case I486_OPCODE_JNZ_REL://   0x0F85, Same as JNE_REL
	case I486_OPCODE_JO_REL://    0x0F80,
	case I486_OPCODE_JP_REL://    0x0F8A,
	// case I486_OPCODE_JPE_REL://   0x0F8A, Same as JP_REL
	// case I486_OPCODE_JPO_REL://   0x0F8B, Same as JNP_REL
	case I486_OPCODE_JS_REL://    0x0F88,
		switch(opCode)
		{
		case I486_OPCODE_JO_REL:   // 0x70,
			disasm="JO";
			break;
		case I486_OPCODE_JNO_REL:  // 0x71,
			disasm="JNO";
			break;
		case I486_OPCODE_JB_REL:   // 0x72,
			disasm="JB";
			break;
		case I486_OPCODE_JAE_REL:  // 0x73,
			disasm="JAE";
			break;
		case I486_OPCODE_JE_REL:   // 0x74,
			disasm="JE";
			break;
		case I486_OPCODE_JNE_REL:  // 0x75,
			disasm="JNE";
			break;
		case I486_OPCODE_JBE_REL:  // 0x76,
			disasm="JBE";
			break;
		case I486_OPCODE_JA_REL:   // 0x77,
			disasm="JA";
			break;
		case I486_OPCODE_JS_REL:   // 0x78,
			disasm="JS";
			break;
		case I486_OPCODE_JNS_REL:  // 0x79,
			disasm="JNS";
			break;
		case I486_OPCODE_JP_REL:   // 0x7A,
			disasm="JP";
			break;
		case I486_OPCODE_JNP_REL:  // 0x7B,
			disasm="JNP";
			break;
		case I486_OPCODE_JL_REL:   // 0x7C,
			disasm="JL";
			break;
		case I486_OPCODE_JGE_REL:  // 0x7D,
			disasm="JGE";
			break;
		case I486_OPCODE_JLE_REL:  // 0x7E,
			disasm="JLE";
			break;
		case I486_OPCODE_JG_REL:   // 0x7F,
			disasm="JG";
			break;
		default:
			disasm="J?";
			break;
		}
		cpputil::ExtendString(disasm,8);
		{
			auto offset=EvalSimm16or32(operandSize);
			auto destin=eip+offset+numBytes;
			disasm+=cpputil::Uitox(destin);

			auto *sym=symTable.Find(cs.value,destin);
			if(nullptr!=sym)
			{
				disasm+=" {"+sym->Format(true,true,true)+"}";
			}
		}
		break;


	case I486_OPCODE_BINARYOP_RM8_FROM_I8://=  0x80, // AND(REG=4), OR(REG=1), or XOR(REG=6) depends on the REG field of MODR/M
	case I486_OPCODE_BINARYOP_RM8_FROM_I8_ALIAS:
		switch(GetREG())
		{
		case 0:
			disasm=DisassembleTypicalOneOperandAndImm("ADD",op1,EvalUimm8(),8);
			break;
		case 1:
			disasm=DisassembleTypicalOneOperandAndImm("OR",op1,EvalUimm8(),8);
			break;
		case 2:
			disasm=DisassembleTypicalOneOperandAndImm("ADC",op1,EvalUimm8(),8);
			break;
		case 3:
			disasm=DisassembleTypicalOneOperandAndImm("SBB",op1,EvalUimm8(),8);
			break;
		case 4:
			disasm=DisassembleTypicalOneOperandAndImm("AND",op1,EvalUimm8(),8);
			break;
		case 5:
			disasm=DisassembleTypicalOneOperandAndImm("SUB",op1,EvalUimm8(),8);
			break;
		case 6:
			disasm=DisassembleTypicalOneOperandAndImm("XOR",op1,EvalUimm8(),8);
			break;
		case 7:
			disasm=DisassembleTypicalOneOperandAndImm("CMP",op1,EvalUimm8(),8);
			break;
		default:
			disasm=DisassembleTypicalOneOperandAndImm(cpputil::Ubtox(opCode)+"?",op1,EvalUimm8(),8);
			break;
		}
		break;
	case I486_OPCODE_BINARYOP_R_FROM_I://=     0x81,
		switch(GetREG())
		{
		case 0:
			disasm=DisassembleTypicalOneOperandAndImm("ADD",op1,EvalUimm8or16or32(operandSize),operandSize);
			break;
		case 1:
			disasm=DisassembleTypicalOneOperandAndImm("OR",op1,EvalUimm8or16or32(operandSize),operandSize);
			break;
		case 2:
			disasm=DisassembleTypicalOneOperandAndImm("ADC",op1,EvalUimm8or16or32(operandSize),operandSize);
			break;
		case 3:
			disasm=DisassembleTypicalOneOperandAndImm("SBB",op1,EvalUimm8or16or32(operandSize),operandSize);
			break;
		case 4:
			disasm=DisassembleTypicalOneOperandAndImm("AND",op1,EvalUimm8or16or32(operandSize),operandSize);
			break;
		case 5:
			disasm=DisassembleTypicalOneOperandAndImm("SUB",op1,EvalUimm8or16or32(operandSize),operandSize);
			break;
		case 6:
			disasm=DisassembleTypicalOneOperandAndImm("XOR",op1,EvalUimm8or16or32(operandSize),operandSize);
			break;
		case 7:
			disasm=DisassembleTypicalOneOperandAndImm("CMP",op1,EvalUimm8or16or32(operandSize),operandSize);
			break;
		default:
			disasm=DisassembleTypicalOneOperandAndImm(cpputil::Ubtox(opCode)+"?",op1,EvalUimm8or16or32(operandSize),operandSize);
			break;
		}
		break;
	case I486_OPCODE_BINARYOP_RM_FROM_SXI8://= 0x83,
		switch(GetREG())
		{
		case 0:
			disasm=DisassembleTypicalOneOperand("ADD",op1,operandSize);
			break;
		case 1:
			disasm=DisassembleTypicalOneOperand("OR",op1,operandSize);
			break;
		case 2:
			disasm=DisassembleTypicalOneOperand("ADC",op1,operandSize);
			break;
		case 3:
			disasm=DisassembleTypicalOneOperand("SBB",op1,operandSize);
			break;
		case 4:
			disasm=DisassembleTypicalOneOperand("AND",op1,operandSize);
			break;
		case 5:
			disasm=DisassembleTypicalOneOperand("SUB",op1,operandSize);
			break;
		case 6:
			disasm=DisassembleTypicalOneOperand("XOR",op1,operandSize);
			break;
		case 7:
			disasm=DisassembleTypicalOneOperand("CMP",op1,operandSize);
			break;
		default:
			disasm=DisassembleTypicalOneOperand(cpputil::Ubtox(opCode)+"?",op1,operandSize);
			break;
		}
		{
			short imm=EvalUimm8();
			if(0x80&imm)
			{
				imm-=0x100;
				imm=-imm;
				disasm+=",-"+cpputil::Ubtox((unsigned int)imm)+"H";
			}
			else
			{
				disasm+=","+cpputil::Ubtox((unsigned int)imm)+"H";
			}
		}
		break;


	case I486_OPCODE_LAHF://=             0x9F,
		disasm="LAHF";
		break;


	case I486_OPCODE_LEA://=              0x8D,
		disasm="LEA";
		cpputil::ExtendString(disasm,8);
		disasm+=op1.Disassemble();
		disasm.push_back(',');
		if(addressSize!=operandSize)
		{
			disasm+=Operand::GetSizeQualifierToDisassembly(op2,addressSize);
		}
		disasm+=op2.Disassemble();
		break;


	case I486_OPCODE_LDS://              0xC5,
	case I486_OPCODE_LSS://              0x0FB2,
	case I486_OPCODE_LES://              0xC4,
	case I486_OPCODE_LFS://              0x0FB4,
	case I486_OPCODE_LGS://              0x0FB5,
		switch(opCode)
		{
		case I486_OPCODE_LDS://              0xC5,
			disasm="LDS";
			break;
		case I486_OPCODE_LSS://              0x0FB2,
			disasm="LSS";
			break;
		case I486_OPCODE_LES://              0xC4,
			disasm="LES";
			break;
		case I486_OPCODE_LFS://              0x0FB4,
			disasm="LFS";
			break;
		case I486_OPCODE_LGS://              0x0FB5,
			disasm="LGS";
			break;
		}
		disasm=DisassembleTypicalTwoOperands(disasm,op1,op2);
		break;


	case I486_OPCODE_LODSB://            0xAC,
		disasm="LODSB   ";
		disasm+=SegmentOverrideSIorESIString(segOverride,addressSize);
		if(INST_PREFIX_REP==instPrefix)
		{
			disasm="REP "+disasm;
		}
		else if(INST_PREFIX_REPNE==instPrefix)
		{
			disasm="REPNE(!) "+disasm;
		}
		break;
	case I486_OPCODE_LODS://             0xAD,
		disasm=(16==operandSize ? "LODSW   " : "LODSD   ");
		disasm+=SegmentOverrideSIorESIString(segOverride,addressSize);
		if(INST_PREFIX_REP==instPrefix)
		{
			disasm="REP "+disasm;
		}
		else if(INST_PREFIX_REPNE==instPrefix)
		{
			disasm="REPNE(!) "+disasm;
		}
		break;


	case I486_OPCODE_LGDT_LIDT_SGDT_SIDT:
		switch(GetREG())
		{
		case 0:
			disasm=DisassembleTypicalOneOperand("SGDT",op1,16+operandSize);
			break;
		case 1:
			disasm=DisassembleTypicalOneOperand("SIDT",op1,16+operandSize);
			break;
		case 2:
			disasm=DisassembleTypicalOneOperand("LGDT",op1,16+operandSize);
			break;
		case 3:
			disasm=DisassembleTypicalOneOperand("LIDT",op1,16+operandSize);
			break;
		case 4:
			disasm=DisassembleTypicalOneOperand("SMSW",op1,16+operandSize);
			break;
		case 6:
			disasm=DisassembleTypicalOneOperand("LMSW",op1,16+operandSize);
			break;
		case 7:
			disasm=DisassembleTypicalOneOperand("INVLPG",op1,16+operandSize);
			break;
		default:
			disasm=DisassembleTypicalTwoOperands(cpputil::Ubtox(opCode)+"?",op1,op2)+" REG="+cpputil::Ubtox(GetREG());
			break;
		}
		break;


	case I486_OPCODE_LSL://              0x0F03,
		disasm=DisassembleTypicalTwoOperands("LSL",op1,op2);
		break;


	case I486_OPCODE_MOV_FROM_R8: //      0x88,
		op2.DecodeMODR_MForRegister(8,operand[0]);
		disasm=DisassembleTypicalTwoOperands("MOV",op1,op2);
		break;
	case I486_OPCODE_MOV_FROM_R: //       0x89, // 16/32 depends on OPSIZE_OVERRIDE
		op2.DecodeMODR_MForRegister(operandSize,operand[0]);
		disasm=DisassembleTypicalTwoOperands("MOV",op1,op2);
		break;
	case I486_OPCODE_MOV_TO_R8: //        0x8A,
		op1.DecodeMODR_MForRegister(8,operand[0]);
		disasm=DisassembleTypicalTwoOperands("MOV",op1,op2);
		break;
	case I486_OPCODE_MOV_TO_R: //         0x8B, // 16/32 depends on OPSIZE_OVERRIDE
		op1.DecodeMODR_MForRegister(operandSize,operand[0]);
		disasm=DisassembleTypicalTwoOperands("MOV",op1,op2);
		break;

	case I486_OPCODE_MOV_M_TO_AL: //      0xA0,
		op2.MakeSimpleAddressOffsetFromImm(*this);
		op1.MakeByRegisterNumber(8,REG_AL-REG_8BIT_REG_BASE);
		disasm=DisassembleTypicalTwoOperands("MOV",op1,op2);
		break;
	case I486_OPCODE_MOV_M_TO_EAX: //     0xA1, // 16/32 depends on OPSIZE_OVERRIDE
		op2.MakeSimpleAddressOffsetFromImm(*this);
		op1.MakeByRegisterNumber(operandSize,REG_AL-REG_8BIT_REG_BASE);
		disasm=DisassembleTypicalTwoOperands("MOV",op1,op2);
		break;
	case I486_OPCODE_MOV_M_FROM_AL: //    0xA2,
		op1.MakeSimpleAddressOffsetFromImm(*this);
		op2.MakeByRegisterNumber(8,REG_AL-REG_8BIT_REG_BASE);
		disasm=DisassembleTypicalTwoOperands("MOV",op1,op2);
		break;
	case I486_OPCODE_MOV_M_FROM_EAX: //   0xA3, // 16/32 depends on OPSIZE_OVERRIDE
		op1.MakeSimpleAddressOffsetFromImm(*this);
		op2.MakeByRegisterNumber(operandSize,REG_AL-REG_8BIT_REG_BASE);
		disasm=DisassembleTypicalTwoOperands("MOV",op1,op2);
		break;

	case I486_OPCODE_MOV_FROM_SEG: //     0x8C,
	case I486_OPCODE_MOV_TO_SEG: //       0x8E,

	case I486_OPCODE_MOV_TO_CR://        0x0F22,
	case I486_OPCODE_MOV_FROM_CR://      0x0F20,
	case I486_OPCODE_MOV_FROM_DR://      0x0F21,
	case I486_OPCODE_MOV_TO_DR://        0x0F23,
	case I486_OPCODE_MOV_FROM_TR://      0x0F24,
	case I486_OPCODE_MOV_TO_TR://        0x0F26,
		disasm=DisassembleTypicalTwoOperands("MOV",op1,op2);
		break;

	case I486_OPCODE_MOV_I8_TO_AL: //     0xB0,
	case I486_OPCODE_MOV_I8_TO_CL: //     0xB1,
	case I486_OPCODE_MOV_I8_TO_DL: //     0xB2,
	case I486_OPCODE_MOV_I8_TO_BL: //     0xB3,
	case I486_OPCODE_MOV_I8_TO_AH: //     0xB4,
	case I486_OPCODE_MOV_I8_TO_CH: //     0xB5,
	case I486_OPCODE_MOV_I8_TO_DH: //     0xB6,
	case I486_OPCODE_MOV_I8_TO_BH: //     0xB7,
		op1.MakeByRegisterNumber(8,opCode&7);
		disasm=DisassembleTypicalOneOperandAndImm("MOV",op1,EvalUimm8(),8);
		disasm+=DisassembleIOLabel(cs.value,eip,symTable,ioTable,EvalUimm8());
		break;

	case I486_OPCODE_MOV_I8_TO_RM8: //    0xC6,
		op1.Decode(addressSize,8,operand);
		disasm=DisassembleTypicalOneOperandAndImm("MOV",op1,EvalUimm8(),8);
		disasm+=DisassembleIOLabel(cs.value,eip,symTable,ioTable,EvalUimm8());
		break;
	case I486_OPCODE_MOV_I_TO_RM: //      0xC7, // 16/32 depends on OPSIZE_OVERRIDE
		op1.Decode(addressSize,operandSize,operand);
		disasm=DisassembleTypicalOneOperandAndImm("MOV",op1,EvalUimm8or16or32(operandSize),operandSize);
		disasm+=DisassembleIOLabel(cs.value,eip,symTable,ioTable,EvalUimm8or16or32(operandSize));
		break;

	case I486_OPCODE_MOV_I_TO_EAX: //     0xB8, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_ECX: //     0xB9, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_EDX: //     0xBA, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_EBX: //     0xBB, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_ESP: //     0xBC, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_EBP: //     0xBD, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_ESI: //     0xBE, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_OPCODE_MOV_I_TO_EDI: //     0xBF, // 16/32 depends on OPSIZE_OVERRIDE
		op1.MakeByRegisterNumber(operandSize,opCode&7);
		disasm=DisassembleTypicalOneOperandAndImm("MOV",op1,EvalUimm8or16or32(operandSize),operandSize);
		disasm+=DisassembleIOLabel(cs.value,eip,symTable,ioTable,EvalUimm8or16or32(operandSize));
		break;



	case I486_OPCODE_MOVSB://            0xA4,
		disasm="MOVSB   ";
		if(INST_PREFIX_REP==instPrefix)
		{
			disasm="REP "+disasm;
		}
		else if(INST_PREFIX_REPNE==instPrefix)
		{
			disasm="REPNE(!) "+disasm;
		}
		disasm+=SegmentOverrideSIorESIString(segOverride,addressSize);
		break;
	case I486_OPCODE_MOVS://             0xA5,
		disasm=(16==operandSize ? "MOVSW   " : "MOVSD  ");
		if(INST_PREFIX_REP==instPrefix)
		{
			disasm="REP "+disasm;
		}
		else if(INST_PREFIX_REPNE==instPrefix)
		{
			disasm="REPNE(!) "+disasm;
		}
		disasm+=SegmentOverrideSIorESIString(segOverride,addressSize);
		break;


	case I486_OPCODE_MOVSX_R_RM8://=      0x0FBE,
	case I486_OPCODE_MOVZX_R_RM8://=      0x0FB6,
		disasm=(I486_OPCODE_MOVZX_R_RM8==opCode ? "MOVZX" : "MOVSX");
		cpputil::ExtendString(disasm,8);
		disasm+=op1.Disassemble();
		disasm.push_back(',');
		disasm+=Operand::GetSizeQualifierToDisassembly(op2,8);
		disasm+=SegmentOverrideString(segOverride);
		disasm+=op2.Disassemble();
		break;
	case I486_OPCODE_MOVSX_R32_RM16://=   0x0FBF,
	case I486_OPCODE_MOVZX_R32_RM16://=   0x0FB7,
		disasm=(I486_OPCODE_MOVZX_R32_RM16==opCode ? "MOVZX" : "MOVSX");
		cpputil::ExtendString(disasm,8);
		disasm+=op1.Disassemble();
		disasm.push_back(',');
		disasm+=Operand::GetSizeQualifierToDisassembly(op2,16);
		disasm+=SegmentOverrideString(segOverride);
		disasm+=op2.Disassemble();
		break;


	case I486_OPCODE_RET://              0xC3,
		disasm="RET";
		break;
	case I486_OPCODE_RETF://             0xCB,
		disasm="RETF";
		break;
	case I486_OPCODE_RET_I16://          0xC2,
		disasm="RET";
		cpputil::ExtendString(disasm,8);
		disasm+=cpputil::Ustox(EvalUimm16())+"H";
		break;
	case I486_OPCODE_RETF_I16://         0xCA,
		disasm="RETF";
		cpputil::ExtendString(disasm,8);
		disasm+=cpputil::Ustox(EvalUimm16())+"H";
		break;


	case I486_OPCODE_SAHF://=             0x9E,
		disasm="SAHF";
		break;


	case I486_OPCODE_SHLD_RM_I8://       0x0FA4,
	case I486_OPCODE_SHLD_RM_CL://       0x0FA5,
	case I486_OPCODE_SHRD_RM_I8://       0x0FAC,
	case I486_OPCODE_SHRD_RM_CL://       0x0FAD,
		{
			std::string count;
			if(I486_OPCODE_SHLD_RM_CL==opCode || I486_OPCODE_SHRD_RM_CL==opCode)
			{
				count="CL";
			}
			else
			{
				count=cpputil::Ubtox(EvalUimm8())+"H";
			}
			switch(opCode)
			{
			case I486_OPCODE_SHLD_RM_I8://       0x0FA4,
			case I486_OPCODE_SHLD_RM_CL://       0x0FA5,
				disasm="SHLD    ";
				break;
			case I486_OPCODE_SHRD_RM_I8://       0x0FAC,
			case I486_OPCODE_SHRD_RM_CL://       0x0FAD,
				disasm="SHRD    ";
				break;
			}
			disasm+=op1.Disassemble()+",";
			disasm+=op2.Disassemble()+",";
			disasm+=count;
		}
		break;


	case I486_OPCODE_SCASB://            0xAE,
		disasm="SCASB";
		if(instPrefix==INST_PREFIX_REPE)
		{
			disasm="REPE "+disasm;
		}
		else if(instPrefix==INST_PREFIX_REPNE)
		{
			disasm="REPNE "+disasm;
		}
		break;
	case I486_OPCODE_SCAS://             0xAF,
		disasm=(16==operandSize ? "SCASW" : "SCASD");
		if(instPrefix==INST_PREFIX_REPE)
		{
			disasm="REPE "+disasm;
		}
		else if(instPrefix==INST_PREFIX_REPNE)
		{
			disasm="REPNE "+disasm;
		}
		break;


	case I486_OPCODE_SETA://             0x0F97,
		disasm=DisassembleTypicalOneOperand("SETA",op1,8);
		break;
	case I486_OPCODE_SETAE://            0x0F93,
		disasm=DisassembleTypicalOneOperand("SETAE",op1,8);
		break;
	case I486_OPCODE_SETB://             0x0F92,
		disasm=DisassembleTypicalOneOperand("SETB",op1,8);
		break;
	case I486_OPCODE_SETBE://            0x0F96,
		disasm=DisassembleTypicalOneOperand("SETBE",op1,8);
		break;
	// I486_OPCODE_SETC://             0x0F92,
	case I486_OPCODE_SETE://             0x0F94,
		disasm=DisassembleTypicalOneOperand("SETE",op1,8);
		break;
	case I486_OPCODE_SETG://             0x0F9F,
		disasm=DisassembleTypicalOneOperand("SETG",op1,8);
		break;
	case I486_OPCODE_SETGE://            0x0F9D,
		disasm=DisassembleTypicalOneOperand("SETGE",op1,8);
		break;
	case I486_OPCODE_SETL://             0x0F9C,
		disasm=DisassembleTypicalOneOperand("SETL",op1,8);
		break;
	case I486_OPCODE_SETLE://            0x0F9E,
		disasm=DisassembleTypicalOneOperand("SETLE",op1,8);
		break;
	//I486_OPCODE_SETNA://            0x0F96,
	//I486_OPCODE_SETNAE://           0x0F92,
	//I486_OPCODE_SETNB://            0x0F93,
	//I486_OPCODE_SETNBE://           0x0F97,
	//I486_OPCODE_SETNC://            0x0F93,
	case I486_OPCODE_SETNE://            0x0F95,
		disasm=DisassembleTypicalOneOperand("SETNE",op1,8);
		break;
	//I486_OPCODE_SETNG://            0x0F9E,
	//I486_OPCODE_SETNGE://           0x0F9C,
	//I486_OPCODE_SETNL://            0x0F9D,
	//I486_OPCODE_SETNLE://           0x0F9F,
	case I486_OPCODE_SETNO://            0x0F91,
		disasm=DisassembleTypicalOneOperand("SETNO",op1,8);
		break;
	case I486_OPCODE_SETNP://            0x0F9B,
		disasm=DisassembleTypicalOneOperand("SETNP",op1,8);
		break;
	case I486_OPCODE_SETNS://            0x0F99,
		disasm=DisassembleTypicalOneOperand("SETNS",op1,8);
		break;
	// I486_OPCODE_SETNZ://            0x0F95,
	case I486_OPCODE_SETO://             0x0F90,
		disasm=DisassembleTypicalOneOperand("SETO",op1,8);
		break;
	case I486_OPCODE_SETP://             0x0F9A,
		disasm=DisassembleTypicalOneOperand("SETP",op1,8);
		break;
	//I486_OPCODE_SETPE://            0x0F9A,
	//I486_OPCODE_SETPO://            0x0F9B,
	case I486_OPCODE_SETS://             0x0F98,
		disasm=DisassembleTypicalOneOperand("SETS",op1,8);
		break;
	// I486_OPCODE_SETZ://             0x0F94,


	case I486_OPCODE_SLDT_STR_LLDT_LTR_VERR_VERW://             0x0F00,
		switch(GetREG())
		{
		case 0:
			disasm=DisassembleTypicalOneOperand("SLDT",op1,16);
			break;
		case 1:
			disasm=DisassembleTypicalOneOperand("STR",op1,16);
			break;
		case 2:
			disasm=DisassembleTypicalOneOperand("LLDT",op1,16);
			break;
		case 3:
			disasm=DisassembleTypicalOneOperand("LTR",op1,16);
			break;
		case 4:
			disasm=DisassembleTypicalOneOperand("VERR",op1,16);
			break;
		case 5:
			disasm=DisassembleTypicalOneOperand("VERW",op1,16);
			break;
		}
		break;


	case I486_OPCODE_STC://              0xF9,
		disasm="STC";
		break;
	case I486_OPCODE_STD://              0xFD,
		disasm="STD";
		break;
	case I486_OPCODE_STI://              0xFB,
		disasm="STI";
		break;


	case I486_OPCODE_STOSB://            0xAA,
		disasm="STOSB";
		if(INST_PREFIX_REP==instPrefix)
		{
			disasm="REP "+disasm;
		}
		else if(INST_PREFIX_REPNE==instPrefix)
		{
			disasm="REPNE(!) "+disasm;
		}
		break;
	case I486_OPCODE_STOS://             0xAB,
		if(16==operandSize)
		{
			disasm="STOSW";
		}
		else
		{
			disasm="STOSD";
		}
		if(INST_PREFIX_REP==instPrefix)
		{
			disasm="REP "+disasm;
		}
		else if(INST_PREFIX_REPNE==instPrefix)
		{
			disasm="REPNE(!) "+disasm;
		}
		break;


	case I486_OPCODE_NOP://              0x90,
		disasm="NOP";
		break;


	case I486_OPCODE_OUT_I8_AL: //        0xE6,
		disasm="OUT";
		cpputil::ExtendString(disasm,8);
		disasm+=cpputil::Ubtox(EvalUimm8())+"H";
		disasm+=",AL";
		{
			auto found=ioTable.find(EvalUimm8());
			if(ioTable.end()!=found)
			{
				disasm+=" (";
				disasm+=found->second;
				disasm+=")";
			}
		}
		break;
	case I486_OPCODE_OUT_I8_A: //         0xE7,
		disasm="OUT";
		cpputil::ExtendString(disasm,8);
		disasm+=cpputil::Ubtox(EvalUimm8())+"H";
		if(16==operandSize)
		{
			disasm+=",AX";
		}
		else
		{
			disasm+=",EAX";
		}
		{
			auto found=ioTable.find(EvalUimm8());
			if(ioTable.end()!=found)
			{
				disasm+=" (";
				disasm+=found->second;
				disasm+=")";
			}
		}
		break;
	case I486_OPCODE_OUT_DX_AL: //        0xEE,
		disasm="OUT     DX,AL";
		break;
	case I486_OPCODE_OUT_DX_A: //         0xEF,
		if(16==operandSize)
		{
			disasm="OUT     DX,AX";
		}
		else
		{
			disasm="OUT     DX,EAX";
		}
		break;


	case I486_OPCODE_OUTSB://            0x6E,
		disasm="OUTSB   ";
		if(INST_PREFIX_REP==instPrefix)
		{
			disasm="REP "+disasm;
		}
		else if(INST_PREFIX_REPNE==instPrefix)
		{
			disasm="REPNE(!) "+disasm;
		}
		disasm+=SegmentOverrideSIorESIString(segOverride,addressSize);
		break;
	case I486_OPCODE_OUTS://             0x6F,
		disasm=(16==operandSize ? "OUTSW   " : "OUTSD   ");
		if(INST_PREFIX_REP==instPrefix)
		{
			disasm="REP "+disasm;
		}
		else if(INST_PREFIX_REPNE==instPrefix)
		{
			disasm="REPNE(!) "+disasm;
		}
		disasm+=SegmentOverrideSIorESIString(segOverride,addressSize);
		break;


	case I486_OPCODE_PUSHA://            0x60,
		disasm=(16==operandSize ? "PUSHA" : "PUSHAD");
		break;
	case I486_OPCODE_PUSHF://            0x9C,
		disasm=(16==operandSize ? "PUSHF" : "PUSHFD");
		break;


	case I486_OPCODE_PUSH_EAX://         0x50,
	case I486_OPCODE_PUSH_ECX://         0x51,
	case I486_OPCODE_PUSH_EDX://         0x52,
	case I486_OPCODE_PUSH_EBX://         0x53,
	case I486_OPCODE_PUSH_ESP://         0x54,
	case I486_OPCODE_PUSH_EBP://         0x55,
	case I486_OPCODE_PUSH_ESI://         0x56,
	case I486_OPCODE_PUSH_EDI://         0x57,
		if(16==operandSize)
		{
			disasm="PUSH    ";
			disasm+=Reg16Str[opCode&7];
		}
		else
		{
			disasm="PUSH    ";
			disasm+=Reg32Str[opCode&7];
		}
		break;
	case I486_OPCODE_PUSH_I8://          0x6A,
		disasm="PUSH    ";
		if(16==operandSize)
		{
			disasm+="WORD PTR ";
			disasm+=cpputil::Ubtox(EvalUimm8())+"H";
		}
		else
		{
			disasm+="DWORD PTR ";
			disasm+=cpputil::Ubtox(EvalUimm8())+"H";
		}
		break;
	case I486_OPCODE_PUSH_I://           0x68,
		disasm="PUSH    ";
		if(16==operandSize)
		{
			disasm+="WORD PTR ";
			disasm+=cpputil::Ustox(EvalUimm16())+"H";
		}
		else
		{
			disasm+="DWORD PTR ";
			disasm+=cpputil::Uitox(EvalUimm32())+"H";
		}
		break;
	case I486_OPCODE_PUSH_CS://          0x0E,
		disasm="PUSH    CS";
		break;
	case I486_OPCODE_PUSH_SS://          0x16,
		disasm="PUSH    SS";
		break;
	case I486_OPCODE_PUSH_DS://          0x1E,
		disasm="PUSH    DS";
		break;
	case I486_OPCODE_PUSH_ES://          0x06,
		disasm="PUSH    ES";
		break;
	case I486_OPCODE_PUSH_FS://          0x0FA0,
		disasm="PUSH    FS";
		break;
	case I486_OPCODE_PUSH_GS://          0x0FA8,
		disasm="PUSH    GS";
		break;


	case I486_OPCODE_POP_M://            0x8F,
		disasm=DisassembleTypicalOneOperand("POP",op1,operandSize);
		break;
	case I486_OPCODE_POP_EAX://          0x58,
	case I486_OPCODE_POP_ECX://          0x59,
	case I486_OPCODE_POP_EDX://          0x5A,
	case I486_OPCODE_POP_EBX://          0x5B,
	case I486_OPCODE_POP_ESP://          0x5C,
	case I486_OPCODE_POP_EBP://          0x5D,
	case I486_OPCODE_POP_ESI://          0x5E,
	case I486_OPCODE_POP_EDI://          0x5F,
		if(16==operandSize)
		{
			disasm="POP     ";
			disasm+=Reg16Str[opCode&7];
		}
		else
		{
			disasm="POP     ";
			disasm+=Reg32Str[opCode&7];
		}
		break;
	case I486_OPCODE_POP_SS://           0x17,
		disasm="POP     SS";
		break;
	case I486_OPCODE_POP_DS://           0x1F,
		disasm="POP     DS";
		break;
	case I486_OPCODE_POP_ES://           0x07,
		disasm="POP     ES";
		break;
	case I486_OPCODE_POP_FS://           0x0FA1,
		disasm="POP     FS";
		break;
	case I486_OPCODE_POP_GS://           0x0FA9,
		disasm="POP     GS";
		break;


	case I486_OPCODE_POPA://             0x61,
		switch(operandSize)
		{
		case 16:
			disasm="POPA";
			break;
		case 32:
			disasm="POPAD";
			break;
		}
		break;
	case I486_OPCODE_POPF://             0x9D,
		switch(operandSize)
		{
		case 16:
			disasm="POPF";
			break;
		case 32:
			disasm="POPFD";
			break;
		}
		break;


	case I486_OPCODE_OR_AL_FROM_I8:
		disasm="OR      AL,"+cpputil::Ubtox(EvalUimm8())+"H";
		break;
	case I486_OPCODE_OR_A_FROM_I:
		if(16==operandSize)
		{
			disasm="OR      AX,"+cpputil::Ustox(EvalUimm16())+"H";
		}
		else
		{
			disasm="OR      EAX,"+cpputil::Uitox(EvalUimm32())+"H";
		}
		break;
	case I486_OPCODE_OR_RM8_FROM_R8:
		op2.DecodeMODR_MForRegister(8,operand[0]);
		disasm=DisassembleTypicalTwoOperands("OR",op1,op2);
		break;
	case I486_OPCODE_OR_R8_FROM_RM8:
		op1.DecodeMODR_MForRegister(8,operand[0]);
		disasm=DisassembleTypicalTwoOperands("OR",op1,op2);
		break;
	case I486_OPCODE_OR_RM_FROM_R:
	case I486_OPCODE_OR_R_FROM_RM:
		disasm=DisassembleTypicalTwoOperands("OR",op1,op2);
		break;


	case I486_OPCODE_XOR_AL_FROM_I8:
		disasm="XOR     AL,"+cpputil::Ubtox(EvalUimm8())+"H";
		break;
	case I486_OPCODE_XOR_A_FROM_I:
		if(16==operandSize)
		{
			disasm="XOR     AX,"+cpputil::Ustox(EvalUimm16())+"H";
		}
		else
		{
			disasm="XOR     EAX,"+cpputil::Uitox(EvalUimm32())+"H";
		}
		break;
	case I486_OPCODE_XOR_RM8_FROM_R8:
		op2.DecodeMODR_MForRegister(8,operand[0]);
		disasm=DisassembleTypicalTwoOperands("XOR",op1,op2);
		break;
	case I486_OPCODE_XOR_R8_FROM_RM8:
		op1.DecodeMODR_MForRegister(8,operand[0]);
		disasm=DisassembleTypicalTwoOperands("XOR",op1,op2);
		break;
	case I486_OPCODE_XOR_RM_FROM_R:
	case I486_OPCODE_XOR_R_FROM_RM:
		disasm=DisassembleTypicalTwoOperands("XOR",op1,op2);
		break;


	case I486_OPCODE_XCHG_EAX_ECX://     0x91,
	case I486_OPCODE_XCHG_EAX_EDX://     0x92,
	case I486_OPCODE_XCHG_EAX_EBX://     0x93,
	case I486_OPCODE_XCHG_EAX_ESP://     0x94,
	case I486_OPCODE_XCHG_EAX_EBP://     0x95,
	case I486_OPCODE_XCHG_EAX_ESI://     0x96,
	case I486_OPCODE_XCHG_EAX_EDI://     0x97,
		if(16==operandSize)
		{
			disasm="XCHG    AX,";
			disasm+=Reg16Str[opCode&7];
		}
		else
		{
			disasm="XCHG    EAX,";
			disasm+=Reg32Str[opCode&7];
		}
		break;
	case I486_OPCODE_XCHG_RM8_R8://           0x86,
	case I486_OPCODE_XCHG_RM_R://             0x87,
		disasm=DisassembleTypicalTwoOperands("XCHG",op1,op2);
		break;


	case I486_OPCODE_XLAT://             0xD7,
		{
			disasm="XLAT    ";
			disasm+=SegmentOverrideString(segOverride);
			if(16==addressSize)
			{
				disasm+="[BX]";
			}
			else
			{
				disasm+="[EBX]";
			}
		}
		break;
	}

	{
		auto *sym=symTable.Find(cs.value,eip);
		if(nullptr!=sym && 0<sym->inLineComment.size())
		{
			for(auto i=disasm.size(); i<19; ++i)
			{
				disasm.push_back(' ');
			}
			disasm.push_back(' ');
			disasm.push_back(';');
			disasm.push_back(' ');
			disasm+=sym->inLineComment;
		}
	}

	return disasm;
}

std::string i486DX::Instruction::DisassembleTypicalOneOperand(std::string inst,const Operand &op,int operandSize) const
{
	auto sizeQual=i486DX::Operand::GetSizeQualifierToDisassembly(op,operandSize);
	auto segQual=i486DX::Operand::GetSegmentQualifierToDisassembly(segOverride,op);
	auto disasm=inst;
	cpputil::ExtendString(disasm,8);
	disasm+=sizeQual+segQual+op.Disassemble();
	return disasm;
}

std::string i486DX::Instruction::DisassembleTypicalOneOperandAndImm(std::string inst,const Operand &op,unsigned int imm,int operandSize) const
{
	auto sizeQual=i486DX::Operand::GetSizeQualifierToDisassembly(op,operandSize);
	auto segQual=i486DX::Operand::GetSegmentQualifierToDisassembly(segOverride,op);
	auto disasm=inst;
	cpputil::ExtendString(disasm,8);
	disasm+=sizeQual+segQual+op.Disassemble()+",";
	switch(operandSize)
	{
	case 8:
		disasm+=cpputil::Ubtox(imm)+"H";
		break;
	case 16:
		disasm+=cpputil::Ustox(imm)+"H";
		break;
	default:
		disasm+=cpputil::Uitox(imm)+"H";
		break;
	}
	return disasm;
}

std::string i486DX::Instruction::DisassembleTypicalOneImm(std::string inst,unsigned int imm,int operandSize) const
{
	auto disasm=inst;
	cpputil::ExtendString(disasm,8);
	switch(operandSize)
	{
	case 8:
		disasm+=cpputil::Ubtox(imm)+"H";
		break;
	case 16:
		disasm+=cpputil::Ustox(imm)+"H";
		break;
	default:
	case 32:
		disasm+=cpputil::Uitox(imm)+"H";
		break;
	}
	return disasm;
}

std::string i486DX::Instruction::DisassembleTypicalRM8_I8(std::string inst,const Operand &op1,unsigned int I8) const
{
	auto sizeQual=i486DX::Operand::GetSizeQualifierToDisassembly(op1,8);
	auto segQual=i486DX::Operand::GetSegmentQualifierToDisassembly(segOverride,op1);
	auto disasm=inst;
	cpputil::ExtendString(disasm,8);
	disasm+=sizeQual+segQual+op1.Disassemble()+","+cpputil::Ubtox(I8)+"H";
	return disasm;
}

std::string i486DX::Instruction::DisassembleTypicalRM_I8(std::string inst,const Operand &op1,unsigned int I8) const
{
	auto sizeQual=i486DX::Operand::GetSizeQualifierToDisassembly(op1,operandSize);
	auto segQual=i486DX::Operand::GetSegmentQualifierToDisassembly(segOverride,op1);
	auto disasm=inst;
	cpputil::ExtendString(disasm,8);
	disasm+=sizeQual+segQual+op1.Disassemble()+","+cpputil::Ubtox(I8)+"H";
	return disasm;
}

std::string i486DX::Instruction::DisassembleTypicalTwoOperands(std::string inst,const Operand &op1,const Operand &op2) const
{
	std::string disasm=inst,op1SizeQual,op2SizeQual,op1SegQual,op2SegQual;
	cpputil::ExtendString(disasm,8);

	op1SegQual=i486DX::Operand::GetSegmentQualifierToDisassembly(segOverride,op1);
	op2SegQual=i486DX::Operand::GetSegmentQualifierToDisassembly(segOverride,op2);
	disasm+=op1SizeQual+op1SegQual+op1.Disassemble();
	disasm.push_back(',');
	disasm+=op2SizeQual+op2SegQual+op2.Disassemble();

	return disasm;
}

std::string i486DX::Instruction::DisassembleIOLabel(unsigned int CS,unsigned int EIP,const i486SymbolTable &symTable,const std::map <unsigned int,std::string> &ioTable,unsigned int imm) const
{
	std::string disasm;
	auto symbolPtr=symTable.Find(CS,EIP);
	if(nullptr!=symbolPtr && true==symbolPtr->immIsIOAddr)
	{
		auto found=ioTable.find(imm);
		if(ioTable.end()!=found)
		{
			disasm+=" (";
			disasm+=found->second;
			disasm+=")";
		}
	}
	return disasm;
}

/* static */ std::string i486DX::Get8BitRegisterNameFromMODR_M(unsigned char MOD_RM)
{
	auto REG_OPCODE=((MOD_RM>>3)&7);
	return Reg8Str[REG_OPCODE];
}
/* static */ std::string i486DX::Get16BitRegisterNameFromMODR_M(unsigned char MOD_RM)
{
	auto REG_OPCODE=((MOD_RM>>3)&7);
	return Reg16Str[REG_OPCODE];
}
/* static */ std::string i486DX::Get32BitRegisterNameFromMODR_M(unsigned char MOD_RM)
{
	auto REG_OPCODE=((MOD_RM>>3)&7);
	return Reg32Str[REG_OPCODE];
}
/* static */ std::string i486DX::Get16or32BitRegisterNameFromMODR_M(int dataSize,unsigned char MOD_RM)
{
	if(16==dataSize)
	{
		auto REG_OPCODE=((MOD_RM>>3)&7);
		return Reg16Str[REG_OPCODE];
	}
	else
	{
		auto REG_OPCODE=((MOD_RM>>3)&7);
		return Reg32Str[REG_OPCODE];
	}
}


unsigned int i486DX::RunOneInstruction(Memory &mem,InOut &io)
{
	// Considered to make it state.EIP=((state.EIP+offset)&operandSizeMask[inst.operandSize>>3]);
	// and delete EIPIncrement=0;  This will save one add and one mov instructions per jump.
	// However, this change may break backward jump to offset 0000H, when operandSize=16.
	// Therefore not taken at this time.
	#define CONDITIONALJUMP8(jumpCond) \
	{ \
		if(true==(jumpCond)) \
		{ \
			auto offset=inst.EvalSimm8(); \
			state.EIP=((state.EIP+offset+inst.numBytes)&operandSizeMask[inst.operandSize>>3]); \
			clocksPassed=3; \
			EIPIncrement=0; \
		} \
		else \
		{ \
			clocksPassed=1; \
		} \
	}

	#define CONDITIONALJUMP16OR32(jumpCond)  \
	{ \
		if(true==(jumpCond)) \
		{ \
			auto offset=inst.EvalSimm16or32(inst.operandSize); \
			auto destin=state.EIP+offset+inst.numBytes; \
			destin&=operandSizeMask[inst.operandSize>>3]; \
			state.EIP=destin; \
			clocksPassed=3; \
			EIPIncrement=0; \
		} \
		else \
		{ \
			clocksPassed=1; \
		} \
	}

	#define SETxx(cond) \
	{ \
		OperandValue value; \
		if(true==(cond)) \
		{ \
			value.MakeByte(1); \
			clocksPassed=4; \
		} \
		else \
		{ \
			value.MakeByte(0); \
			clocksPassed=3; \
		} \
		StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value); \
	}

	#define BINARYOP_R_FROM_RM(func,clock_for_addr,update) \
	{ \
		if(op2.operandType==OPER_ADDR) \
		{ \
			clocksPassed=(clock_for_addr); \
		} \
		else \
		{ \
			clocksPassed=1; \
		} \
		auto regNum=inst.GetREG(); \
		auto op32or16=inst.operandSize>>3; \
		unsigned int dst=(state.reg32()[regNum]&operandSizeMask[op32or16]); \
		auto src=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,(op32or16)); \
		if(true==state.exception) \
		{ \
			break; \
		} \
		(func)(inst.operandSize,dst,src.GetAsDword()); \
		if(true==(update)) \
		{ \
			state.reg32()[regNum]&=operandSizeAndPattern[op32or16]; \
			state.reg32()[regNum]|=(dst&operandSizeMask[op32or16]); \
		} \
	}

	#define BINARYOP_RM_FROM_R(func,clock_for_addr,update) \
	{ \
		if(op1.operandType==OPER_ADDR || op2.operandType==OPER_ADDR) \
		{ \
			clocksPassed=(clock_for_addr); \
		} \
		else \
		{ \
			clocksPassed=1; \
		} \
		auto regNum=inst.GetREG(); \
		auto op32or16=inst.operandSize>>3; \
		auto dst=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,op32or16); \
		auto src=(state.reg32()[regNum]&operandSizeMask[op32or16]); \
		if(true==state.exception) \
		{ \
			break; \
		} \
		auto i=dst.GetAsDword(); \
		(func)(inst.operandSize,i,src); \
		if(true==(update)) \
		{ \
			dst.SetDword(i); \
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,dst); \
		} \
	}

	#define BINARYOP_RM8_R8(func,clock_for_addr,update) \
	{ \
		if(op1.operandType==OPER_ADDR || op2.operandType==OPER_ADDR) \
		{ \
			clocksPassed=(clock_for_addr); \
		} \
		else \
		{ \
			clocksPassed=1; \
		} \
		unsigned int reg=REG_AL+inst.GetREG(); \
		auto value1=EvaluateOperand8(mem,inst.addressSize,inst.segOverride,op1); \
		auto value2=GetRegisterValue8(reg); \
		if(true==state.exception) \
		{ \
			break; \
		} \
		auto i=value1.GetAsDword(); \
		(func)(i,value2); \
		if(true==update) \
		{ \
			value1.SetDword(i); \
			StoreOperandValue8(op1,mem,inst.addressSize,inst.segOverride,value1); \
		} \
	}

	#define BINARYOP_R8_RM8(func,clock_for_addr,update) \
	{ \
		if(op1.operandType==OPER_ADDR || op2.operandType==OPER_ADDR) \
		{ \
			clocksPassed=(clock_for_addr); \
		} \
		else \
		{ \
			clocksPassed=1; \
		} \
		unsigned int reg=REG_AL+inst.GetREG(); \
		auto value1=GetRegisterValue8(reg); \
		auto value2=EvaluateOperand8(mem,inst.addressSize,inst.segOverride,op2); \
		if(true==state.exception) \
		{ \
			break; \
		} \
		(func)(value1,value2.GetAsDword()); \
		if(true==update) \
		{ \
			SetRegisterValue8(reg,value1); \
		} \
	}

	#define BINARYOP_AL_I8(func,update) \
	{ \
		clocksPassed=1; \
		auto al=GetAL(); \
		auto v=inst.EvalUimm8(); \
		(func)(al,v); \
		if(true==update) \
		{ \
			SetAL(al); \
		} \
	}

	#define BINARYOP_xAX_I(func16,func32,update) \
	{ \
		clocksPassed=1; \
		if(16==inst.operandSize) \
		{ \
			auto ax=GetAX(); \
			auto v=inst.EvalUimm16(); \
			(func16)(ax,v); \
			if(true==update) \
			{ \
				SetAX(ax); \
			} \
		} \
		else \
		{ \
			auto eax=GetEAX(); \
			auto v=inst.EvalUimm32(); \
			(func32)(eax,v); \
			if(true==update) \
			{ \
				SetEAX(eax); \
			} \
		} \
	}

	// For RCL and RCR see reminder #20200123-1
	#define ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8(ctr) \
	{ \
		auto value=EvaluateOperand8(mem,inst.addressSize,inst.segOverride,op1); \
		auto i=value.GetAsDword(); \
		if(true!=state.exception) \
		{ \
			switch(inst.GetREG()) \
			{ \
			case 0: \
				RolByte(i,ctr); \
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
				break; \
			case 1: \
				RorByte(i,ctr); \
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
				break; \
			case 2: \
				RclByte(i,ctr); \
				clocksPassed=(OPER_ADDR==op1.operandType ? 10 : 11); \
				break; \
			case 3: \
				RcrByte(i,ctr); \
				clocksPassed=(OPER_ADDR==op1.operandType ? 10 : 11); \
				break; \
			case 4: \
				ShlByte(i,ctr); \
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
				break; \
			case 5: \
				ShrByte(i,ctr); \
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
				break; \
			case 6: \
				Abort("Undefined REG for "+cpputil::Ubtox(inst.opCode)); \
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
				return 0; \
			case 7: \
				SarByte(i,ctr); \
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
				break; \
			} \
			value.SetDword(i); \
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value); \
		} \
	}

	// For RCL and RCR See reminder #20200123-1
	#define ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM(ctr) \
	{ \
		auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8); \
		auto i=value.GetAsDword(); \
		if(true!=state.exception) \
		{ \
			switch(inst.GetREG()) \
			{ \
			case 0: \
				RolByteWordOrDword(inst.operandSize,i,ctr); \
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
				break; \
			case 1: \
				RorByteWordOrDword(inst.operandSize,i,ctr); \
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
				break; \
			case 2: \
				RclWordOrDword(inst.operandSize,i,ctr); \
				clocksPassed=(OPER_ADDR==op1.operandType ? 10 : 11); \
				break; \
			case 3: \
				clocksPassed=(OPER_ADDR==op1.operandType ? 10 : 11); \
				RcrWordOrDword(inst.operandSize,i,ctr); \
				break; \
			case 4: \
				ShlWordOrDword(inst.operandSize,i,ctr); \
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
				break; \
			case 5: \
				ShrWordOrDword(inst.operandSize,i,ctr); \
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
				break; \
			case 6: \
				Abort("Undefined REG for "+cpputil::Ubtox(inst.opCode)); \
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
				return 0; \
			case 7: \
				SarByteWordOrDword(inst.operandSize,i,ctr); \
				clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2); \
				break; \
			} \
			value.SetDword(i); \
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value); \
		} \
	}


	static const unsigned int reg8AndPattern[]=
	{
		0xFFFFFF00,   // AL
		0xFFFFFF00,   // CL
		0xFFFFFF00,   // DL
		0xFFFFFF00,   // BL
		0xFFFF00FF,   // AH
		0xFFFF00FF,   // CH
		0xFFFF00FF,   // DH
		0xFFFF00FF,   // BH
	};
	static const unsigned char reg8Shift[]=
	{
		0,   // AL
		0,   // CL
		0,   // DL
		0,   // BL
		8,   // AH
		8,   // CH
		8,   // DH
		8,   // BH
	};
	static const unsigned int operandSizeMask[]= // Same for addressSizeMask
	{
		0x00000000,  // 0
		0x000000FF,  // (8>>3)
		0x0000FFFF,  // (16>>3)
		0x00FFFFFF,  // (24>>3)
		0xFFFFFFFF,  // (32>>3)
	};
	static const unsigned int operandSizeSignBit[]=
	{
		0x00000000,  // 0
		0x00000080,  // (8>>3)
		0x00008000,  // (16>>3)
		0x00800000,  // (24>>3)
		0x80000000,  // (32>>3)
	};
	static const unsigned int operandSizeAndPattern[]=
	{
		0xFFFFFFFF, // 0
		0xFFFFFF00, // (8>>3)
		0xFFFF0000, // (16>>3)
		0xFF000000, // (24>>3)
		0x00000000, // (32>>3)
	};



	if(true==state.halt)
	{
		return 1;
	}

	state.holdIRQ=false;

	Instruction inst;
	Operand op1,op2;
	FetchInstruction(state.CSEIPWindow,inst,op1,op2,state.CS(),state.EIP,mem);
	if(nullptr!=debuggerPtr)
	{
		debuggerPtr->BeforeRunOneInstruction(*this,mem,io,inst);
	}

	int EIPIncrement=inst.numBytes;
	unsigned int clocksPassed=0;

	switch(opCodeRenumberTable[inst.opCode])
	{
	case I486_RENUMBER_UNDEFINED_SHOOT_INT6:
		Interrupt(INT_INVALID_OPCODE,mem,0,0);
		EIPIncrement=0;
		clocksPassed=26;  // ? How many clocks should I use?
		break;

	case I486_RENUMBER_C0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_I8://0xC0,// ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8(inst.EvalUimm8()&31); // [1] pp.26-243 Only bottom 5 bits are used.
		break;
	case I486_RENUMBER_D0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_1://0xD0, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8(1)
		break;
	case I486_RENUMBER_D2_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_CL://0xD2,// ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8(GetCL()&31)
		break;

	case I486_RENUMBER_C1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_I8:// =0xC1, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM(inst.EvalUimm8()&31); // [1] pp.26-243 Only bottom 5 bits are used.
		break;
	case I486_RENUMBER_D1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_1://=0xD1, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM(1);
		break;
	case I486_RENUMBER_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_CL://0xD3, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
		ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM(GetCL()&31);
		break;

	case I486_RENUMBER_F6_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV: //=0xF6
		switch(inst.GetREG())
		{
		case 0: // TEST
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 2 : 1);
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				unsigned int byte=value.byteData[0];
				AndByte(byte,inst.EvalUimm8());
				// SetCF(false); Done in AndByte
				// SetOF(false); Done in AndByte
			}
			break;
		case 2: // NOT
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 3 : 1);
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				if(true!=state.exception)
				{
					value.byteData[0]=~value.byteData[0];
					StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
				}
			}
			break;
		case 3: // NEG
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 3 : 1);
				auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				auto i=value1.GetAsSignedDword();
				SetOF(-128==i);
				SetZF(0==i);
				SetCF(0!=i);
				i=-i;
				SetPF(CheckParity(i&0xFF));
				SetSF(0!=(i&0x80));
				value1.SetSignedDword(i);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
			}
			break;
		case 4: // MUL
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 18 : 13);
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				auto mul=GetAL()*value.byteData[0];
				SetAX(mul);
				if(0!=(mul&0xff00))
				{
					SetCFOF();
					//SetCF(true);
					//SetOF(true);
				}
				else
				{
					ClearCFOF();
					//SetCF(false);
					//SetOF(false);
				}
			}
			break;
		case 5: // IMUL R/M8
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 18 : 13);
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				if(true!=state.exception)
				{
					int AL=GetAL();
					int OP=value.byteData[0];
					AL=(AL&0x7F)-(AL&0x80);
					OP=(OP&0x7F)-(OP&0x80);
					auto imul=AL*OP;
					SetAX(imul&0xFFFF);
					if(0==(imul&0xFF80) || 0xFF80==(imul&0xFF80))
					{
						ClearCFOF();
						//SetCF(false);
						//SetOF(false);
					}
					else
					{
						SetCFOF();
						//SetCF(true);
						//SetOF(true);
					}
				}
			}
			break;
		case 6: // DIV
			{
				clocksPassed=16;
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				if(0==value.byteData[0])
				{
					Interrupt(0,mem,0,0); // [1] pp.26-28
					EIPIncrement=0;
					// I don't think INT 0 was issued unless division by zero.
					// I thought it just overflew if quo didn't fit in the target register, am I wrong?
				}
				else
				{
					unsigned int quo=GetAX()/value.byteData[0];
					unsigned int rem=GetAX()%value.byteData[0];
					SetAL(quo);
					SetAH(rem);
				}
			}
			break;
		case 7: // IDIV
			{
				clocksPassed=20;
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				if(0==value.byteData[0])
				{
					Interrupt(0,mem,0,0); // [1] pp.26-28
					EIPIncrement=0;
					// I don't think INT 0 was issued unless division by zero.
					// I thought it just overflew if quo didn't fit in the target register, am I wrong?
				}
				else
				{
					int ax=GetAX();
					ax=(0x7FFF&ax)-(0x8000&ax);
					int rm8=value.byteData[0];
					rm8=(rm8&0x7F)-(rm8&0x80);

					int quo=ax/rm8;
					int rem=ax%rm8;
					quo=(quo+0x10000)&0xff;
					rem=(rem+0x10000)&0xff;
					SetAL(quo);
					SetAH(rem);
				}
			}
			break;
		default:
			{
				std::string msg;
				msg="Undefined REG for ";
				msg+=cpputil::Ubtox(inst.opCode);
				msg+="(REG=";
				msg+=cpputil::Ubtox(inst.GetREG());
				msg+=")";
				Abort(msg);
				return 0;
			}
			clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2);
			break;
		}
		break;
	case I486_RENUMBER_F7_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV: //=0xF7,
		switch(inst.GetREG())
		{
		case 0: // TEST
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 2 : 1);
				auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				auto value2=inst.EvalUimm8or16or32(inst.operandSize);
				unsigned int i1=value1.GetAsDword();
				AndWordOrDword(inst.operandSize,i1,value2);
				// SetCF(false); Done in AndWordOrDword
				// SetOF(false);
			}
			break;
		case 2: // NOT
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 2 : 1);
				auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				value1.byteData[0]=~value1.byteData[0];
				value1.byteData[1]=~value1.byteData[1];
				value1.byteData[2]=~value1.byteData[2];
				value1.byteData[3]=~value1.byteData[3];
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
			}
			break;
		case 3: // NEG
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 3 : 1);
				auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				auto i=value1.GetAsSignedDword();
				if(16==inst.operandSize)
				{
					SetOF(-32768==i);
				}
				else if(32==inst.operandSize)
				{
					SetOF(-0x80000000LL==i);
				}
				i=-i;
				SetZF(0==i);
				SetCF(0!=i);
				SetSF(i<0);
				value1.SetSignedDword(i);
				SetPF(CheckParity(i&0xFF));
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
			}
			break;
		case 4: // MUL
			if(16==inst.operandSize)
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 26 : 13);
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,2);
				auto DXAX=GetAX()*value.GetAsWord();
				SetAX(DXAX&0xffff);
				SetDX((DXAX>>16)&0xffff);
				if(0!=(DXAX&0xffff0000))
				{
					SetCFOF();
					//SetCF(true);
					//SetOF(true);
				}
				else
				{
					ClearCFOF();
					//SetCF(false);
					//SetOF(false);
				}
			}
			else
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 42 : 13);
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,4);
				unsigned long long EAX=GetEAX();
				unsigned long long MUL=value.GetAsDword();
				unsigned long long EDXEAX=EAX*MUL;
				SetEAX(EDXEAX&0xffffffff);
				SetEDX((EDXEAX>>32)&0xffffffff);
				if(0!=(EDXEAX&0xffffffff00000000))
				{
					SetCFOF();
					//SetCF(true);
					//SetOF(true);
				}
				else
				{
					ClearCFOF();
					//SetCF(false);
					//SetOF(false);
				}
			}
			break;
		case 5: // IMUL
			{
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				if(true==state.exception)
				{
					EIPIncrement=0;
				}
				else
				{
					int multiplicand=value.GetAsSignedDword();
					if(16==inst.operandSize)
					{
						clocksPassed=20; // 13-26.  I don't know exactly how to calculate it.
						int DXAX=GetAX();
						DXAX=(DXAX&0x7FFF)-(DXAX&0x8000);
						DXAX*=multiplicand;

						SetAX(DXAX&0xFFFF);
						SetDX((DXAX>>16)&0xFFFF);

						auto signExtCheck=DXAX&0xFFFF8000;
						if(0==signExtCheck || signExtCheck==0xFFFF8000)
						{
							ClearCFOF();
							//SetOF(false);
							//SetCF(false);
						}
						else
						{
							SetCFOF();
							//SetOF(true);
							//SetCF(true);
						}
					}
					else
					{
						clocksPassed=30; // 13-42.  I don't know exactly how to calculate it.
						long long int EDXEAX=GetEAX();
						EDXEAX=(EDXEAX&0x7FFFFFFF)-(EDXEAX&0x80000000);
						EDXEAX*=(long long int)multiplicand;

						SetEAX(EDXEAX&0xFFFFFFFF);
						SetEDX((EDXEAX>>32)&0xFFFFFFFF);

						auto signExtCheck=EDXEAX&0xFFFFFFFF80000000LL;
						if(0==signExtCheck || signExtCheck==0xFFFFFFFF80000000LL)
						{
							ClearCFOF();
							//SetOF(false);
							//SetCF(false);
						}
						else
						{
							SetCFOF();
							//SetOF(true);
							//SetCF(true);
						}
					}
				}
			}
			break;
		case 6: // DIV
			{
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				unsigned int denom=value.GetAsDword();
				if(true==state.exception)
				{
					EIPIncrement=0;
				}
				else if(0==denom)
				{
					clocksPassed=40;
					Interrupt(0,mem,0,0); // [1] pp.26-28
					EIPIncrement=0;
					// I don't think INT 0 was issued unless division by zero.
					// I thought it just overflew if quo didn't fit in the target register, am I wrong?
				}
				else if(16==inst.operandSize)
				{
					clocksPassed=24;
					unsigned int DXAX=(GetDX()<<16)|GetAX();
					unsigned int quo=DXAX/denom;
					unsigned int rem=DXAX%denom;
					SetAX(quo);
					SetDX(rem);
				}
				else if(32==inst.operandSize)
				{
					clocksPassed=40;
					unsigned long long int EDXEAX=GetEDX();
					EDXEAX<<=32;
					EDXEAX|=GetEAX();
					unsigned int quo=(unsigned int)(EDXEAX/denom);
					unsigned int rem=(unsigned int)(EDXEAX%denom);
					SetEAX(quo);
					SetEDX(rem);
				}
			}
			break;
		case 7: // IDIV
			{
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				int denom=value.GetAsSignedDword();
				if(true==state.exception)
				{
					EIPIncrement=0;
				}
				else if(0==denom)
				{
					clocksPassed=40;
					Interrupt(0,mem,0,0); // [1] pp.26-28
					EIPIncrement=0;
					// I don't think INT 0 was issued unless division by zero.
					// I thought it just overflew if quo didn't fit in the target register, am I wrong?
				}
				else if(16==inst.operandSize)
				{
					clocksPassed=24;

					int DXAX=GetDX();
					DXAX=(DXAX&0x7FFF)-(DXAX&0x8000);
					DXAX<<=inst.operandSize;
					DXAX|=GetAX();

					int quo=DXAX/denom;
					int rem=DXAX%denom;

					SetAX(quo&0xFFFF);
					SetDX(rem&0xFFFF);
				}
				else if(32==inst.operandSize)
				{
					clocksPassed=40;

					long long int EDXEAX=GetEDX();
					EDXEAX=(EDXEAX&0x7FFFFFFF)-(EDXEAX&0x80000000);
					EDXEAX<<=32;
					EDXEAX|=GetEAX();

					long long int quo=EDXEAX/denom;
					long long int rem=EDXEAX%denom;

					SetEAX(quo&0xFFFFFFFF);
					SetEDX(rem&0xFFFFFFFF);
				}
			}
			break;
		default:
			Abort("Undefined REG for "+cpputil::Ubtox(inst.opCode));
			clocksPassed=(OPER_ADDR==op1.operandType ? 4 : 2);
			return 0;
		}
		break;

	case I486_RENUMBER_ADC_AL_FROM_I8://  0x14,
		BINARYOP_AL_I8(AdcByte,true);
		break;
	case I486_RENUMBER_ADD_AL_FROM_I8://  0x04,
		BINARYOP_AL_I8(AddByte,true);
		break;
	case I486_RENUMBER_AND_AL_FROM_I8://  0x24,
		BINARYOP_AL_I8(AndByte,true);
		break;
	case I486_RENUMBER_CMP_AL_FROM_I8://  0x3C,
		BINARYOP_AL_I8(SubByte,false);
		break;
	case I486_RENUMBER_OR_AL_FROM_I8://  0x0C,
		BINARYOP_AL_I8(OrByte,true);
		break;
	case I486_RENUMBER_SBB_AL_FROM_I8://  0x1C,
		BINARYOP_AL_I8(SbbByte,true);
		break;
	case I486_RENUMBER_SUB_AL_FROM_I8://  0x2C,
		BINARYOP_AL_I8(SubByte,true);
		break;
	case I486_RENUMBER_TEST_AL_FROM_I8://  0xA8,
		BINARYOP_AL_I8(AndByte,false);
		break;
	case I486_RENUMBER_XOR_AL_FROM_I8:
		BINARYOP_AL_I8(XorByte,true);
		break;

	case I486_RENUMBER_ADC_A_FROM_I://    0x15,
		BINARYOP_xAX_I(AdcWord,AdcDword,true);
		break;
	case I486_RENUMBER_ADD_A_FROM_I://    0x05,
		BINARYOP_xAX_I(AddWord,AddDword,true);
		break;
	case I486_RENUMBER_AND_A_FROM_I://    0x25,
		BINARYOP_xAX_I(AndWord,AndDword,true);
		break;
	case I486_RENUMBER_CMP_A_FROM_I://    0x3D,
		BINARYOP_xAX_I(SubWord,SubDword,false);
		break;
	case I486_RENUMBER_OR_A_FROM_I://    0x0D,
		BINARYOP_xAX_I(OrWord,OrDword,true);
		break;
	case I486_RENUMBER_SBB_A_FROM_I://    0x1D,
		BINARYOP_xAX_I(SbbWord,SbbDword,true);
		break;
	case I486_RENUMBER_SUB_A_FROM_I://    0x2D,
		BINARYOP_xAX_I(SubWord,SubDword,true);
		break;
	case I486_RENUMBER_TEST_A_FROM_I://    0xA9,
		BINARYOP_xAX_I(AndWord,AndDword,false);
		break;
	case I486_RENUMBER_XOR_A_FROM_I:
		BINARYOP_xAX_I(XorWord,XorDword,true);
		break;

	case I486_RENUMBER_ADC_RM8_FROM_R8:// 0x10,
		BINARYOP_RM8_R8(AdcByte,3,true);
		break;
	case I486_RENUMBER_ADD_RM8_FROM_R8:// 0x00,
		BINARYOP_RM8_R8(AddByte,3,true);
		break;
	case I486_RENUMBER_AND_RM8_FROM_R8:// 0x20,
		BINARYOP_RM8_R8(AndByte,3,true);
		break;
	case I486_RENUMBER_CMP_RM8_FROM_R8:// 0x38,
		BINARYOP_RM8_R8(SubByte,3,false);
		break;
	case I486_RENUMBER_OR_RM8_FROM_R8:// 0x08,
		BINARYOP_RM8_R8(OrByte,3,true);
		break;
	case I486_RENUMBER_SBB_RM8_FROM_R8:// 0x18,
		BINARYOP_RM8_R8(SbbByte,3,true);
		break;
	case I486_RENUMBER_SUB_RM8_FROM_R8:// 0x28,
		BINARYOP_RM8_R8(SubByte,3,true);
		break;
	case I486_RENUMBER_XOR_RM8_FROM_R8:
		BINARYOP_RM8_R8(XorByte,3,true);
		break;
	case I486_RENUMBER_TEST_RM8_FROM_R8:// 0x84,
		BINARYOP_RM8_R8(AndByte,2,false);
		break;

	case I486_RENUMBER_ADC_R8_FROM_RM8:// 0x12,
		BINARYOP_R8_RM8(AdcByte,3,true);
		break;
	case I486_RENUMBER_ADD_R8_FROM_RM8:// 0x02,
		BINARYOP_R8_RM8(AddByte,3,true);
		break;
	case I486_RENUMBER_AND_R8_FROM_RM8:// 0x22,
		BINARYOP_R8_RM8(AndByte,3,true);
		break;
	case I486_RENUMBER_CMP_R8_FROM_RM8:// 0x3A,
		BINARYOP_R8_RM8(SubByte,3,false);
		break;
	case I486_RENUMBER_OR_R8_FROM_RM8:// 0x0A,
		BINARYOP_R8_RM8(OrByte,3,true);
		break;
	case I486_RENUMBER_SBB_R8_FROM_RM8:// 0x1A,
		BINARYOP_R8_RM8(SbbByte,3,true);
		break;
	case I486_RENUMBER_SUB_R8_FROM_RM8:// 0x2A,
		BINARYOP_R8_RM8(SubByte,3,true);
		break;
	case I486_RENUMBER_XOR_R8_FROM_RM8:
		BINARYOP_R8_RM8(XorByte,3,true);
		break;

	case I486_RENUMBER_ADC_RM_FROM_R://   0x11,
		BINARYOP_RM_FROM_R(AdcWordOrDword,3,true);
		break;
	case I486_RENUMBER_ADD_RM_FROM_R://   0x01,
		BINARYOP_RM_FROM_R(AddWordOrDword,3,true);
		break;
	case I486_RENUMBER_AND_RM_FROM_R://   0x21,
		BINARYOP_RM_FROM_R(AndWordOrDword,3,true);
		break;
	case I486_RENUMBER_CMP_RM_FROM_R://   0x39,
		BINARYOP_RM_FROM_R(SubWordOrDword,3,false);
		break;
	case I486_RENUMBER_SBB_RM_FROM_R://   0x19,
		BINARYOP_RM_FROM_R(SbbWordOrDword,3,true);
		break;
	case I486_RENUMBER_SUB_RM_FROM_R://   0x29,
		BINARYOP_RM_FROM_R(SubWordOrDword,3,true);
		break;
	case I486_RENUMBER_TEST_RM_FROM_R://   0x85,
		BINARYOP_RM_FROM_R(AndWordOrDword,1,false);
		break;
	case I486_RENUMBER_OR_RM_FROM_R://   0x09,
		BINARYOP_RM_FROM_R(OrWordOrDword,3,true);
		break;
	case I486_RENUMBER_XOR_RM_FROM_R:
		BINARYOP_RM_FROM_R(XorWordOrDword,3,true);
		break;

	case I486_RENUMBER_ADC_R_FROM_RM://   0x13,
		BINARYOP_R_FROM_RM(AdcWordOrDword,3,true);
		break;
	case I486_RENUMBER_ADD_R_FROM_RM://    0x03,
		BINARYOP_R_FROM_RM(AddWordOrDword,3,true);
		break;
	case I486_RENUMBER_AND_R_FROM_RM://    0x23,
		BINARYOP_R_FROM_RM(AndWordOrDword,3,true);
		break;
	case I486_RENUMBER_CMP_R_FROM_RM://    0x3B,
		BINARYOP_R_FROM_RM(SubWordOrDword,3,false);
		break;
	case I486_RENUMBER_SBB_R_FROM_RM://    0x1B,
		BINARYOP_R_FROM_RM(SbbWordOrDword,3,true);
		break;
	case I486_RENUMBER_SUB_R_FROM_RM://    0x2B,
		BINARYOP_R_FROM_RM(SubWordOrDword,3,true);
		break;
	case I486_RENUMBER_OR_R_FROM_RM://    0x0B,
		BINARYOP_R_FROM_RM(OrWordOrDword,3,true);
		break;
	case I486_RENUMBER_XOR_R_FROM_RM:
		BINARYOP_R_FROM_RM(XorWordOrDword,3,true);
		break;


	case I486_RENUMBER_AAA: // 0x37
		clocksPassed=3;
		if(9<(GetAL()&0x0f) || true==GetAF())
		{
			auto AL=((GetAL()+6)&0x0F);
			auto AH=(GetAH()+1);
			SetAX((AH<<8)|AL);
			SetCF(true);
			SetAF(true);
		}
		else
		{
			SetAL(GetAL()&0x0F);
			SetCF(false);
			SetAF(false);
		}
		break;

	case I486_RENUMBER_AAD://    0xD5,
		{
			clocksPassed=14;
			auto AL=GetAH()*inst.EvalUimm8()+GetAL();
			SetAL(AL);
			SetAH(0);
			SetZF(0==GetAX());
			SetSF(0!=(GetAL()&0x80));
			SetPF(CheckParity(AL));
		}
		break;
	case I486_RENUMBER_AAM://    0xD4,
		{
			clocksPassed=15;
			auto AL=GetAL();
			auto quo=AL/inst.EvalUimm8();
			auto rem=AL%inst.EvalUimm8();
			SetAH(quo);
			SetAL(rem);
			SetZF(0==GetAL());   // ?
			SetSF(0!=(GetAH()&0x80));
			SetPF(CheckParity(GetAL()));
		}
		break;

	case I486_RENUMBER_AAS:
		{ // BP 000C:0371
			clocksPassed=3;
			auto AL=GetAL();
			if((AL&0x0F)>9 || true==GetAF())
			{
				SetAL((AL-6)&0x0F);
				SetAH((GetAH()-1)&0xFF);
				SetAF(true);
				SetCF(true);
			}
			else
			{
				SetAL(AL&0x0F); // [1] pp. 26-21 "In either case, the AL register is left with its top nibble set to 0."
				SetAF(false);
				SetCF(false);
			}
		}
		break;

	case I486_RENUMBER_ARPL://       0x63,
		{
			clocksPassed=9;
			if(0!=(state.EFLAGS&EFLAGS_VIRTUAL86))
			{
				RaiseException(EXCEPTION_UD,0);
				HandleException(true,mem,inst.numBytes);
				EIPIncrement=0;
				break;
			}

			auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
			auto value2=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8);
			auto dst=value1.GetAsWord();
			auto src=value2.GetAsWord();
			if((dst&3)<(src&3))
			{
				dst&=(~3);
				dst|=(src&3);
				SetZF(false);
				value1.MakeWord(dst);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
			}
			else
			{
				SetZF(true);
			}
		}
		break;


	case I486_RENUMBER_BOUND: // 0x62
		{
			clocksPassed=7;
			if(OPER_ADDR==op2.operandType)
			{
				unsigned int offset;
				auto &seg=*ExtractSegmentAndOffset(offset,op2,inst.segOverride);
				int min,max,idx;
				if(16==inst.operandSize)
				{
					unsigned int reg=REG_AX+inst.GetREG();
					unsigned int i=GetRegisterValue(reg);
					idx=(i&0x7FFF)-(i&0x8000);

					int MIN,MAX;
					MIN=FetchWord(inst.addressSize,seg,offset  ,mem);
					MAX=FetchWord(inst.addressSize,seg,offset+2,mem);

					min=(MIN&0x7FFF)-(MIN&0x8000);
					max=(MAX&0x7FFF)-(MAX&0x8000);
				}
				else
				{
					unsigned int reg=REG_EAX+inst.GetREG();
					unsigned int i=GetRegisterValue(reg);
					idx=(i&0x7FFFFFFF)-(i&0x80000000);

					int MIN,MAX;
					MIN=FetchDword(inst.addressSize,seg,offset  ,mem);
					MAX=FetchDword(inst.addressSize,seg,offset+4,mem);

					min=(MIN&0x7FFFFFFF)-(MIN&0x80000000);
					max=(MAX&0x7FFFFFFF)-(MAX&0x80000000);
				}
				if(idx<min || max<idx)
				{
					Interrupt(5,mem,0,0); // inst.numBytes,inst.numBytes ?
					EIPIncrement=0;
				}
			}
			else
			{
				if(IsInRealMode() || 0!=(state.EFLAGS&EFLAGS_VIRTUAL86))
				{
					Interrupt(6,mem,0,0);
					EIPIncrement=0;
				}
				else
				{
					RaiseException(EXCEPTION_UD,0);
					HandleException(true,mem,inst.numBytes);
					EIPIncrement=0;
					break;
				}
			}
		}
		break;


	case I486_RENUMBER_BT_BTS_BTR_BTC_RM_I8:// 0FBA
		{
			clocksPassed=(OPER_ADDR==op1.operandType ? 8 : 6);
			auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
			if(true!=state.exception)
			{
				auto bitOffset=inst.EvalUimm8()&0x1F;
				auto bit=(1<<bitOffset);
				auto src=value1.GetAsDword();
				SetCF(0!=(src&bit));
				switch(inst.GetREG())
				{
				case 4: // BT (Bit Test)
					break;
				case 5: // BTS (Bit Test and Set)
					if(0==(src&bit))
					{
						src|=bit;
						value1.SetDword(src);
						StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
					}
					break;
				case 6: // BTR (Bit Test and Reset)
					if(0!=(src&bit))
					{
						src&=(~bit);
						value1.SetDword(src);
						StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
					}
					break;
				case 7: // BTC (Bit Test and Complement)
					{
						src^=bit;
						value1.SetDword(src);
						StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
					}
					break;
				}
			}
		}
		break;


	case I486_RENUMBER_BSF_R_RM://   0x0FBC,
	case I486_RENUMBER_BSR_R_RM://   0x0FBD,
		{
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8);
			auto src=value.GetAsDword();
			clocksPassed=(I486_OPCODE_BSF_R_RM==inst.opCode ? 6 : 7);
			if(0==src)
			{
				SetZF(true);
			}
			else
			{
				unsigned int bit;
				unsigned int count;
				if(I486_OPCODE_BSF_R_RM==inst.opCode)
				{
					bit=1;
					for(count=0; count<inst.operandSize; ++count)
					{
						if(0!=(src&bit))
						{
							break;
						}
						bit<<=1;
					}
					clocksPassed+=count; // On actual CPU 6-42  ?? Why is it not 6+count ?? [1] pp. 26-31
				}
				else // if(I486_OPCODE_BSR_R_RM==inst.opCode)
				{
					bit=(1<<(inst.operandSize-1));
					for(count=inst.operandSize-1; 0!=bit; --count)
					{
						if(0!=(src&bit))
						{
							break;
						}
						bit>>=1;
					}
					clocksPassed+=count*2; // On actual CPU 6-103 clocks.
				}
				if(OPER_ADDR==op2.operandType)
				{
					++clocksPassed;
				}
				if(count<inst.operandSize)
				{
					value.SetDword(count);
					SetZF(false);
				}
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			}
		}
		break;

	case I486_RENUMBER_BT_R_RM://    0x0FA3,
	case I486_RENUMBER_BTC_RM_R://   0x0FBB,
	case I486_RENUMBER_BTR_RM_R://   0x0FB3,
	case I486_RENUMBER_BTS_RM_R://   0x0FAB,
		if(OPER_ADDR!=op1.operandType)
		{
			clocksPassed=6;
			auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
			auto value2=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8);
			if(true!=state.exception)
			{
				auto bitOffset=value2.GetAsByte()&0x1F;
				auto bit=(1<<bitOffset);
				auto src=value1.GetAsDword();
				SetCF(0!=(src&bit));
				// Nothing more to do for I486_OPCODE_BT_R_RM
				if(I486_OPCODE_BTS_RM_R==inst.opCode)
				{
					if(0==(src&bit))
					{
						src|=bit;
						value1.SetDword(src);
						StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
					}
				}
				else if(I486_OPCODE_BTC_RM_R==inst.opCode)
				{
					src^=bit;
					value1.SetDword(src);
					StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
				}
				else if(I486_OPCODE_BTR_RM_R==inst.opCode)
				{
					if(0!=(src&bit))
					{
						src&=(~bit);
						value1.SetDword(src);
						StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
					}
				}
			}
		}
		else // if(OPER_ADDR==op1.operandType)
		{
			clocksPassed=13;
			auto value2=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8);
			unsigned bitCount=value2.GetAsDword();

			unsigned int offset;
			auto &seg=*ExtractSegmentAndOffset(offset,op1,inst.segOverride);
			offset+=(bitCount>>3);

			unsigned int src=FetchByte(inst.addressSize,seg,offset,mem);
			if(true!=state.exception)
			{
				auto bitOffset=bitCount&7;
				auto bit=(1<<bitOffset);
				SetCF(0!=(src&bit));
				// Nothing more to do for I486_OPCODE_BT_R_RM
				if(I486_OPCODE_BTS_RM_R==inst.opCode)
				{
					if(0==(src&bit))
					{
						src|=bit;
						StoreByte(mem,inst.addressSize,seg,offset,src);
					}
				}
				else if(I486_OPCODE_BTC_RM_R==inst.opCode)
				{
					src^=bit;
					StoreByte(mem,inst.addressSize,seg,offset,src);
				}
				else if(I486_OPCODE_BTR_RM_R==inst.opCode)
				{
					if(0!=(src&bit))
					{
						src&=(~bit);
						StoreByte(mem,inst.addressSize,seg,offset,src);
					}
				}
			}
		}
		break;


	case I486_RENUMBER_LAR:
		if(IsInRealMode())
		{
			Interrupt(6,mem,0,0);
			EIPIncrement=0;
		}
		else
		{
			auto value2=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8);
			auto selector=value2.GetAsWord();

			auto RPL=(selector&3);
			auto TI=(0!=(selector&4));

			unsigned char rawDescBuf[8];
			const unsigned char *rawDesc;

			unsigned int DTLinearBaseAddr=0;
			if(0==TI)
			{
				DTLinearBaseAddr=state.GDTR.linearBaseAddr;
			}
			else
			{
				DTLinearBaseAddr=state.LDTR.linearBaseAddr;
			}
			DTLinearBaseAddr+=(selector&0xfff8); // Use upper 13 bits.

			auto memWin=GetConstMemoryWindowFromLinearAddress(DTLinearBaseAddr,mem);
			if(nullptr!=memWin.ptr && (DTLinearBaseAddr&(MemoryAccess::MEMORY_WINDOW_SIZE-1))<=(MemoryAccess::MEMORY_WINDOW_SIZE-8))
			{
				rawDesc=memWin.ptr+(DTLinearBaseAddr&(MemoryAccess::MEMORY_WINDOW_SIZE-1));
			}
			else
			{
				rawDesc=rawDescBuf;
				rawDescBuf[0]=FetchByteByLinearAddress(mem,DTLinearBaseAddr);
				rawDescBuf[1]=FetchByteByLinearAddress(mem,DTLinearBaseAddr+1);
				rawDescBuf[2]=FetchByteByLinearAddress(mem,DTLinearBaseAddr+2);
				rawDescBuf[3]=FetchByteByLinearAddress(mem,DTLinearBaseAddr+3);
				rawDescBuf[4]=FetchByteByLinearAddress(mem,DTLinearBaseAddr+4);
				rawDescBuf[5]=FetchByteByLinearAddress(mem,DTLinearBaseAddr+5);
				rawDescBuf[6]=FetchByteByLinearAddress(mem,DTLinearBaseAddr+6);
				rawDescBuf[7]=FetchByteByLinearAddress(mem,DTLinearBaseAddr+7);
			}


			uint32_t accessRightBytes=cpputil::GetDword(rawDesc+4);
			accessRightBytes&=0x00F0FF00; // As described in INTEL 80386 PROGRAMMER'S REFERENCE MANUAL 1986
			OperandValue value1;
			value1.SetDword(accessRightBytes);
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);


			SetZF(true);
			clocksPassed=16;
		}
		break;


	case I486_RENUMBER_CALL_FAR://   0x9A,
		{
			if(true==IsInRealMode())
			{
				clocksPassed=18;
			}
			else
			{
				clocksPassed=20;
			}
			Push(mem,inst.operandSize,state.CS().value,state.EIP+inst.numBytes);

			if(true==enableCallStack)
			{
				PushCallStack(
				    false,0xffff,0xffff,
				    state.GetCR(0),
				    state.CS().value,state.EIP,inst.numBytes,
				    op1.seg,op1.offset,
				    mem);
			}

			auto descHighword=LoadSegmentRegister(state.CS(),op1.seg,mem);
			auto descType=(descHighword&0x0f00);
			if(descType==(DESC_TYPE_16BIT_CALL_GATE<<8) ||
			   descType==(DESC_TYPE_32BIT_CALL_GATE<<8))
			{
				auto ptr=GetCallGate(op1.seg,mem);
				if(descType==(DESC_TYPE_16BIT_CALL_GATE<<8))
				{
					ptr.OFFSET&=0xFFFF;
				}
				LoadSegmentRegister(state.CS(),ptr.SEG,mem);
				state.EIP=ptr.OFFSET;
				clocksPassed=35;
			}
			else
			{
				state.EIP=op1.offset;
			}
			EIPIncrement=0;
		}
		break;
	case I486_RENUMBER_CALL_REL://   0xE8,
		{
			clocksPassed=3;

			auto offset=inst.EvalSimm16or32(inst.operandSize);
			auto destin=state.EIP+offset+inst.numBytes;
			destin&=operandSizeMask[inst.operandSize>>3];

			Push(mem,inst.operandSize,state.EIP+inst.numBytes);
			if(true==enableCallStack)
			{
				PushCallStack(
				    false,0xffff,0xffff,
				    state.GetCR(0),
				    state.CS().value,state.EIP,inst.numBytes,
				    state.CS().value,destin,
				    mem);
			}

			state.EIP=destin;
			EIPIncrement=0;
		}
		break;
	case I486_RENUMBER_JMP_REL://          0xE9,   // cw or cd
		{
			clocksPassed=3;

			auto offset=inst.EvalSimm16or32(inst.operandSize);
			auto destin=state.EIP+offset+inst.numBytes;
			destin&=operandSizeMask[inst.operandSize>>3];

			state.EIP=destin;
			EIPIncrement=0;
		}
		break;


	case I486_RENUMBER_CBW_CWDE://        0x98,
		clocksPassed=3;
		if(16==inst.operandSize) // Sign Extend AL to AX
		{
			unsigned int AL=GetAL();
			if(0!=(0x80&AL))
			{
				AL|=0xff00;
			}
			SetAX(AL);
		}
		else // Sign Extend AX to EAX
		{
			unsigned int AX=GetAX();
			if(0!=(0x8000&AX))
			{
				AX|=0xffff0000;
			}
			SetEAX(AX);
		}
		break;
	case I486_RENUMBER_CWD_CDQ://         0x99,
		clocksPassed=3;
		if(16==inst.operandSize) // CWD AX->DX:AX
		{
			SetDX(0!=(GetAX()&0x8000) ? 0xFFFF : 0);
		}
		else // CDQ
		{
			SetEDX(0!=(GetEAX()&0x80000000) ? 0xFFFFFFFF : 0);
		}
		break;
	case I486_RENUMBER_CLC:
		state.EFLAGS&=(~EFLAGS_CARRY);
		clocksPassed=2;
		break;
	case I486_RENUMBER_CLD:
		state.EFLAGS&=(~EFLAGS_DIRECTION);
		clocksPassed=2;
		break;
	case I486_RENUMBER_CLI:
		state.EFLAGS&=(~EFLAGS_INT_ENABLE);
		clocksPassed=2;
		break;
	case I486_RENUMBER_CLTS:
		{
			clocksPassed=7;
			auto CR0=state.GetCR(0);
			CR0&=(~CR0_TASK_SWITCHED);
			SetCR(0,CR0);
		}
		break;


	case I486_RENUMBER_CMC://        0xF5,
		SetCF(GetCF()==true ? false : true);
		clocksPassed=2;
		break;


	case I486_RENUMBER_CMPSB://           0xA6,
	case I486_RENUMBER_CMPS://            0xA7,
		{
			auto &seg=SegmentOverrideDefaultDS(inst.segOverride);
			for(int ctr=0;
			    ctr<MAX_REP_BUNDLE_COUNT &&
			    true==REPCheck(clocksPassed,inst.instPrefix,inst.addressSize);
			    ++ctr)
			{
				auto data1=FetchByteWordOrDword(inst.operandSize,inst.addressSize,seg,state.ESI(),mem);
				auto data2=FetchByteWordOrDword(inst.operandSize,inst.addressSize,state.ES(),state.EDI(),mem);
				if(true!=state.exception)
				{
					SubByteWordOrDword(inst.operandSize,data1,data2);
					UpdateESIandEDIAfterStringOp(inst.addressSize,inst.operandSize);
					clocksPassed+=8;
					if(true==REPEorNECheck(clocksPassed,inst.instPrefix,inst.addressSize))
					{
						EIPIncrement=0;
					}
					else
					{
						EIPIncrement=inst.numBytes;
						break;
					}
				}
			}
		}
		break;


	case I486_RENUMBER_DAA://             0x27,
		clocksPassed=2;
		if(true==GetAF() || 9<(GetAL()&0x0F))
		{
			SetAL(GetAL()+6);
			SetAF(true);
		}
		else
		{
			SetAF(false);
		}
		if(0x9F<GetAL() || true==GetCF())
		{
			SetAL(GetAL()+0x60);
			SetCF(true);
		}
		else
		{
			SetCF(false);
		}
		SetZF(0==GetAL());
		SetSF(0!=(GetAL()&0x80));
		SetPF(CheckParity(GetAL()));
		break;

	case I486_RENUMBER_DAS://             0x2F,
		clocksPassed=2;
		if(true==GetAF() || 9<(GetAL()&0x0F))
		{
			SetAL(GetAL()-6);
			SetAF(true);
		}
		else
		{
			SetAF(false);
		}
		if(0x9F<GetAL() || true==GetCF())
		{
			SetAL(GetAL()-0x60);
			SetCF(true);
		}
		else
		{
			SetCF(false);
		}
		SetZF(0==GetAL());
		SetSF(0!=(GetAL()&0x80));
		SetPF(CheckParity(GetAL()));
		break;

	case I486_RENUMBER_ENTER://      0xC8,
		{
			// Weird operand.
			unsigned int frameSize=inst.operand[0]|(((unsigned int)inst.operand[1])<<8);
			unsigned int level=inst.operand[2]&0x1F;

			clocksPassed=14+level*3;

			Push(mem,inst.operandSize,state.EBP());
			auto framePtr=state.ESP();
			if(0<level)
			{
				// Rewritten based on the psudo-code in https://www.scs.stanford.edu/05au-cs240c/lab/i386/ENTER.htm
				while(0<level)
				{
					if(16==inst.operandSize)
					{
						SetBP(GetBP()-2);
						Push(mem,inst.operandSize,state.BP());
					}
					else
					{
						SetEBP(GetEBP()-4);
						Push(mem,inst.operandSize,state.EBP());
					}
					--level;
				}
				Push(mem,inst.operandSize,framePtr);  // Should it be operandSize or addressSize?  Extremely confusing!
			}
			if(16==inst.operandSize)
			{
				SetBP(framePtr&0xFFFF);
			}
			else
			{
				SetEBP(framePtr);
			}
			if(16==GetStackAddressingSize())
			{
				SetSP(GetSP()-frameSize);
			}
			else
			{
				SetESP(GetESP()-frameSize);
			}
		}
		break;


	case I486_RENUMBER_FWAIT://      0x9B,
		if(true==state.fpuState.ExceptionPending())
		{
			EIPIncrement=0;
		}
		clocksPassed=3;
		break;

	case I486_RENUMBER_FPU_D8_FADD: // 0xD8
		#ifdef BREAK_ON_FPU_INST
			if(nullptr!=debuggerPtr)
			{
				debuggerPtr->ExternalBreak("FPU Inst");
			}
		#endif
		{
			auto MODR_M=inst.operand[0];
			if(0xC0<=MODR_M && MODR_M<=0xC7)
			{
				clocksPassed=state.fpuState.FADD_ST_STi(*this,MODR_M&7);
			}
			else if(0xD1==MODR_M)   // FCOM
			{
			}
			else if(0xD9==MODR_M)   // FCOMP
			{
			}
			else
			{
				switch(inst.GetREG())
				{
				case 3:
					// FCOMP(m32real)
					{
						auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,4);
						clocksPassed=state.fpuState.FCOMP_m32real(*this,value.byteData);
					}
					break;
				}
			}
		}
		break;

	case I486_RENUMBER_FPU_D9_FNSTCW_M16_FNSTENV_F2XM1_FXAM_FXCH_FXTRACT_FYL2X_FYL2XP1_FABS_:// 0xD9,
		#ifdef BREAK_ON_FPU_INST
			if(nullptr!=debuggerPtr)
			{
				debuggerPtr->ExternalBreak("FPU Inst");
			}
		#endif

		if(0xF0<=inst.operand[0] && inst.operand[0]<=0xFF)
		{
		}
		else if(0xC0<=inst.operand[0] && inst.operand[0]<=0xC7)
		{
			clocksPassed=state.fpuState.FLD_ST(*this,inst.operand[0]-0xC0);
		}
		else if(0xE0==inst.operand[0])
		{
			clocksPassed=state.fpuState.FCHS(*this);
		}
		else if(0xE5==inst.operand[0])
		{
			clocksPassed=state.fpuState.FXAM(*this);
		}
		else if(0xE8==inst.operand[0])
		{
			clocksPassed=state.fpuState.FLD1(*this);
		}
		else if(0xE9==inst.operand[0])
		{
			clocksPassed=state.fpuState.FLDL2T(*this);
		}
		else if(0xEE==inst.operand[0])
		{
			clocksPassed=state.fpuState.FLDZ(*this);
		}
		else
		{
			switch(inst.GetREG())
			{
			case 0: // "FLD m32real"
				{
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,4);
					clocksPassed=state.fpuState.FLD32(*this,value.byteData);
				}
				break;
			case 5: // "FLDCW"
				{
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
					auto cw=value.GetAsWord();
					clocksPassed=state.fpuState.FLDCW(*this,cw);
				}
				break;
			case 7: // "FNSTCW"
				{
					OperandValue value;
					value.MakeWord(state.fpuState.GetControlWord());
					StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
					clocksPassed=3;
				}
				break;
			}
		}
		break;
	case I486_RENUMBER_FPU_DB_FNINIT_FRSTOR://     0xDB, 
		#ifdef BREAK_ON_FPU_INST
			if(nullptr!=debuggerPtr)
			{
				debuggerPtr->ExternalBreak("FPU Inst");
			}
		#endif

		if(0xE3==inst.operand[0])
		{
			state.fpuState.FNINIT();
			clocksPassed=17;
		}
		else
		{
			switch(Instruction::GetREG(inst.operand[0]))
			{
			case 3: // FISTP m32int
			case 0:
			case 1:
			case 2:
			case 4:
			case 6:
			default:
				break;
			case 5: // FLD m80real
				{
					auto value=EvaluateOperand80(mem,inst.addressSize,inst.segOverride,op1);
					clocksPassed=state.fpuState.FLD80(*this,value.byteData);
				}
				break;
			case 7: // FSTP m80real
				{
					OperandValue value;
					i486DX::FPUState::DoubleTo80Bit(value,state.fpuState.ST(*this).value);
					state.fpuState.Pop();
					StoreOperandValue80(op1,mem,inst.addressSize,inst.segOverride,value);
					clocksPassed=6;
				}
				break;
			}
		}
		break;
	case I486_RENUMBER_FPU_DC_FADD:
		{
		#ifdef BREAK_ON_FPU_INST
			if(nullptr!=debuggerPtr)
			{
				debuggerPtr->ExternalBreak("FPU Inst");
			}
		#endif
			unsigned int MODR_M=inst.operand[0];
			{
				switch(Instruction::GetREG(MODR_M))
				{
				case 0:	// FADD m64real
					{
						auto value=EvaluateOperand64(mem,inst.addressSize,inst.segOverride,op1);
						clocksPassed=state.fpuState.FADD64(*this,value.byteData);
					}
					break;
				case 1: // FMUL m64real
					{
						auto value=EvaluateOperand64(mem,inst.addressSize,inst.segOverride,op1);
						clocksPassed=state.fpuState.FMUL_m64real(*this,value.byteData);
					}
					break;
				case 2: //
					break;
				case 3: // FCOMP m64real
					{
						auto value=EvaluateOperand64(mem,inst.addressSize,inst.segOverride,op1);
						clocksPassed=state.fpuState.FCOMP_m64real(*this,value.byteData);
					}
					break;
				case 4:
					break;
				case 5:
					{
						auto value=EvaluateOperand64(mem,inst.addressSize,inst.segOverride,op1);
						clocksPassed=state.fpuState.FSUBR_m64real(*this,value.byteData);
					}
					break;
				case 6: //
					break;
				case 7: // FDIVR m64real
					{
						auto value=EvaluateOperand64(mem,inst.addressSize,inst.segOverride,op1);
						clocksPassed=state.fpuState.FDIVR_m64real(*this,value.byteData);
					}
					break;
				default:
					break;
				}
			}
		}
		break;
	case I486_RENUMBER_FPU_DD_FLD_FSAVE_FST_FNSTSW_M16_FFREE_FUCOM:
		{
		#ifdef BREAK_ON_FPU_INST
			if(nullptr!=debuggerPtr)
			{
				debuggerPtr->ExternalBreak("FPU Inst");
			}
		#endif
			unsigned int MODR_M=inst.operand[0];
			if(0xD0==(MODR_M&0xF8)) // D0 11010xxx    [1] pp.151  0<=i<=7
			{
			}
			else if(0xD8<=(MODR_M&0xF8) && (MODR_M&0xF8)<=0xDF) // D8 11011xxx
			{
				clocksPassed=state.fpuState.FSTP_STi(*this,(MODR_M&7));
			}
			else if(0xC0==(MODR_M&0xF8)) // C0 11000xxx
			{
			}
			else if(0xE0==(MODR_M&0xF8) || 0xE1==(MODR_M&0xF8) || 0xE8==(MODR_M&0xF8) || 0xE9==(MODR_M&0xF8))
			{
			}
			else
			{
				switch(Instruction::GetREG(MODR_M))
				{
				case 0:	// FLD m64real
					{
						auto value=EvaluateOperand64(mem,inst.addressSize,inst.segOverride,op1);
						clocksPassed=state.fpuState.FLD64(*this,value.byteData);
					}
					break;
				case 2: // FST m64real
					break;
				case 3: // FSTP m64real
					{
						OperandValue value;
						state.fpuState.GetSTAsDouble(*this,value);
						state.fpuState.Pop();
						StoreOperandValue64(op1,mem,inst.addressSize,inst.segOverride,value);
						clocksPassed=8;
					}
					break;
				case 6: // FSAVE m94/108byte
					break;
				case 7: // FNSTSW m2byte
					{
						clocksPassed=3;
						OperandValue value;
						value.MakeWord(state.fpuState.GetStatusWord());
						StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
					}
					break;
				default:
					break;
				}
			}
		}
		break;
	case I486_RENUMBER_FPU_DE:
		#ifdef BREAK_ON_FPU_INST
			if(nullptr!=debuggerPtr)
			{
				debuggerPtr->ExternalBreak("FPU Inst");
			}
		#endif
		if(0xC0<=inst.operand[0] && inst.operand[0]<=0xC7)
		{
			clocksPassed=state.fpuState.FADDP_STi_ST(*this,inst.operand[0]&7);
		}
		else if(0xC9==inst.operand[0])
		{
			clocksPassed=state.fpuState.FMUL(*this);
		}
		else if(0xF8<=inst.operand[0] && inst.operand[0]<=0xFF)
		{
			clocksPassed=state.fpuState.FDIVP_STi_ST(*this,inst.operand[0]&7);
		}
		else if(0xF0<=inst.operand[0] && inst.operand[0]<=0xF7)
		{
			clocksPassed=state.fpuState.FDIVRP_STi_ST(*this,inst.operand[0]&7);
		}
		else if(0xD9==inst.operand[0])
		{
			clocksPassed=state.fpuState.FCOMPP(*this);
		}
		break;
	case I486_RENUMBER_FPU_DF_FNSTSW_AX://  0xDF,
		#ifdef BREAK_ON_FPU_INST
			if(nullptr!=debuggerPtr)
			{
				debuggerPtr->ExternalBreak("FPU Inst");
			}
		#endif
		if(0xE0==inst.operand[0])
		{
			SetAX(state.fpuState.GetStatusWord());
			clocksPassed=3;
		}
		else
		{
			switch(Instruction::GetREG(inst.operand[0]))
			{
			case 6: // FBSTP m80dec
				{
					OperandValue value;
					state.fpuState.GetSTAs80BitBCD(*this,value);
					state.fpuState.Pop();
					StoreOperandValue80(op1,mem,inst.addressSize,inst.segOverride,value);
					clocksPassed=175;
				}
				break;
			case 7: // FISTP m64int
				{
					OperandValue value;
					state.fpuState.GetSTAsSignedInt(*this,value);
					state.fpuState.Pop();
					StoreOperandValue64(op1,mem,inst.addressSize,inst.segOverride,value);
					clocksPassed=33;
				}
				break;
			}
		}
		break;


	case I486_RENUMBER_DEC_EAX:
		clocksPassed=1;
		{
			const auto nBytes=(inst.operandSize>>3);
			auto value=state.EAX();
			DecrementWithMask(value,operandSizeMask[nBytes],operandSizeSignBit[nBytes]);
			state.EAX()=((state.EAX()&operandSizeAndPattern[nBytes])|value);
		}
		break;
	case I486_RENUMBER_DEC_ECX:
		clocksPassed=1;
		{
			const auto nBytes=(inst.operandSize>>3);
			auto value=state.ECX();
			DecrementWithMask(value,operandSizeMask[nBytes],operandSizeSignBit[nBytes]);
			state.ECX()=((state.ECX()&operandSizeAndPattern[nBytes])|value);
		}
		break;
	case I486_RENUMBER_DEC_EDX:
		clocksPassed=1;
		{
			const auto nBytes=(inst.operandSize>>3);
			auto value=state.EDX();
			DecrementWithMask(value,operandSizeMask[nBytes],operandSizeSignBit[nBytes]);
			state.EDX()=((state.EDX()&operandSizeAndPattern[nBytes])|value);
		}
		break;
	case I486_RENUMBER_DEC_EBX:
		clocksPassed=1;
		{
			const auto nBytes=(inst.operandSize>>3);
			auto value=state.EBX();
			DecrementWithMask(value,operandSizeMask[nBytes],operandSizeSignBit[nBytes]);
			state.EBX()=((state.EBX()&operandSizeAndPattern[nBytes])|value);
		}
		break;
	case I486_RENUMBER_DEC_ESP:
		clocksPassed=1;
		{
			const auto nBytes=(inst.operandSize>>3);
			auto value=state.ESP();
			DecrementWithMask(value,operandSizeMask[nBytes],operandSizeSignBit[nBytes]);
			state.ESP()=((state.ESP()&operandSizeAndPattern[nBytes])|value);
		}
		break;
	case I486_RENUMBER_DEC_EBP:
		clocksPassed=1;
		{
			const auto nBytes=(inst.operandSize>>3);
			auto value=state.EBP();
			DecrementWithMask(value,operandSizeMask[nBytes],operandSizeSignBit[nBytes]);
			state.EBP()=((state.EBP()&operandSizeAndPattern[nBytes])|value);
		}
		break;
	case I486_RENUMBER_DEC_ESI:
		clocksPassed=1;
		{
			const auto nBytes=(inst.operandSize>>3);
			auto value=state.ESI();
			DecrementWithMask(value,operandSizeMask[nBytes],operandSizeSignBit[nBytes]);
			state.ESI()=((state.ESI()&operandSizeAndPattern[nBytes])|value);
		}
		break;
	case I486_RENUMBER_DEC_EDI:
		clocksPassed=1;
		{
			const auto nBytes=(inst.operandSize>>3);
			auto value=state.EDI();
			DecrementWithMask(value,operandSizeMask[nBytes],operandSizeSignBit[nBytes]);
			state.EDI()=((state.EDI()&operandSizeAndPattern[nBytes])|value);
		}
		break;


	case I486_RENUMBER_INSB://     0x6C,
		{
			auto prefix=REPNEtoREP(inst.instPrefix);
			if(true==REPCheck(clocksPassed,prefix,inst.addressSize))
			{
				clocksPassed+=(IsInRealMode() ? 17 : 10); // Protected Mode 32 if CPL>IOPL
				if(true==TakeIOReadException(GetDX(),1,mem,inst.numBytes))
				{
					EIPIncrement=0;
					break;
				}
				auto ioRead=IOIn8(io,GetDX());
				StoreByte(mem,inst.addressSize,state.ES(),state.EDI(),ioRead);
				UpdateDIorEDIAfterStringOp(inst.addressSize,8);
				if(INST_PREFIX_REP==prefix)
				{
					EIPIncrement=0;
				}
			}
		}
		break;


	case I486_RENUMBER_IN_AL_I8://=        0xE4,
		if(true==IsInRealMode())
		{
			clocksPassed=14;
		}
		else
		{
			clocksPassed=8; // 28 if CPL>IOPL
		}
		if(true==TakeIOReadException(inst.EvalUimm8(),1,mem,inst.numBytes))
		{
			EIPIncrement=0;
			break;
		}
		{
			auto ioRead=IOIn8(io,inst.EvalUimm8());
			if(true!=state.exception)
			{
				SetAL(ioRead);
			}
		}
		break;
	case I486_RENUMBER_IN_A_I8://=         0xE5,
		if(true==IsInRealMode())
		{
			clocksPassed=14;
		}
		else
		{
			clocksPassed=8; // 28 if CPL>IOPL
		}
		if(true==TakeIOReadException(inst.EvalUimm8(),inst.operandSize>>3,mem,inst.numBytes))
		{
			EIPIncrement=0;
			break;
		}
		if(16==inst.operandSize)
		{
			auto ioRead=IOIn16(io,inst.EvalUimm8());
			if(true!=state.exception)
			{
				SetAX(ioRead);
			}
		}
		else
		{
			auto ioRead=IOIn32(io,inst.EvalUimm8());
			if(true!=state.exception)
			{
				SetEAX(ioRead);
			}
		}
		break;
	case I486_RENUMBER_IN_AL_DX://=        0xEC,
		if(true==IsInRealMode())
		{
			clocksPassed=14;
		}
		else
		{
			clocksPassed=8; // 28 if CPL>IOPL
		}
		if(true==TakeIOReadException(GetDX(),1,mem,inst.numBytes))
		{
			EIPIncrement=0;
			break;
		}
		{
			auto ioRead=IOIn8(io,GetDX());
			if(true!=state.exception)
			{
				SetAL(ioRead);
			}
		}
		break;
	case I486_RENUMBER_IN_A_DX://=         0xED,
		if(true==IsInRealMode())
		{
			clocksPassed=14;
		}
		else
		{
			clocksPassed=8; // 28 if CPL>IOPL
		}
		if(true==TakeIOReadException(GetDX(),inst.operandSize>>3,mem,inst.numBytes))
		{
			EIPIncrement=0;
			break;
		}
		if(16==inst.operandSize)
		{
			auto ioRead=IOIn16(io,GetDX());
			if(true!=state.exception)
			{
				SetAX(ioRead);
			}
		}
		else
		{
			auto ioRead=IOIn32(io,GetDX());
			if(true!=state.exception)
			{
				SetEAX(ioRead);
			}
		}
		break;


	case I486_RENUMBER_IMUL_R_RM_I8://0x6B,
	case I486_RENUMBER_IMUL_R_RM_IMM://0x69,
	case I486_RENUMBER_IMUL_R_RM://       0x0FAF,
		{
			// Clocks should be 13-26 for 16-bit operand, 13-42 for 32-bit operand, (I486_OPCODE_IMUL_R_RM)
			// or 13-42 (I486_OPCODE_IMUL_R_RM_IMM).
			// I don't know how it should be calculated.
			// I just make it 20 clocks.
			clocksPassed=20;
			auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
			auto value2=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8);
			if(true!=state.exception)
			{
				long long int result;
				if(I486_OPCODE_IMUL_R_RM_I8==inst.opCode)
				{
					long long int i2=value2.GetAsSignedDword();
					long long int i3=inst.EvalSimm8();
					result=i2*i3;
					value1.SetSignedDword((int)result);
				}
				else if(I486_OPCODE_IMUL_R_RM_IMM==inst.opCode)
				{
					long long int i2=value2.GetAsSignedDword();
					long long int i3=inst.EvalSimm16or32(inst.operandSize);
					result=i2*i3;
					value1.SetSignedDword((int)result);
				}
				else if(I486_OPCODE_IMUL_R_RM==inst.opCode)
				{
					long long int i1=value1.GetAsSignedDword();
					long long int i2=value2.GetAsSignedDword();
					result=i1*i2;
					value1.SetSignedDword((int)result);
				}
				else
				{
					Abort("What IMUL?");
					return 0;
				}
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
				bool setOFCF=false;
				switch(inst.operandSize)
				{
				case 8:
					setOFCF=(result<-128 || 127<result);
					break;
				case 16:
					setOFCF=(result<-32768 || 32767<result);
					break;
				case 32:
					setOFCF=(result<-0x80000000LL || 0x7FFFFFFFLL<result);
					break;
				}
				if(true==setOFCF)
				{
					SetCFOF();
				}
				else
				{
					ClearCFOF();
					//SetCF(false);
					//SetOF(false);
				}
			}
		}
		break;


	case I486_RENUMBER_LEAVE://            0xC9,
		clocksPassed=5;
		if(16==GetStackAddressingSize())
		{
			SetSP(state.BP());
		}
		else
		{
			SetESP(state.EBP());
		}
		if(16==inst.operandSize)
		{
			SetBP(Pop(mem,inst.operandSize));
		}
		else
		{
			SetEBP(Pop(mem,inst.operandSize));
		}
		break;


	case I486_RENUMBER_HLT://        0xF4,
		if(0==(state.EFLAGS&EFLAGS_VIRTUAL86))
		{
			state.halt=true;
		}
		else
		{
			RaiseException(EXCEPTION_GP,0);
		}
		clocksPassed=4;
		break;


	case I486_RENUMBER_INC_DEC_R_M8:
		{
			auto value=EvaluateOperand8(mem,inst.addressSize,inst.segOverride,op1);
			if(true!=state.exception)
			{
				auto i=value.GetAsDword();
				switch(inst.GetREG())
				{
				case 0:
					IncrementByte(i);
					break;
				case 1:
					DecrementByte(i);
					break;
				default:
					Abort("Undefined REG for "+cpputil::Ubtox(inst.opCode));
					return 0;
				}
				value.SetDword(i);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			}
			if(op1.operandType==OPER_ADDR)
			{
				clocksPassed=3;
			}
			else
			{
				clocksPassed=1;
			}
		}
		break;
	case I486_RENUMBER_INC_DEC_CALL_CALLF_JMP_JMPF_PUSH:
		{
			auto REG=inst.GetREG();
			switch(REG)
			{
			case 0:
			case 1:
				{
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
					if(true!=state.exception)
					{
						auto i=value.GetAsDword();
						if(0==REG)
						{
							IncrementWordOrDword(inst.operandSize,i);
						}
						else
						{
							DecrementWordOrDword(inst.operandSize,i);
						}
						value.SetDword(i);
						StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
					}
					if(op1.operandType==OPER_ADDR)
					{
						clocksPassed=3;
					}
					else
					{
						clocksPassed=1;
					}
				}
				break;
			case 2: // CALL Indirect
			case 4: // JMP Indirect
				{
					clocksPassed=5;  // Same for CALL Indirect and JMP Indirect.
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
					if(true!=state.exception)
					{
						if(2==REG) // CALL
						{
							Push(mem,inst.operandSize,state.EIP+inst.numBytes);
							if(true==enableCallStack)
							{
								PushCallStack(
								    false,0xffff,0xffff,
								    state.GetCR(0),
								    state.CS().value,state.EIP,inst.numBytes,
								    state.CS().value,value.GetAsDword(),
								    mem);
							}
						}
						state.EIP=(value.GetAsDword()&operandSizeMask[inst.operandSize>>3]);
						EIPIncrement=0;
					}
				}
				break;
			case 3: // CALLF Indirect
			case 5: // JMPF Indirect
				{
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,(inst.operandSize+16)/8);
					if(true!=state.exception)
					{
						if(3==REG) // Call
						{
							/* What is this?
							   FM TOWNS BIOS uses 
									MOV		AX,0110H
									MOV		FS,AX
									CALL	FAR PTR FS:[0040H]
								for reading from a mouse.  That is a perfect opportunity for the emulator to
								identify the operating system version.  The CPU class fires:
									mouseBIOSInterceptorPtr->Intercept();
								when indirect CALL to 0110:[0040H].
							*/
							if(nullptr!=mouseBIOSInterceptorPtr)
							{
								unsigned int offset;
								auto segPtr=ExtractSegmentAndOffset(offset,op1,inst.segOverride);
								if(0x0110==segPtr->value && 0x0040==offset)
								{
									mouseBIOSInterceptorPtr->InterceptMouseBIOS();
								}
							}

							Push(mem,inst.operandSize,state.CS().value,state.EIP+inst.numBytes);
							if(true==enableCallStack)
							{
								auto destSeg=value.GetFwordSegment();
								auto destEIP=value.GetAsDword();
								if(16==inst.operandSize)
								{
									destEIP&=0xFFFF;
								}
								PushCallStack(
								    false,0xffff,0xffff,
								    state.GetCR(0),
								    state.CS().value,state.EIP,inst.numBytes,
								    destSeg,destEIP,
								    mem);
							}
						}
						SetIPorEIP(inst.operandSize,value.GetAsDword());
						auto descHighword=LoadSegmentRegister(state.CS(),value.GetFwordSegment(),mem);
						auto descType=(descHighword&0x0f00);
						if(descType==(DESC_TYPE_16BIT_CALL_GATE<<8) ||
						   descType==(DESC_TYPE_32BIT_CALL_GATE<<8))
						{
							Abort("CALLF/JMPF to Gate");
						}
						EIPIncrement=0;
					}
					if(3==REG) // CALLF Indirect
					{
						if(true==IsInRealMode())
						{
							clocksPassed=17;
						}
						else
						{
							clocksPassed=20;
						}
					}
					else if(op1.operandType==OPER_ADDR)
					{
						clocksPassed=3;
					}
					else
					{
						clocksPassed=1;
					}
				}
				break;
			case 6: // PUSH
				{
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
					clocksPassed=4;
					if(true!=state.exception)
					{
						Push(mem,inst.operandSize,value.GetAsDword());
					}
				}
				break;
			default:
				Abort("Undefined REG for "+cpputil::Ubtox(inst.opCode));
				return 0;
			}
		}
		break;
	case I486_RENUMBER_INC_EAX://    0x40, // 16/32 depends on OPSIZE_OVERRIDE
		clocksPassed=1;
		{
			const auto nBytes=(inst.operandSize>>3);
			auto value=state.EAX();
			IncrementWithMask(value,operandSizeMask[nBytes],operandSizeSignBit[nBytes]);
			state.EAX()=((state.EAX()&operandSizeAndPattern[nBytes])|value);
		}
		break;
	case I486_RENUMBER_INC_ECX://    0x41, // 16/32 depends on OPSIZE_OVERRIDE
		clocksPassed=1;
		{
			const auto nBytes=(inst.operandSize>>3);
			auto value=state.ECX();
			IncrementWithMask(value,operandSizeMask[nBytes],operandSizeSignBit[nBytes]);
			state.ECX()=((state.ECX()&operandSizeAndPattern[nBytes])|value);
		}
		break;
	case I486_RENUMBER_INC_EDX://    0x42, // 16/32 depends on OPSIZE_OVERRIDE
		clocksPassed=1;
		{
			const auto nBytes=(inst.operandSize>>3);
			auto value=state.EDX();
			IncrementWithMask(value,operandSizeMask[nBytes],operandSizeSignBit[nBytes]);
			state.EDX()=((state.EDX()&operandSizeAndPattern[nBytes])|value);
		}
		break;
	case I486_RENUMBER_INC_EBX://    0x43, // 16/32 depends on OPSIZE_OVERRIDE
		clocksPassed=1;
		{
			const auto nBytes=(inst.operandSize>>3);
			auto value=state.EBX();
			IncrementWithMask(value,operandSizeMask[nBytes],operandSizeSignBit[nBytes]);
			state.EBX()=((state.EBX()&operandSizeAndPattern[nBytes])|value);
		}
		break;
	case I486_RENUMBER_INC_ESP://    0x44, // 16/32 depends on OPSIZE_OVERRIDE
		clocksPassed=1;
		{
			const auto nBytes=(inst.operandSize>>3);
			auto value=state.ESP();
			IncrementWithMask(value,operandSizeMask[nBytes],operandSizeSignBit[nBytes]);
			state.ESP()=((state.ESP()&operandSizeAndPattern[nBytes])|value);
		}
		break;
	case I486_RENUMBER_INC_EBP://    0x45, // 16/32 depends on OPSIZE_OVERRIDE
		clocksPassed=1;
		{
			const auto nBytes=(inst.operandSize>>3);
			auto value=state.EBP();
			IncrementWithMask(value,operandSizeMask[nBytes],operandSizeSignBit[nBytes]);
			state.EBP()=((state.EBP()&operandSizeAndPattern[nBytes])|value);
		}
		break;
	case I486_RENUMBER_INC_ESI://    0x46, // 16/32 depends on OPSIZE_OVERRIDE
		clocksPassed=1;
		{
			const auto nBytes=(inst.operandSize>>3);
			auto value=state.ESI();
			IncrementWithMask(value,operandSizeMask[nBytes],operandSizeSignBit[nBytes]);
			state.ESI()=((state.ESI()&operandSizeAndPattern[nBytes])|value);
		}
		break;
	case I486_RENUMBER_INC_EDI://    0x47, // 16/32 depends on OPSIZE_OVERRIDE
		clocksPassed=1;
		{
			const auto nBytes=(inst.operandSize>>3);
			auto value=state.EDI();
			IncrementWithMask(value,operandSizeMask[nBytes],operandSizeSignBit[nBytes]);
			state.EDI()=((state.EDI()&operandSizeAndPattern[nBytes])|value);
		}
		break;


	case I486_RENUMBER_INT3://       0xCC,
		Interrupt(3,mem,1,1);
		EIPIncrement=0;
		clocksPassed=26;
		break;
	case I486_RENUMBER_INT://        0xCD,
		clocksPassed=(IsInRealMode() ? 30 : 44);
		Interrupt(inst.EvalUimm8(),mem,2,2);
		EIPIncrement=0;
		break;
	case I486_RENUMBER_INTO://       0xCE,
		if(GetOF())
		{
			Interrupt(INT_INTO_OVERFLOW, mem, 1, 1);
			EIPIncrement=0;
			clocksPassed=(IsInRealMode() ? 28 : 46);
		}
		else
		{
			clocksPassed=3;
		}
		break;


	case I486_RENUMBER_JMP_REL8://         0xEB,   // cb
		{
			auto offset=inst.EvalSimm8();
			state.EIP=((state.EIP+offset+inst.numBytes)&operandSizeMask[inst.operandSize>>3]);
			clocksPassed=3;
			EIPIncrement=0;
		}
		break;
	case I486_RENUMBER_JO_REL8:   // 0x70,
		CONDITIONALJUMP8(CondJO());
		break;
	case I486_RENUMBER_JNO_REL8:  // 0x71,
		CONDITIONALJUMP8(CondJNO());
		break;
	case I486_RENUMBER_JB_REL8:   // 0x72,
		CONDITIONALJUMP8(CondJB());
		break;
	case I486_RENUMBER_JAE_REL8:  // 0x73,
		CONDITIONALJUMP8(CondJAE());
		break;
	case I486_RENUMBER_JE_REL8:   // 0x74,
		CONDITIONALJUMP8(CondJE());
		break;
	case I486_RENUMBER_JECXZ_REL8:// 0xE3,  // Depending on the operand size
		{
			if(16==inst.operandSize)
			{
				CONDITIONALJUMP8(GetCX()==0);
			}
			else
			{
				CONDITIONALJUMP8(GetECX()==0);
			}
		}
		break;
	case I486_RENUMBER_JNE_REL8:  // 0x75,
		CONDITIONALJUMP8(CondJNE());
		break;
	case I486_RENUMBER_JBE_REL8:  // 0x76,
		CONDITIONALJUMP8(CondJBE());
		break;
	case I486_RENUMBER_JA_REL8:   // 0x77,
		CONDITIONALJUMP8(CondJA());
		break;
	case I486_RENUMBER_JS_REL8:   // 0x78,
		CONDITIONALJUMP8(CondJS());
		break;
	case I486_RENUMBER_JNS_REL8:  // 0x79,
		CONDITIONALJUMP8(CondJNS());
		break;
	case I486_RENUMBER_JP_REL8:   // 0x7A,
		CONDITIONALJUMP8(CondJP());
		break;
	case I486_RENUMBER_JNP_REL8:  // 0x7B,
		CONDITIONALJUMP8(CondJNP());
		break;
	case I486_RENUMBER_JL_REL8:   // 0x7C,
		CONDITIONALJUMP8(CondJL());
		break;
	case I486_RENUMBER_JGE_REL8:  // 0x7D,
		CONDITIONALJUMP8(CondJGE());
		break;
	case I486_RENUMBER_JLE_REL8:  // 0x7E,
		CONDITIONALJUMP8(CondJLE());
		break;
	case I486_RENUMBER_JG_REL8:   // 0x7F,
		CONDITIONALJUMP8(CondJG());
		break;
	case I486_RENUMBER_LOOP://             0xE2,
		{
			auto nBytes=(inst.operandSize>>3);
			unsigned int ctr=((state.ECX()-1)&operandSizeMask[nBytes]);
			state.ECX()=((state.ECX()&operandSizeAndPattern[nBytes])|ctr);
			CONDITIONALJUMP8(0!=ctr);
		}
		break;
	case I486_RENUMBER_LOOPE://            0xE1,
		{
			auto nBytes=(inst.operandSize>>3);
			unsigned int ctr=((state.ECX()-1)&operandSizeMask[nBytes]);
			state.ECX()=((state.ECX()&operandSizeAndPattern[nBytes])|ctr);
			CONDITIONALJUMP8(0!=ctr && true==GetZF());
		}
		break;
	case I486_RENUMBER_LOOPNE://           0xE0,
		{
			auto nBytes=(inst.operandSize>>3);
			unsigned int ctr=((state.ECX()-1)&operandSizeMask[nBytes]);
			state.ECX()=((state.ECX()&operandSizeAndPattern[nBytes])|ctr);
			CONDITIONALJUMP8(0!=ctr && true!=GetZF());
		}
		break;


	case I486_RENUMBER_JA_REL://    0x0F87,
		CONDITIONALJUMP16OR32(CondJA());
		break;
	case I486_RENUMBER_JAE_REL://   0x0F83,
		CONDITIONALJUMP16OR32(CondJAE());
		break;
	case I486_RENUMBER_JB_REL://    0x0F82,
	// case I486_RENUMBER_JC_REL://    0x0F82, Same as JB_REL
		CONDITIONALJUMP16OR32(CondJB());
		break;
	case I486_RENUMBER_JBE_REL://   0x0F86,
		CONDITIONALJUMP16OR32(CondJBE());
		break;
	case I486_RENUMBER_JE_REL://    0x0F84,
	// case I486_RENUMBER_JZ_REL://    0x0F84, Same as JE_REL
		CONDITIONALJUMP16OR32(CondJE());
		break;
	case I486_RENUMBER_JG_REL://    0x0F8F,
		CONDITIONALJUMP16OR32(CondJG());
		break;
	case I486_RENUMBER_JGE_REL://   0x0F8D,
		CONDITIONALJUMP16OR32(CondJGE());
		break;
	case I486_RENUMBER_JL_REL://    0x0F8C,
		CONDITIONALJUMP16OR32(CondJL());
		break;
	case I486_RENUMBER_JLE_REL://   0x0F8E,
		CONDITIONALJUMP16OR32(CondJLE());
		break;
	// case I486_RENUMBER_JNA_REL://   0x0F86, Same as JBE_REL
	// case I486_RENUMBER_JNAE_REL://  0x0F82, Same as JB_REL
	// case I486_RENUMBER_JNB_REL://   0x0F83, Same as JAE_REL
	// case I486_RENUMBER_JNBE_REL://  0x0F87, Same as JA_REL
	// case I486_RENUMBER_JNC_REL://   0x0F83, Same as JAE_REL
	case I486_RENUMBER_JNE_REL://   0x0F85,
		CONDITIONALJUMP16OR32(CondJNE());
		break;
	// case I486_RENUMBER_JNG_REL://   0x0F8E, Same as JLE_REL
	// case I486_RENUMBER_JNGE_REL://  0x0F8C, Same as JL_REL
	// case I486_RENUMBER_JNL_REL://   0x0F8D, Same as JGE_REL
	// case I486_RENUMBER_JNLE_REL://  0x0F8F, Same as JG_REL
	case I486_RENUMBER_JNO_REL://   0x0F81,
		CONDITIONALJUMP16OR32(CondJNO());
		break;
	case I486_RENUMBER_JNP_REL://   0x0F8B,
		CONDITIONALJUMP16OR32(CondJNP());
		break;
	case I486_RENUMBER_JNS_REL://   0x0F89,
		CONDITIONALJUMP16OR32(CondJNS());
		break;
	// case I486_RENUMBER_JNZ_REL://   0x0F85, Same as JNE_REL
	case I486_RENUMBER_JO_REL://    0x0F80,
		CONDITIONALJUMP16OR32(CondJO());
		break;
	case I486_RENUMBER_JP_REL://    0x0F8A,
		CONDITIONALJUMP16OR32(CondJP());
		break;
	// case I486_RENUMBER_JPE_REL://   0x0F8A, Same as JP_REL
	// case I486_RENUMBER_JPO_REL://   0x0F8B, Same as JNP_REL
	case I486_RENUMBER_JS_REL://    0x0F88,
		CONDITIONALJUMP16OR32(CondJS());
		break;


	case I486_RENUMBER_JMP_FAR:
		{
			switch(inst.operandSize)
			{
			case 16:
				if(true==IsInRealMode())
				{
					clocksPassed=17;
				}
				else
				{
					clocksPassed=19;
				}
				break;
			case 32:
				if(true==IsInRealMode())
				{
					clocksPassed=13;
				}
				else
				{
					clocksPassed=18;
				}
				break;
			}
			auto descHighword=LoadSegmentRegister(state.CS(),op1.seg,mem);
			auto descType=(descHighword&0x0f00);
			if(descType==(DESC_TYPE_16BIT_CALL_GATE<<8) ||
			   descType==(DESC_TYPE_32BIT_CALL_GATE<<8))
			{
				Abort("JMPF to Gate");
			}
			state.EIP=op1.offset;
			EIPIncrement=0;
		}
		break;


	case I486_RENUMBER_BINARYOP_RM8_FROM_I8://=  0x80, // AND(REG=4), OR(REG=1), or XOR(REG=6) depends on the REG field of MODR/M
	case I486_RENUMBER_BINARYOP_RM8_FROM_I8_ALIAS:
		{
			if(op1.operandType==OPER_ADDR || op2.operandType==OPER_ADDR)
			{
				clocksPassed=3;
			}
			else
			{
				clocksPassed=1;
			}

			auto value1=EvaluateOperand8(mem,inst.addressSize,inst.segOverride,op1);
			auto value2=inst.EvalUimm8();
			if(true==state.exception)
			{
				break;
			}

			auto i=value1.GetAsDword();
			auto REG=inst.GetREG();
			switch(REG)
			{
			case 0:
				AddByte(i,value2);
				break;
			case 1:
				OrByte(i,value2);
				break;
			case 2:
				AdcByte(i,value2);
				break;
			case 3:
				SbbByte(i,value2);
				break;
			case 4:
				AndByte(i,value2);
				break;
			case 5:
				SubByte(i,value2);
				break;
			case 6:
				XorByte(i,value2);
				break;
			case 7: // CMP
				SubByte(i,value2);
				break;
			default:
				Abort("Undefined REG for "+cpputil::Ubtox(inst.opCode));
				return 0;
			}
			if(7!=REG) // Don't store a value if it is CMP
			{
				value1.SetDword(i);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
			}
		}
		break;

	case I486_RENUMBER_BINARYOP_R_FROM_I://=     0x81,
	case I486_RENUMBER_BINARYOP_RM_FROM_SXI8://= 0x83, Sign of op2 is already extended when decoded.
		{
			if(op1.operandType==OPER_ADDR || op2.operandType==OPER_ADDR)
			{
				clocksPassed=3;
			}
			else
			{
				clocksPassed=1;
			}

			auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
			unsigned int value2;
			if(I486_OPCODE_BINARYOP_R_FROM_I==inst.opCode)
			{
				value2=inst.EvalUimm8or16or32(inst.operandSize);
			}
			else
			{
				value2=inst.EvalUimm8();
				if(value2&0x80)
				{
					value2|=0xFFFFFF00;
				}
			}
			if(true==state.exception)
			{
				break;
			}

			auto i=value1.GetAsDword();
			auto REG=inst.GetREG();
			switch(REG)
			{
			case 0:
				AddWordOrDword(inst.operandSize,i,value2);
				break;
			case 1:
				OrWordOrDword(inst.operandSize,i,value2);
				break;
			case 2:
				AdcWordOrDword(inst.operandSize,i,value2);
				break;
			case 3:
				SbbWordOrDword(inst.operandSize,i,value2);
				break;
			case 4:
				AndWordOrDword(inst.operandSize,i,value2);
				break;
			case 5:
				SubWordOrDword(inst.operandSize,i,value2);
				break;
			case 6:
				XorWordOrDword(inst.operandSize,i,value2);
				break;
			case 7:
				SubWordOrDword(inst.operandSize,i,value2);
				break;
			default:
				Abort("Undefined REG for "+cpputil::Ubtox(inst.opCode));
				return 0;
			}
			if(7!=REG) // Don't store a value if it is CMP
			{
				value1.SetDword(i);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
			}
		}
		break;


	case I486_RENUMBER_LAHF://=             0x9F,
		SetAH(state.EFLAGS&0xFF);
		clocksPassed=2;
		break;


	case I486_RENUMBER_LEA://=              0x8D,
		clocksPassed=1;
		if(OPER_ADDR==op2.operandType && 
		  (OPER_REG32==op1.operandType || OPER_REG16==op1.operandType))
		{
			unsigned int offset=
			    state.NULL_and_reg32[op2.baseReg&15]+
			   (state.NULL_and_reg32[op2.indexReg&15]<<op2.indexShift)+
			   op2.offset;
			offset&=operandSizeMask[inst.addressSize>>3];
			OperandValue value;
			value.MakeDword(offset);
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
		}
		else
		{
			RaiseException(EXCEPTION_UD,0);
		}
		break;


	case I486_RENUMBER_LDS://              0xC5,
	case I486_RENUMBER_LSS://              0x0FB2,
	case I486_RENUMBER_LES://              0xC4,
	case I486_RENUMBER_LFS://              0x0FB4,
	case I486_RENUMBER_LGS://              0x0FB5,
		if(OPER_ADDR==op2.operandType)
		{
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,(inst.operandSize+16)/8);
			if(true!=state.exception)
			{
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
				auto seg=value.GetFwordSegment();
				switch(opCodeRenumberTable[inst.opCode])
				{
				case I486_RENUMBER_LDS://              0xC5,
					LoadSegmentRegister(state.DS(),seg,mem);
					break;
				case I486_RENUMBER_LSS://              0x0FB2,
					if(0==seg)
					{
						RaiseException(EXCEPTION_GP,0);
					}
					else
					{
						LoadSegmentRegister(state.SS(),seg,mem);
					}
					break;
				case I486_RENUMBER_LES://              0xC4,
					LoadSegmentRegister(state.ES(),seg,mem);
					break;
				case I486_RENUMBER_LFS://              0x0FB4,
					LoadSegmentRegister(state.FS(),seg,mem);
					break;
				case I486_RENUMBER_LGS://              0x0FB5,
					LoadSegmentRegister(state.GS(),seg,mem);
					break;
				}
			}
			clocksPassed=9;  // It is described as 6/12, but what makes it 6 clocks or 12 clocks is not given.  Quaaaaack!!!!
		}
		else
		{
			RaiseException(EXCEPTION_GP,0);
		}
		break;


	case I486_RENUMBER_LODSB://            0xAC,
		// REP/REPE/REPNE CX or ECX is chosen based on addressSize.
		{
			auto ECX=state.ECX();
			auto prefix=REPNEtoREP(inst.instPrefix);
			if(true==REPCheck(clocksPassed,prefix,inst.addressSize))
			{
				auto &seg=SegmentOverrideDefaultDS(inst.segOverride);
				auto newAL=FetchByte(inst.addressSize,seg,state.ESI(),mem);
				if(true!=state.exception)
				{
					SetAL(newAL);
					UpdateSIorESIAfterStringOp(inst.addressSize,8);
					if(INST_PREFIX_REP==prefix)
					{
						EIPIncrement=0;
					}
				}
				else
				{
					SetECX(ECX);
					HandleException(true,mem,inst.numBytes);
					EIPIncrement=0;
				}
				clocksPassed+=5;
				ECX=state.ECX();
			}
		}
		break;
	case I486_RENUMBER_LODS://             0xAD,
		{
			auto ECX=state.ECX();
			auto prefix=REPNEtoREP(inst.instPrefix);
			if(true==REPCheck(clocksPassed,prefix,inst.addressSize))
			{
				auto &seg=SegmentOverrideDefaultDS(inst.segOverride);
				unsigned int newEAX=FetchWordOrDword(inst.operandSize,inst.addressSize,seg,state.ESI(),mem);
				if(true!=state.exception)
				{
					if(16==inst.operandSize)
					{
						SetAX(FetchWord(inst.addressSize,seg,state.ESI(),mem));
					}
					else
					{
						SetEAX(FetchDword(inst.addressSize,seg,state.ESI(),mem));
					}
					UpdateSIorESIAfterStringOp(inst.addressSize,inst.operandSize);
					if(INST_PREFIX_REP==prefix)
					{
						EIPIncrement=0;
					}
				}
				else
				{
					SetECX(ECX);
					HandleException(true,mem,inst.numBytes);
					EIPIncrement=0;
				}
				clocksPassed+=5;
				ECX=state.ECX();
			}
		}
		break;


	case I486_RENUMBER_LGDT_LIDT_SGDT_SIDT:
		switch(inst.GetREG())
		{
		case 2: // LGDT
		case 3: // LIDT
			clocksPassed=11;
			if(OPER_ADDR==op1.operandType)
			{
				// If operand size==16, take first 3 bytes of linear-base address.
				// Which means it nees 5 bytes minimum.
				auto numBytes=(16==inst.operandSize ? 5 : 6);
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,numBytes);
				switch(inst.GetREG())
				{
				case 2:
					InvalidateDescriptorCache();
					LoadDescriptorTableRegister(state.GDTR,inst.operandSize,value.byteData);
					break;
				case 3:
					LoadDescriptorTableRegister(state.IDTR,inst.operandSize,value.byteData);
					break;
				}
			}
			else
			{
				if(IsInRealMode())
				{
					Interrupt(6,mem,0,0);
					EIPIncrement=0;
				}
				else
				{
					RaiseException(EXCEPTION_UD,0);
					EIPIncrement=0;
				}
			}
			break;
		case 0: // SGDT
		case 1: // SIDT
			clocksPassed=11;
			if(OPER_ADDR==op1.operandType)
			{
				auto numBytes=(16==inst.operandSize ? 5 : 6);;
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,numBytes);
				OperandValue fwordData;
				switch(inst.GetREG())
				{
				case 0:
					fwordData=DescriptorTableToOperandValue(state.GDTR,inst.operandSize);
					break;
				case 1:
					fwordData=DescriptorTableToOperandValue(state.IDTR,inst.operandSize);
					break;
				}
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,fwordData);
			}
			else
			{
				if(IsInRealMode())
				{
					Interrupt(6,mem,0,0);
					EIPIncrement=0;
				}
				else
				{
					RaiseException(EXCEPTION_UD,0);
					EIPIncrement=0;
				}
			}
			break;
		case 4: // SMSW
			clocksPassed=(OPER_ADDR==op1.operandType ? 3 : 2);
			{
				OperandValue value;
				value.MakeWord(GetRegisterValue(REG_CR0));
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			}
			break;
		case 6: // LMSW
			clocksPassed=13;
			{
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,2);
				auto i=value.GetAsDword();
				auto cr0=state.GetCR(0);
				cr0&=(~0xFFFF);
				cr0|=(i&0xFFFF);
				SetCR(0,cr0);
			}
			break;
		default:
			Abort("Undefined REG for "+cpputil::Ubtox(inst.opCode));
			return 0;
		}
		break;


	case I486_RENUMBER_LSL://              0x0F03,
		clocksPassed=10;
		{
			auto selectorValue=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8); // What to do with high 16 bits?
			auto selector=selectorValue.GetAsWord();
			SegmentRegister seg;
			LoadSegmentRegister(seg,selector,mem,false);
			OperandValue limit;
			limit.MakeDword(seg.limit);
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,limit);
			SetZF(true);
		}
		break;


	case I486_RENUMBER_MOV_FROM_SEG: //     0x8C,
		{
			auto seg=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,2);
			seg.byteData[2]=0;
			seg.byteData[3]=0;
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,seg);
		}
		clocksPassed=3;
		break;
	case I486_RENUMBER_MOV_TO_SEG: //       0x8E,
		Move(mem,inst.addressSize,inst.segOverride,op1,op2);
		if(true==IsInRealMode())
		{
			clocksPassed=3;
		}
		else
		{
			clocksPassed=9;
		}
		break;
	case I486_RENUMBER_MOV_M_TO_AL: //      0xA0,
		{
			clocksPassed=1;
			auto &seg=SegmentOverrideDefaultDS(inst.segOverride);
			SetAL(FetchByte(inst.addressSize,seg,inst.EvalUimm32(),mem));
		}
		break;
	case I486_RENUMBER_MOV_M_TO_EAX: //     0xA1, // 16/32 depends on OPSIZE_OVERRIDE
		{
			clocksPassed=1;
			auto &seg=SegmentOverrideDefaultDS(inst.segOverride);
			unsigned int value=FetchWordOrDword(inst.operandSize,inst.addressSize,seg,inst.EvalUimm32(),mem);
			state.EAX()&=operandSizeAndPattern[inst.operandSize>>3];
			state.EAX()|=(value&operandSizeMask[inst.operandSize>>3]);
		}
		break;
	case I486_RENUMBER_MOV_M_FROM_AL: //    0xA2,
		{
			clocksPassed=1;
			auto &seg=SegmentOverrideDefaultDS(inst.segOverride);
			StoreByte(mem,inst.addressSize,seg,inst.EvalUimm32(),GetAL());
		}
		break;
	case I486_RENUMBER_MOV_M_FROM_EAX: //   0xA3, // 16/32 depends on OPSIZE_OVERRIDE
		{
			clocksPassed=1;
			auto &seg=SegmentOverrideDefaultDS(inst.segOverride);
			StoreWordOrDword(mem,inst.operandSize,inst.addressSize,seg,inst.EvalUimm32(),GetEAX());
		}
		break;
	case I486_RENUMBER_MOV_I8_TO_RM8: //    0xC6,
		{
			OperandValue src;
			src.MakeByte(inst.EvalUimm8());
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,src);
			clocksPassed=1;
		}
		break;
	case I486_RENUMBER_MOV_I_TO_RM: //      0xC7, // 16/32 depends on OPSIZE_OVERRIDE
		{
			OperandValue src;
			if(16==inst.operandSize)
			{
				src.MakeWord(inst.EvalUimm16());
			}
			else
			{
				src.MakeDword(inst.EvalUimm32());
			}
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,src);
			clocksPassed=1;
		}
		break;

	case I486_RENUMBER_MOV_FROM_R: //       0x89, // 16/32 depends on OPSIZE_OVERRIDE
		{
			auto regNum=inst.GetREG();
			unsigned int value=(state.reg32()[regNum]&operandSizeMask[inst.operandSize>>3]);
			OperandValue src;
			src.MakeByteWordOrDword(inst.operandSize,value);
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,src);
			if(true==state.exception)
			{
				HandleException(true,mem,inst.numBytes);
				EIPIncrement=0;
			}
			clocksPassed=1;
		}
		break;
	case I486_RENUMBER_MOV_TO_R: //         0x8B, // 16/32 depends on OPSIZE_OVERRIDE
		{
			auto nBytes=(inst.operandSize>>3);
			auto regNum=inst.GetREG(); // Guaranteed to be between 0 and 7
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8);
			if(true!=state.exception)
			{
				state.reg32()[regNum]&=operandSizeAndPattern[nBytes];
				state.reg32()[regNum]|=(unsigned int)(value.GetAsDword());
			}
			else
			{
				HandleException(true,mem,inst.numBytes);
				EIPIncrement=0;
			}
			clocksPassed=1;
		}
		break;

	case I486_RENUMBER_MOV_FROM_R8: //      0x88,
		{
			auto regNum=inst.GetREG(); // Guaranteed to be between 0 and 7
			unsigned int value=(255&(state.reg32()[regNum&3]>>reg8Shift[regNum]));
			OperandValue src;
			src.MakeByte(value);
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,src);
			clocksPassed=1;
		}
		break;
	case I486_RENUMBER_MOV_TO_R8: //        0x8A,
		{
			auto regNum=inst.GetREG(); // Guaranteed to be between 0 and 7
			auto value=EvaluateOperand8(mem,inst.addressSize,inst.segOverride,op2);
			if(true!=state.exception)
			{
				state.reg32()[regNum&3]&=reg8AndPattern[regNum];
				state.reg32()[regNum&3]|=((unsigned int)(value.GetAsByte())<<reg8Shift[regNum]);
			}
			else
			{
				HandleException(true,mem,inst.numBytes);
				EIPIncrement=0;
			}
			clocksPassed=1;
		}
		break;

	case I486_RENUMBER_MOV_I8_TO_AL: //     0xB0,
	case I486_RENUMBER_MOV_I8_TO_CL: //     0xB1,
	case I486_RENUMBER_MOV_I8_TO_DL: //     0xB2,
	case I486_RENUMBER_MOV_I8_TO_BL: //     0xB3,
		{
			auto regNum=inst.opCode&3;
			auto imm=inst.EvalUimm8();
			state.reg32()[regNum]=(state.reg32()[regNum]&0xFFFFFF00)|imm;
			clocksPassed=1;
		}
		break;
	case I486_RENUMBER_MOV_I8_TO_AH: //     0xB4,
	case I486_RENUMBER_MOV_I8_TO_CH: //     0xB5,
	case I486_RENUMBER_MOV_I8_TO_DH: //     0xB6,
	case I486_RENUMBER_MOV_I8_TO_BH: //     0xB7,
		{
			auto regNum=inst.opCode&3;
			auto imm=inst.EvalUimm8();
			state.reg32()[regNum]=(state.reg32()[regNum]&0xFFFF00FF)|(imm<<8);
			clocksPassed=1;
		}
		break;

	case I486_RENUMBER_MOV_I_TO_EAX: //     0xB8, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_RENUMBER_MOV_I_TO_ECX: //     0xB9, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_RENUMBER_MOV_I_TO_EDX: //     0xBA, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_RENUMBER_MOV_I_TO_EBX: //     0xBB, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_RENUMBER_MOV_I_TO_ESP: //     0xBC, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_RENUMBER_MOV_I_TO_EBP: //     0xBD, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_RENUMBER_MOV_I_TO_ESI: //     0xBE, // 16/32 depends on OPSIZE_OVERRIDE
	case I486_RENUMBER_MOV_I_TO_EDI: //     0xBF, // 16/32 depends on OPSIZE_OVERRIDE
		{
			auto nBytes=(inst.operandSize>>3);
			auto regNum=inst.opCode&7;
			auto imm=inst.EvalUimm8or16or32(inst.operandSize);
			state.reg32()[regNum]=(state.reg32()[regNum]&operandSizeAndPattern[nBytes])|imm;
			clocksPassed=1;
		}
		break;


	case I486_RENUMBER_MOV_TO_CR://        0x0F22,
		if(0==(state.EFLAGS&EFLAGS_VIRTUAL86))
		{
			unsigned int MODR_M=inst.operand[0];
			auto crNum=((MODR_M>>3)&3); // I think it should be &3 not &7.  Only CR0 to CR3.
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,4);
			SetCR(crNum,value.GetAsDword(),mem);
		}
		else
		{
			RaiseException(EXCEPTION_GP,0);
			HandleException(false,mem,inst.numBytes);
			EIPIncrement=0;
		}
		clocksPassed=16;
		break;
	case I486_RENUMBER_MOV_FROM_CR://      0x0F20,
		if(0==(state.EFLAGS&EFLAGS_VIRTUAL86))
		{
			Move(mem,inst.addressSize,inst.segOverride,op1,op2);
		}
		else
		{
			RaiseException(EXCEPTION_GP,0);
			HandleException(false,mem,inst.numBytes);
			EIPIncrement=0;
		}
		clocksPassed=4;
		break;
	case I486_RENUMBER_MOV_FROM_DR://      0x0F21,
		Move(mem,inst.addressSize,inst.segOverride,op1,op2);
		clocksPassed=10;
		break;
	case I486_RENUMBER_MOV_TO_DR://        0x0F23,
		Move(mem,inst.addressSize,inst.segOverride,op1,op2);
		clocksPassed=11;
		break;
	case I486_RENUMBER_MOV_FROM_TR://      0x0F24,
		Move(mem,inst.addressSize,inst.segOverride,op1,op2);
		clocksPassed=4;  // 3 for TR3 strictly speaking.
		break;
	case I486_RENUMBER_MOV_TO_TR://        0x0F26,
		Move(mem,inst.addressSize,inst.segOverride,op1,op2);
		clocksPassed=4;  // 6 for TR6 strictly speaking.
		break;


	case I486_RENUMBER_MOVSB://            0xA4,
		// REP/REPE/REPNE CX or ECX is chosen based on addressSize.
		{
			auto ECX=state.ECX();
			auto prefix=REPNEtoREP(inst.instPrefix);
			auto &seg=SegmentOverrideDefaultDS(inst.segOverride);
			for(int ctr=0;
			    ctr<MAX_REP_BUNDLE_COUNT &&
			    true==REPCheck(clocksPassed,prefix,inst.addressSize);
			    ++ctr)
			{
				auto data=FetchByte(inst.addressSize,seg,state.ESI(),mem);
				StoreByte(mem,inst.addressSize,state.ES(),state.EDI(),data);
				clocksPassed+=7;
				if(true!=state.exception)
				{
					UpdateSIorESIAfterStringOp(inst.addressSize,8);
					UpdateDIorEDIAfterStringOp(inst.addressSize,8);
					if(INST_PREFIX_REP==prefix)
					{
						EIPIncrement=0;
					}
					else
					{
						EIPIncrement=inst.numBytes;
						break;
					}
				}
				else
				{
					SetECX(ECX);
					HandleException(true,mem,inst.numBytes);
					EIPIncrement=0;
					break;
				}
				ECX=state.ECX();
			}
		}
		break;
	case I486_RENUMBER_MOVS://             0xA5,
		{
			auto ECX=state.ECX();
			auto prefix=REPNEtoREP(inst.instPrefix);
			auto &seg=SegmentOverrideDefaultDS(inst.segOverride);
			for(int ctr=0;
			    ctr<MAX_REP_BUNDLE_COUNT &&
			    true==REPCheck(clocksPassed,prefix,inst.addressSize);
			    ++ctr)
			{
				auto data=FetchWordOrDword(inst.operandSize,inst.addressSize,seg,state.ESI(),mem);
				StoreWordOrDword(mem,inst.operandSize,inst.addressSize,state.ES(),state.EDI(),data);
				clocksPassed+=7;
				if(true!=state.exception)
				{
					UpdateESIandEDIAfterStringOp(inst.addressSize,inst.operandSize);
					if(INST_PREFIX_REP==prefix)
					{
						EIPIncrement=0;
					}
					else
					{
						EIPIncrement=inst.numBytes;
						break;
					}
				}
				else
				{
					SetECX(ECX);
					HandleException(true,mem,inst.numBytes);
					EIPIncrement=0;
					break;
				}
				ECX=state.ECX();
			}
		}
		break;


	case I486_RENUMBER_MOVSX_R_RM8://=      0x0FBE,
	case I486_RENUMBER_MOVZX_R_RM8://=      0x0FB6, 8bit to 16or32bit
		{
			clocksPassed=3;
			auto value=EvaluateOperand8(mem,inst.addressSize,inst.segOverride,op2);
			if(true!=state.exception)
			{
				value.numBytes=4;
				if(I486_OPCODE_MOVZX_R_RM8==inst.opCode || 0==(value.byteData[0]&0x80))
				{
					value.byteData[1]=0;
					value.byteData[2]=0;
					value.byteData[3]=0;
				}
				else
				{
					value.byteData[1]=0xff;
					value.byteData[2]=0xff;
					value.byteData[3]=0xff;
				}
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			}
			else
			{
				HandleException(true,mem,inst.numBytes);
				EIPIncrement=0;
			}
		}
		break;
	case I486_RENUMBER_MOVSX_R32_RM16://=   0x0FBF,
	case I486_RENUMBER_MOVZX_R32_RM16://=   0x0FB7, 16bit to 32bit
		{
			clocksPassed=3;
			auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,2);
			if(true!=state.exception)
			{
				value.numBytes=4;
				if(I486_OPCODE_MOVZX_R32_RM16==inst.opCode || 0==(value.byteData[1]&0x80))
				{
					value.byteData[2]=0;
					value.byteData[3]=0;
				}
				else
				{
					value.byteData[2]=0xff;
					value.byteData[3]=0xff;
				}
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			}
			else
			{
				HandleException(true,mem,inst.numBytes);
				EIPIncrement=0;
			}
		}
		break;


	case I486_RENUMBER_NOP://              0x90,
		clocksPassed=1;
		break;


	case I486_RENUMBER_OUT_I8_AL: //        0xE6,
		if(true==IsInRealMode())
		{
			clocksPassed=16;
		}
		else
		{
			clocksPassed=11; // 31 if CPL>IOPL
		}
		if(true==TakeIOWriteException(inst.EvalUimm8(),1,mem,inst.numBytes))
		{
			EIPIncrement=0;
			break;
		}
		IOOut8(io,inst.EvalUimm8(),GetAL());
		break;
	case I486_RENUMBER_OUT_I8_A: //         0xE7,
		if(true==IsInRealMode())
		{
			clocksPassed=16;
		}
		else
		{
			clocksPassed=11; // 31 if CPL>IOPL
		}
		if(true==TakeIOWriteException(inst.EvalUimm8(),inst.operandSize>>3,mem,inst.numBytes))
		{
			EIPIncrement=0;
			break;
		}
		if(16==inst.operandSize)
		{
			IOOut16(io,inst.EvalUimm8(),GetAX());
		}
		else
		{
			IOOut32(io,inst.EvalUimm8(),GetEAX());
		}
		break;
	case I486_RENUMBER_OUT_DX_AL: //        0xEE,
		if(true==IsInRealMode())
		{
			clocksPassed=16;
		}
		else
		{
			clocksPassed=10; // 30 if CPL>IOPL
		}
		if(true==TakeIOWriteException(GetDX(),1,mem,inst.numBytes))
		{
			EIPIncrement=0;
			break;
		}
		IOOut8(io,GetDX(),GetAL());
		break;
	case I486_RENUMBER_OUT_DX_A: //         0xEF,
		if(true==IsInRealMode())
		{
			clocksPassed=16;
		}
		else
		{
			clocksPassed=10; // 30 if CPL>IOPL
		}
		if(true==TakeIOWriteException(GetDX(),inst.operandSize>>3,mem,inst.numBytes))
		{
			EIPIncrement=0;
			break;
		}
		if(16==inst.operandSize)
		{
			IOOut16(io,GetDX(),GetAX());
		}
		else
		{
			IOOut32(io,GetDX(),GetEAX());
		}
		break;


	case I486_RENUMBER_OUTSB://            0x6E,
		// REP/REPE/REPNE CX or ECX is chosen based on addressSize.
		{
			auto prefix=REPNEtoREP(inst.instPrefix);
			if(true==REPCheck(clocksPassed,prefix,inst.addressSize))
			{
				clocksPassed+=(IsInRealMode() ? 17 : 10); // Protected Mode 32 if CPL>IOPL
				if(true==TakeIOWriteException(GetDX(),1,mem,inst.numBytes))
				{
					EIPIncrement=0;
					break;
				}
				auto &seg=SegmentOverrideDefaultDS(inst.segOverride);
				IOOut8(io,GetDX(),FetchByte(inst.addressSize,seg,state.ESI(),mem));
				UpdateSIorESIAfterStringOp(inst.addressSize,8);
				if(INST_PREFIX_REP==prefix)
				{
					EIPIncrement=0;
				}
			}
		}
		break;
	case I486_RENUMBER_OUTS://             0x6F,
		{
			auto prefix=REPNEtoREP(inst.instPrefix);
			if(true==REPCheck(clocksPassed,prefix,inst.addressSize))
			{
				clocksPassed+=(IsInRealMode() ? 17 : 10); // Protected Mode 32 if CPL>IOPL
				if(true==TakeIOWriteException(GetDX(),inst.operandSize>>3,mem,inst.numBytes))
				{
					EIPIncrement=0;
					break;
				}
				auto &seg=SegmentOverrideDefaultDS(inst.segOverride);
				if(16==inst.operandSize)
				{
					IOOut16(io,GetDX(),FetchWord(inst.addressSize,seg,state.ESI(),mem));
				}
				else
				{
					IOOut32(io,GetDX(),FetchDword(inst.addressSize,seg,state.ESI(),mem));
				}
				UpdateSIorESIAfterStringOp(inst.addressSize,inst.operandSize);
				if(INST_PREFIX_REP==prefix)
				{
					EIPIncrement=0;
				}
			}
		}
		break;


	case I486_RENUMBER_PUSHA://            0x60,
		clocksPassed=11;
		{
			auto temp=state.ESP();
			Push(mem,inst.operandSize,state.EAX(),state.ECX(),state.EDX());
			Push(mem,inst.operandSize,state.EBX(),temp       ,state.EBP());
			Push(mem,inst.operandSize,state.ESI(),state.EDI());
		}
		break;
	case I486_RENUMBER_PUSHF://            0x9C,
		clocksPassed=4; // If running as 386 and in protected mode, 3 clocks.
		{
			Push(mem,inst.operandSize,state.EFLAGS);
		}
		break;


	case I486_RENUMBER_PUSH_EAX://         0x50,
	case I486_RENUMBER_PUSH_ECX://         0x51,
	case I486_RENUMBER_PUSH_EDX://         0x52,
	case I486_RENUMBER_PUSH_EBX://         0x53,
	case I486_RENUMBER_PUSH_ESP://         0x54,
	case I486_RENUMBER_PUSH_EBP://         0x55,
	case I486_RENUMBER_PUSH_ESI://         0x56,
	case I486_RENUMBER_PUSH_EDI://         0x57,
		clocksPassed=1;
		Push(mem,inst.operandSize,state.reg32()[(inst.opCode&7)]);
		break;
	case I486_RENUMBER_PUSH_I8://          0x6A,
		clocksPassed=1;
		Push(mem,inst.operandSize,inst.EvalSimm8());
		break;
	case I486_RENUMBER_PUSH_I://           0x68,
		clocksPassed=1;
		Push(mem,inst.operandSize,inst.EvalSimm16or32(inst.operandSize));
		break;
	case I486_RENUMBER_PUSH_CS://          0x0E,
		Push(mem,inst.operandSize,state.CS().value);
		clocksPassed=3;
		break;
	case I486_RENUMBER_PUSH_SS://          0x16,
		Push(mem,inst.operandSize,state.SS().value);
		clocksPassed=3;
		break;
	case I486_RENUMBER_PUSH_DS://          0x1E,
		Push(mem,inst.operandSize,state.DS().value);
		clocksPassed=3;
		break;
	case I486_RENUMBER_PUSH_ES://          0x06,
		Push(mem,inst.operandSize,state.ES().value);
		clocksPassed=3;
		break;
	case I486_RENUMBER_PUSH_FS://          0x0FA0,
		Push(mem,inst.operandSize,state.FS().value);
		clocksPassed=3;
		break;
	case I486_RENUMBER_PUSH_GS://          0x0FA8,
		Push(mem,inst.operandSize,state.GS().value);
		clocksPassed=3;
		break;


	case I486_RENUMBER_POP_M://            0x8F,
		clocksPassed=6;
		{
			OperandValue value;
			value.MakeByteWordOrDword(inst.operandSize,Pop(mem,inst.operandSize));
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
		}
		break;


	case I486_RENUMBER_POP_EAX://          0x58,
		clocksPassed=4;
		{
			auto nBytes=(inst.operandSize>>3);
			auto value=Pop(mem,inst.operandSize);
			state.EAX()&=operandSizeAndPattern[nBytes];
			state.EAX()|=value;
		}
		break;
	case I486_RENUMBER_POP_ECX://          0x59,
		clocksPassed=4;
		{
			auto nBytes=(inst.operandSize>>3);
			auto value=Pop(mem,inst.operandSize);
			state.ECX()&=operandSizeAndPattern[nBytes];
			state.ECX()|=value;
		}
		break;
	case I486_RENUMBER_POP_EDX://          0x5A,
		clocksPassed=4;
		{
			auto nBytes=(inst.operandSize>>3);
			auto value=Pop(mem,inst.operandSize);
			state.EDX()&=operandSizeAndPattern[nBytes];
			state.EDX()|=value;
		}
		break;
	case I486_RENUMBER_POP_EBX://          0x5B,
		clocksPassed=4;
		{
			auto nBytes=(inst.operandSize>>3);
			auto value=Pop(mem,inst.operandSize);
			state.EBX()&=operandSizeAndPattern[nBytes];
			state.EBX()|=value;
		}
		break;
	case I486_RENUMBER_POP_ESP://          0x5C,
		clocksPassed=4;
		{
			auto nBytes=(inst.operandSize>>3);
			auto value=Pop(mem,inst.operandSize);
			state.ESP()&=operandSizeAndPattern[nBytes];
			state.ESP()|=value;
		}
		break;
	case I486_RENUMBER_POP_EBP://          0x5D,
		clocksPassed=4;
		{
			auto nBytes=(inst.operandSize>>3);
			auto value=Pop(mem,inst.operandSize);
			state.EBP()&=operandSizeAndPattern[nBytes];
			state.EBP()|=value;
		}
		break;
	case I486_RENUMBER_POP_ESI://          0x5E,
		clocksPassed=4;
		{
			auto nBytes=(inst.operandSize>>3);
			auto value=Pop(mem,inst.operandSize);
			state.ESI()&=operandSizeAndPattern[nBytes];
			state.ESI()|=value;
		}
		break;
	case I486_RENUMBER_POP_EDI://          0x5F,
		clocksPassed=4;
		{
			auto nBytes=(inst.operandSize>>3);
			auto value=Pop(mem,inst.operandSize);
			state.EDI()&=operandSizeAndPattern[nBytes];
			state.EDI()|=value;
		}
		break;


	case I486_RENUMBER_POP_SS://           0x17,
		clocksPassed=3;
		LoadSegmentRegister(state.SS(),Pop(mem,inst.operandSize),mem);
		break;
	case I486_RENUMBER_POP_DS://           0x1F,
		clocksPassed=3;
		LoadSegmentRegister(state.DS(),Pop(mem,inst.operandSize),mem);
		break;
	case I486_RENUMBER_POP_ES://           0x07,
		clocksPassed=3;
		LoadSegmentRegister(state.ES(),Pop(mem,inst.operandSize),mem);
		break;
	case I486_RENUMBER_POP_FS://           0x0FA1,
		clocksPassed=3;
		LoadSegmentRegister(state.FS(),Pop(mem,inst.operandSize),mem);
		break;
	case I486_RENUMBER_POP_GS://           0x0FA9,
		clocksPassed=3;
		LoadSegmentRegister(state.GS(),Pop(mem,inst.operandSize),mem);
		break;

	case I486_RENUMBER_POPA://             0x61,
		clocksPassed=9;
		if(16==inst.operandSize)
		{
			state.EDI()=((state.EDI()&0xffff0000)|(Pop(mem,inst.operandSize)&0xffff));
			state.ESI()=((state.ESI()&0xffff0000)|(Pop(mem,inst.operandSize)&0xffff));
			state.EBP()=((state.EBP()&0xffff0000)|(Pop(mem,inst.operandSize)&0xffff));
			Pop(mem,inst.operandSize);
			state.EBX()=((state.EBX()&0xffff0000)|(Pop(mem,inst.operandSize)&0xffff));
			state.EDX()=((state.EDX()&0xffff0000)|(Pop(mem,inst.operandSize)&0xffff));
			state.ECX()=((state.ECX()&0xffff0000)|(Pop(mem,inst.operandSize)&0xffff));
			state.EAX()=((state.EAX()&0xffff0000)|(Pop(mem,inst.operandSize)&0xffff));
		}
		else
		{
			state.EDI()=Pop(mem,inst.operandSize);
			state.ESI()=Pop(mem,inst.operandSize);
			state.EBP()=Pop(mem,inst.operandSize);
			Pop(mem,inst.operandSize);
			state.EBX()=Pop(mem,inst.operandSize);
			state.EDX()=Pop(mem,inst.operandSize);
			state.ECX()=Pop(mem,inst.operandSize);
			state.EAX()=Pop(mem,inst.operandSize);
		}
		break;

	case I486_RENUMBER_POPF://             0x9D,
		{
			// VM and RF flags must be preserved.
			unsigned int EFLAGS=Pop(mem,inst.operandSize);
			EFLAGS&=~(EFLAGS_RESUME|EFLAGS_VIRTUAL86);
			EFLAGS|= (state.EFLAGS&(EFLAGS_RESUME|EFLAGS_VIRTUAL86));
			EFLAGS&=EFLAGS_MASK;
			EFLAGS|=EFLAGS_ALWAYS_ON;
			SetFLAGSorEFLAGS(inst.operandSize,EFLAGS);
		}
		clocksPassed=(IsInRealMode() ? 9 : 6);
		break;


	case I486_RENUMBER_RET://              0xC3,
		clocksPassed=5;
		SetIPorEIP(inst.operandSize,Pop(mem,inst.operandSize));
		EIPIncrement=0;
		if(enableCallStack)
		{
			PopCallStack(state.CS().value,state.EIP);
		}
		break;
	case I486_RENUMBER_IRET://   0xCF,
		{
			auto prevVMFlag=state.EFLAGS&EFLAGS_VIRTUAL86;
			if(true==IsInRealMode())
			{
				clocksPassed=15;
			}
			else
			{
				clocksPassed=36;
			}

			bool IRET_TO_VM86=false;

			uint32_t eip,cs,eflags;
			Pop(eip,cs,eflags,mem,inst.operandSize);

			SetIPorEIP(inst.operandSize,eip);
			auto segRegValue=cs;

			SetFLAGSorEFLAGS(inst.operandSize,eflags);
			if(true!=IsInRealMode())
			{
				// if(state.EFLAGS&EFLAGS_NESTED)
				//{
				//	TaskReturn
				//}
				// else
				if(0==prevVMFlag && 0!=(state.EFLAGS&EFLAGS_VIRTUAL86)) // Stack-Return-To-V86
				{
					auto TempESP=Pop(mem,inst.operandSize);
					auto TempSS=Pop(mem,inst.operandSize);
					LoadSegmentRegister(state.ES(),Pop(mem,inst.operandSize),mem);
					LoadSegmentRegister(state.DS(),Pop(mem,inst.operandSize),mem);
					LoadSegmentRegister(state.FS(),Pop(mem,inst.operandSize),mem);
					LoadSegmentRegister(state.GS(),Pop(mem,inst.operandSize),mem);
					state.ESP()&=operandSizeAndPattern[inst.operandSize>>3];
					state.ESP()|=(TempESP&operandSizeMask[inst.operandSize>>3]);
					LoadSegmentRegister(state.SS(),TempSS,mem);
					IRET_TO_VM86=true;
				}

				// The pseudo code in i486 Programmer's Reference Manual suggests that IRET in VM86 mode will cause #GP(0).
				// The textual explanation in i486 Programmer's Reference Manual tells IRET will cause #GP(0) if IOPL<3.
				// i486 Programmer's Reference Manual also says IRETD can be used to enter VM86 mode.  However,
				// Figure 23-2 (pp. 23-5) clearly indicates that IRETD cannot be used to exit VM86 mode.
				// G*d D**n it!  What should I believe?
				// For the time being, I make sure IRETD won't exit VM86 mode.
				if(0!=prevVMFlag)
				{
					state.EFLAGS|=EFLAGS_VIRTUAL86;
				}
			}
			state.EFLAGS&=EFLAGS_MASK;
			state.EFLAGS|=EFLAGS_ALWAYS_ON;

			// IRET to Virtual86 mode requires EFLAGS be loaded before the segment register.
			auto CPL=state.CS().DPL;
			LoadSegmentRegister(state.CS(),segRegValue,mem);
			EIPIncrement=0;
			if(true==enableCallStack)
			{
				PopCallStack(state.CS().value,state.EIP);
			}

			if(state.CS().DPL>CPL && true!=IsInRealMode() && 0==(state.EFLAGS&EFLAGS_VIRTUAL86))
			{
				// IRET to outer level
				auto TempESP=Pop(mem,inst.operandSize);
				auto TempSS=Pop(mem,inst.operandSize);
				LoadSegmentRegister(state.SS(),TempSS,mem);
				state.ESP()=TempESP;
				// Supposed to be zero segment selectors that are not valid in the outer level.
			}
		}
		break;
	case I486_RENUMBER_RETF://             0xCB,
		{
			if(true==IsInRealMode())
			{
				clocksPassed=13;
			}
			else
			{
				clocksPassed=18;
			}

			uint32_t eip,cs;
			Pop(eip,cs,mem,inst.operandSize);

			SetIPorEIP(inst.operandSize,eip);
			LoadSegmentRegister(state.CS(),cs,mem);
			EIPIncrement=0;
			if(enableCallStack)
			{
				PopCallStack(state.CS().value,state.EIP);
			}
		}
		break;
	case I486_RENUMBER_RET_I16://          0xC2,
		clocksPassed=5;
		SetIPorEIP(inst.operandSize,Pop(mem,inst.operandSize));
		state.ESP()+=inst.EvalUimm16(); // Do I need to take &0xffff if address mode is 16? 
		EIPIncrement=0;
		if(enableCallStack)
		{
			PopCallStack(state.CS().value,state.EIP);
		}
		break;
	case I486_RENUMBER_RETF_I16://         0xCA,
		if(true==IsInRealMode())
		{
			clocksPassed=14;
		}
		else
		{
			clocksPassed=17;
		}
		SetIPorEIP(inst.operandSize,Pop(mem,inst.operandSize));
		LoadSegmentRegister(state.CS(),Pop(mem,inst.operandSize),mem);
		state.ESP()+=inst.EvalUimm16(); // Do I need to take &0xffff if address mode is 16? 
		EIPIncrement=0;
		if(enableCallStack)
		{
			PopCallStack(state.CS().value,state.EIP);
		}
		break;


	case I486_RENUMBER_SAHF://=             0x9E,
		state.EFLAGS&=(~0xFF);
		state.EFLAGS|=GetAH();
		clocksPassed=2;
		break;


	case I486_RENUMBER_SHLD_RM_I8://       0x0FA4,
	case I486_RENUMBER_SHLD_RM_CL://       0x0FA5,
	case I486_RENUMBER_SHRD_RM_I8://       0x0FAC,
	case I486_RENUMBER_SHRD_RM_CL://       0x0FAD,
		{
			unsigned int count;
			if(I486_OPCODE_SHLD_RM_CL==inst.opCode ||
			   I486_OPCODE_SHRD_RM_CL==inst.opCode)
			{
				clocksPassed=3;
				count=GetCL()&0x1F;
			}
			else
			{
				clocksPassed=2;
				count=inst.EvalUimm8()&0x1F;
			}
			if(OPER_ADDR==op1.operandType)
			{
				++clocksPassed;
			}
			if(0!=count)
			{
				auto value1=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				auto value2=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op2,inst.operandSize/8);

				unsigned long long int concat;
				switch(opCodeRenumberTable[inst.opCode])
				{
				case I486_RENUMBER_SHLD_RM_I8://       0x0FA4,
				case I486_RENUMBER_SHLD_RM_CL://       0x0FA5,
					if(16==inst.operandSize)
					{
						auto v1=value1.GetAsWord();
						concat=(v1<<16)|value2.GetAsWord();
						concat>>=(16-count);
						value1.MakeWord(concat&0xFFFF);
						StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
						SetCF(0!=(concat&0x10000));
						SetOF((concat&0x8000)!=(v1&0x8000));
						SetZF(0==(concat&0xFFFF));
						SetSF(0!=(concat&0x8000));
						SetPF(CheckParity(concat&0xFF));
					}
					else
					{
						unsigned long long int v1=value1.GetAsDword();
						concat=(v1<<32)|value2.GetAsDword();
						concat>>=(32-count);
						value1.MakeDword(concat&0xFFFFFFFF);
						StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
						SetCF(0!=(concat&0x100000000LL));
						SetOF((concat&0x80000000)!=(v1&0x80000000));
						SetZF(0==(concat&0xFFFFFFFF));
						SetSF(0!=(concat&0x80000000));
						SetPF(CheckParity(concat&0xFF));
					}
					break;
				case I486_RENUMBER_SHRD_RM_I8://       0x0FAC,
				case I486_RENUMBER_SHRD_RM_CL://       0x0FAD,
					if(16==inst.operandSize)
					{
						auto v1=value1.GetAsWord();
						concat=(value2.GetAsWord()<<16)|v1;
						SetCF(0!=count && 0!=(concat&(1LL<<(count-1))));
						concat>>=count;
						value1.MakeWord(concat&0xffff);
						StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
						SetOF((concat&0x8000)!=(v1&0x8000));
						SetZF(0==(concat&0xFFFF));
						SetSF(0!=(concat&0x8000));
						SetPF(CheckParity(concat&0xFF));
					}
					else
					{
						unsigned long long int v1=value1.GetAsDword();
						concat=value2.GetAsDword();
						concat=(concat<<32)|v1;
						SetCF(0!=count && 0!=(concat&(1LL<<(count-1))));
						concat>>=count;
						value1.MakeDword(concat&0xffffffff);
						StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value1);
						SetOF((concat&0x80000000)!=(v1&0x80000000));
						SetZF(0==(concat&0xFFFFFFFF));
						SetSF(0!=(concat&0x80000000));
						SetPF(CheckParity(concat&0xFF));
					}
					break;
				}
			}
		}
		break;


	case I486_RENUMBER_SCASB://            0xAE,
		for(int ctr=0;
		    ctr<MAX_REP_BUNDLE_COUNT &&
		    true==REPCheck(clocksPassed,inst.instPrefix,inst.addressSize);
		    ++ctr)
		{
			auto data=FetchByte(inst.addressSize,state.ES(),state.EDI(),mem);
			auto AL=GetAL();
			SubByte(AL,data);
			UpdateDIorEDIAfterStringOp(inst.addressSize,8);
			clocksPassed+=6;
			if(true==REPEorNECheck(clocksPassed,inst.instPrefix,inst.addressSize))
			{
				EIPIncrement=0;
			}
			else
			{
				EIPIncrement=inst.numBytes;
				break;
			}
		}
		break;
	case I486_RENUMBER_SCAS://             0xAF,
		for(int ctr=0;
		    ctr<MAX_REP_BUNDLE_COUNT &&
		    true==REPCheck(clocksPassed,inst.instPrefix,inst.addressSize);
		    ++ctr)
		{
			auto data=FetchWordOrDword(inst.operandSize,inst.addressSize,state.ES(),state.EDI(),mem);
			auto EAX=GetEAX();
			SubWordOrDword(inst.operandSize,EAX,data);
			UpdateDIorEDIAfterStringOp(inst.addressSize,inst.operandSize);
			clocksPassed+=6;
			if(true==REPEorNECheck(clocksPassed,inst.instPrefix,inst.addressSize))
			{
				EIPIncrement=0;
			}
			else
			{
				EIPIncrement=inst.numBytes;
				break;
			}
		}
		break;


	case I486_RENUMBER_SETA://             0x0F97,
		SETxx(CondJA());
		break;
	case I486_RENUMBER_SETAE://            0x0F93,
		SETxx(CondJAE());
		break;
	case I486_RENUMBER_SETB://             0x0F92,
		SETxx(CondJB());
		break;
	case I486_RENUMBER_SETBE://            0x0F96,
		SETxx(CondJBE());
		break;
	// I486_OPCODE_SETC://             0x0F92,
	case I486_RENUMBER_SETE://             0x0F94,
		SETxx(CondJE());
		break;
	case I486_RENUMBER_SETG://             0x0F9F,
		SETxx(CondJG());
		break;
	case I486_RENUMBER_SETGE://            0x0F9D,
		SETxx(CondJGE());
		break;
	case I486_RENUMBER_SETL://             0x0F9C,
		SETxx(CondJL());
		break;
	case I486_RENUMBER_SETLE://            0x0F9E,
		SETxx(CondJLE());
		break;
	//I486_OPCODE_SETNA://            0x0F96,
	//I486_OPCODE_SETNAE://           0x0F92,
	//I486_OPCODE_SETNB://            0x0F93,
	//I486_OPCODE_SETNBE://           0x0F97,
	//I486_OPCODE_SETNC://            0x0F93,
	case I486_RENUMBER_SETNE://            0x0F95,
		SETxx(CondJNE());
		break;
	//I486_OPCODE_SETNG://            0x0F9E,
	//I486_OPCODE_SETNGE://           0x0F9C,
	//I486_OPCODE_SETNL://            0x0F9D,
	//I486_OPCODE_SETNLE://           0x0F9F,
	case I486_RENUMBER_SETNO://            0x0F91,
		SETxx(CondJNO());
		break;
	case I486_RENUMBER_SETNP://            0x0F9B,
		SETxx(CondJNP());
		break;
	case I486_RENUMBER_SETNS://            0x0F99,
		SETxx(CondJNS());
		break;
	// case I486_RENUMBER_SETNZ://            0x0F95,
	case I486_RENUMBER_SETO://             0x0F90,
		SETxx(CondJO());
		break;
	case I486_RENUMBER_SETP://             0x0F9A,
		SETxx(CondJP());
		break;
	//I486_OPCODE_SETPE://            0x0F9A,
	//I486_OPCODE_SETPO://            0x0F9B,
	case I486_RENUMBER_SETS://             0x0F98,
		SETxx(CondJS());
		break;
	// I486_OPCODE_SETZ://             0x0F94,

	case I486_RENUMBER_SLDT_STR_LLDT_LTR_VERR_VERW://             0x0F00,
		switch(inst.GetREG())
		{
		case 0: // "SLDT"
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 3 : 2);
				OperandValue value;
				value.MakeWord(state.LDTR.selector);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			}
			break;
		case 1: // "STR"
			{
				clocksPassed=(OPER_ADDR==op1.operandType ? 3 : 2);
				OperandValue value;
				value.MakeWord(state.TR.value);
				StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,value);
			}
			break;
		case 2: // "LLDT"
			{
				clocksPassed=11;
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				if(true!=state.exception)
				{
					InvalidateDescriptorCache();
					auto selector=value.GetAsWord();
					auto TI=(0!=(selector&4));
					state.LDTR.selector=selector;
					if(0==selector)
					{
						state.LDTR.linearBaseAddr=0;
						state.LDTR.limit=0;
					}
					else if(0!=TI) // Pointing LDT
					{
						RaiseException(EXCEPTION_GP,selector); // [1] pp.26-199
						HandleException(false,mem,inst.numBytes);
						EIPIncrement=0;
					}
					else
					{
						SegmentRegister seg;
						LoadSegmentRegister(seg,selector,mem,false); // Force to read from GDT by setting isInRealMode=false
						const unsigned char byteData[]=
						{
							(unsigned char)( seg.limit    &0xff),
							(unsigned char)((seg.limit>>8)&0xff),
							(unsigned char)( seg.baseLinearAddr     &0xff),
							(unsigned char)((seg.baseLinearAddr>>8) &0xff),
							(unsigned char)((seg.baseLinearAddr>>16)&0xff),
							(unsigned char)((seg.baseLinearAddr>>24)&0xff),
						};
						LoadDescriptorTableRegister(state.LDTR,32,byteData);
					}
				}
			}
			break;
		case 3: // LTR
			{
				// I need to correct implementation of task behavior to support EMM386.EXE
				auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,inst.operandSize/8);
				LoadTaskRegister(value.GetAsDword(),mem);
				clocksPassed=20;
			}
			break;
		case 4: // "VERR"
			{
				clocksPassed=11;
				if(true==IsInRealMode())
				{
					Interrupt(6,mem,0,0);
					EIPIncrement=0;
				}
				else if(0!=(state.EFLAGS&EFLAGS_VIRTUAL86))
				{
					RaiseException(EXCEPTION_UD,0);
					HandleException(true,mem,inst.numBytes);
					EIPIncrement=0;
				}
				else
				{
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,2);
					SegmentRegister seg;
					auto fourBytes=LoadSegmentRegister(seg,value.GetAsWord(),mem,false);
					auto type=((fourBytes>>8)&0xF);
					if(type<8 || 10==type || 11==type || 14==type || 15==type) // i486 Programmer's Reference Manual 5.2.3 Segment Descriptors Table 5-1 pp.5-12
					{
						SetZF(true); // If readable.
					}
					else
					{
						SetZF(false); // If readable.
					}
				}
			}
			break;
		case 5: // "VERW"
			{
				clocksPassed=11;
				if(true==IsInRealMode())
				{
					Interrupt(6,mem,0,0);
					EIPIncrement=0;
				}
				else if(0!=(state.EFLAGS&EFLAGS_VIRTUAL86))
				{
					RaiseException(EXCEPTION_UD,0);
					HandleException(true,mem,inst.numBytes);
					EIPIncrement=0;
				}
				else
				{
					auto value=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,2);
					SegmentRegister seg;
					auto fourBytes=LoadSegmentRegister(seg,value.GetAsWord(),mem,false);
					auto type=((fourBytes>>8)&0xF);
					if(2==type || 3==type || 6==type || 7==type) // i486 Programmer's Reference Manual 5.2.3 Segment Descriptors Table 5-1 pp.5-12
					{
						SetZF(true); // If writable.
					}
					else
					{
						SetZF(false); // If writable.
					}
				}
			}
			break;
		}
		break;


	case I486_RENUMBER_STC://              0xFB,
		SetCF(true);
		clocksPassed=2;
		break;
	case I486_RENUMBER_STD://              0xFD,
		SetDF(true);
		clocksPassed=2;
		break;
	case I486_RENUMBER_STI://              0xFB,
		SetIF(true);
		// i486 Programmer's Reference Manual says:
		// The processor then responds to the external interrupts after executing the next instruction
		// if the next instruction allows the IF flag to remain enabled.
		// Therefore, STI behaves like substituting a value to SS register, which holds an IRQ for the
		// next instruction.
		state.holdIRQ=true;
		clocksPassed=5;
		break;


	case I486_RENUMBER_STOSB://            0xAA,
		// REP/REPE/REPNE CX or ECX is chosen based on addressSize.
		{
			auto ECX=state.ECX();
			auto prefix=REPNEtoREP(inst.instPrefix);
			for(int ctr=0; 
			    ctr<MAX_REP_BUNDLE_COUNT && 
			    true==REPCheck(clocksPassed,prefix,inst.addressSize);
			    ++ctr)
			{
				StoreByte(mem,inst.addressSize,state.ES(),state.EDI(),GetAL());
				clocksPassed+=5;
				if(true!=state.exception)
				{
					UpdateDIorEDIAfterStringOp(inst.addressSize,8);
					if(INST_PREFIX_REP==prefix)
					{
						EIPIncrement=0;
					}
					else
					{
						EIPIncrement=inst.numBytes;
						break;
					}
				}
				else
				{
					SetECX(ECX);
					HandleException(false,mem,inst.numBytes);
					EIPIncrement=0;
					break;
				}
				ECX=state.ECX();
			}
		}
		break;
	case I486_RENUMBER_STOS://             0xAB,
		// REP/REPE/REPNE CX or ECX is chosen based on addressSize.
		{
			auto ECX=state.ECX();
			auto prefix=REPNEtoREP(inst.instPrefix);
			for(int ctr=0;
			    ctr<MAX_REP_BUNDLE_COUNT &&
			    true==REPCheck(clocksPassed,prefix,inst.addressSize);
			    ++ctr)
			{
				StoreWordOrDword(mem,inst.operandSize,inst.addressSize,state.ES(),state.EDI(),GetEAX());
				clocksPassed+=5;
				if(true!=state.exception)
				{
					UpdateDIorEDIAfterStringOp(inst.addressSize,inst.operandSize);
					if(INST_PREFIX_REP==prefix)
					{
						EIPIncrement=0;
					}
					else
					{
						EIPIncrement=inst.numBytes;
						break;
					}
				}
				else
				{
					SetECX(ECX);
					HandleException(false,mem,inst.numBytes);
					EIPIncrement=0;
					break;
				}
				ECX=state.ECX();
			}
		}
		break;


	case I486_RENUMBER_XCHG_EAX_ECX://     0x91,
	case I486_RENUMBER_XCHG_EAX_EDX://     0x92,
	case I486_RENUMBER_XCHG_EAX_EBX://     0x93,
	case I486_RENUMBER_XCHG_EAX_ESP://     0x94,
	case I486_RENUMBER_XCHG_EAX_EBP://     0x95,
	case I486_RENUMBER_XCHG_EAX_ESI://     0x96,
	case I486_RENUMBER_XCHG_EAX_EDI://     0x97,
		clocksPassed=3;
		if(16==inst.operandSize)
		{
			auto op1=GetAX();
			auto op2=state.reg32()[inst.opCode&7];
			SetAX(op2);
			state.reg32()[inst.opCode&7]=(state.reg32()[inst.opCode&7]&0xffff0000)|(op1&0xffff);
		}
		else
		{
			auto op1=GetEAX();
			auto op2=state.reg32()[inst.opCode&7];
			SetEAX(op2);
			state.reg32()[inst.opCode&7]=op1;
		}
		break;
	case I486_RENUMBER_XCHG_RM8_R8://           0x86,
		clocksPassed=(OPER_ADDR==op1.operandType ? 5 : 3);
		{
			auto RM=EvaluateOperand8(mem,inst.addressSize,inst.segOverride,op1);
			auto R=EvaluateOperand8(mem,inst.addressSize,inst.segOverride,op2);
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,R);
			StoreOperandValue(op2,mem,inst.addressSize,inst.segOverride,RM);
		}
		break;
	case I486_RENUMBER_XCHG_RM_R://             0x87,
		clocksPassed=(OPER_ADDR==op1.operandType ? 5 : 3);
		{
			auto op32or16=inst.operandSize>>3;
			auto RM=EvaluateOperand(mem,inst.addressSize,inst.segOverride,op1,op32or16);
			auto R=state.reg32()[inst.GetREG()]&operandSizeMask[op32or16];

			state.reg32()[inst.GetREG()]&=operandSizeAndPattern[op32or16];
			state.reg32()[inst.GetREG()]|=(RM.GetAsDword()&operandSizeMask[op32or16]);

			RM.SetDword(R);
			StoreOperandValue(op1,mem,inst.addressSize,inst.segOverride,RM);
		}
		break;


	case I486_RENUMBER_XLAT://             0xD7,
		clocksPassed=4;
		{
 			SegmentRegister seg=SegmentOverrideDefaultDS(inst.segOverride);
			unsigned int offset=GetAL();
			if(32==inst.addressSize)
			{
				offset+=GetEBX();
			}
			else
			{
				offset+=GetBX();
			}
			auto nextAL=FetchByte(inst.addressSize,seg,offset,mem);
			if(true!=state.exception)
			{
				SetAL(nextAL);
			}
		}
		break;


	default:
		Abort("Undefined instruction or simply not supported yet.");
		return 0;
	}

	if(0==clocksPassed)
	{
		Abort("Clocks-Passed is not set.");
		return 0;
	}
	state.EIP+=EIPIncrement;

	if(nullptr!=debuggerPtr)
	{
		if(true==state.exception)
		{
			std::string msg="Unhandled exception!  ";
			msg+=ExceptionTypeToStr(state.exceptionType);
			msg+="(0x";
			msg+=cpputil::Uitox(state.exceptionCode);
			msg+=")";
			Abort(msg);
		}
		debuggerPtr->AfterRunOneInstruction(clocksPassed,*this,mem,io,inst);
	}

	return clocksPassed;
}
