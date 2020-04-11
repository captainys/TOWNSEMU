						.386p
						ASSUME	CS:CODE

						EXPORT	TEST_IMUL_R32_R32
						EXPORT	TEST_IMUL_R32_MUL
						EXPORT	TEST_MUL_R32_R32
						EXPORT	TEST_MUL_R32_MUL


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

EFLAGS_CF				EQU		000000001H
EFLAGS_PF				EQU		000000004H
EFLAGS_AF				EQU		000000010H
EFLAGS_ZF				EQU		000000040H
EFLAGS_SF				EQU		000000080H
EFLAGS_TF				EQU		000000100H
EFLAGS_DF				EQU		000000400H
EFLAGS_OF				EQU		000000800H
EFLAGS_NF				EQU		000004000H
EFLAGS_RF				EQU		000010000H
EFLAGS_VF				EQU		000020000H
MUL_EFLAGS_MASK			EQU		EFLAGS_CF+EFLAGS_OF+EFLAGS_AF+EFLAGS_PF+EFLAGS_SF+EFLAGS_ZF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



CODE					SEGMENT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



C0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_I8	PROC
C0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_I8	ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



C1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_I8	PROC
C1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_I8	ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



D0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_1	PROC
D0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_1	ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



D1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_1		PROC
D1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_1		ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

CODE					ENDS
						END

