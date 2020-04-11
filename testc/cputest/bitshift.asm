						.386p
						ASSUME	CS:CODE

						PUBLIC	C0_BITSHIFT_R8_I8
						PUBLIC	C1_BITSHIFT_R_I8
						PUBLIC	D3_BITSHIFT_R_CL


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
BITSHIFT1_EFLAGS_MASK	EQU		EFLAGS_OF+EFLAGS_CF
BITSHIFTN_EFLAGS_MASK	EQU		EFLAGS_CF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



CODE					SEGMENT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



BITSHIFT				MACRO	INST,VAL,AL_AX_OR_EAX,CTR
						MOV		EAX,VAL
						INST	AL_AX_OR_EAX,CTR
						PUSHFD
						POP		EBX
						AND		EBX,BITSHIFTN_EFLAGS_MASK
						MOV		[EDI],EAX
						MOV		[EDI+4],EBX
						LEA		EDI,[EDI+8]
						ENDM



BITSHIFT_R8_I8_X8		MACRO	INST,VAL
						BITSHIFT	INST,VAL,AL,0

						MOV		EAX,VAL
						INST	AL,1
						PUSHFD
						POP		EBX
						AND		EBX,BITSHIFT1_EFLAGS_MASK
						MOV		[EDI],EAX
						MOV		[EDI+4],EBX
						LEA		EDI,[EDI+8]

						BITSHIFT	INST,VAL,AL,2
						BITSHIFT	INST,VAL,AL,3
						BITSHIFT	INST,VAL,AL,4
						BITSHIFT	INST,VAL,AL,5
						BITSHIFT	INST,VAL,AL,6
						BITSHIFT	INST,VAL,AL,7
						ENDM



BITSHIFT_R32_I8_X8		MACRO	INST,VAL
						BITSHIFT	INST,VAL,EAX,0

						MOV		EAX,VAL
						INST	EAX,1
						PUSHFD
						POP		EBX
						AND		EBX,BITSHIFT1_EFLAGS_MASK
						MOV		[EDI],EAX
						MOV		[EDI+4],EBX
						LEA		EDI,[EDI+8]

						BITSHIFT	INST,VAL,EAX,2
						BITSHIFT	INST,VAL,EAX,7
						BITSHIFT	INST,VAL,EAX,15
						BITSHIFT	INST,VAL,EAX,16
						BITSHIFT	INST,VAL,EAX,30
						BITSHIFT	INST,VAL,EAX,63  ; Should be ANDed with 31
						ENDM



BITSHIFT_R32_CL_X8		MACRO	INST,VAL

						XOR		CL,CL
						BITSHIFT	INST,VAL,EAX,CL

						MOV		CL,1
						MOV		EAX,VAL
						INST	EAX,CL
						PUSHFD
						POP		EBX
						AND		EBX,BITSHIFT1_EFLAGS_MASK
						MOV		[EDI],EAX
						MOV		[EDI+4],EBX
						LEA		EDI,[EDI+8]
						INC		CL

						MOV		CL,2
						BITSHIFT	INST,VAL,EAX,CL
						MOV		CL,6
						BITSHIFT	INST,VAL,EAX,CL
						MOV		CL,16
						BITSHIFT	INST,VAL,EAX,CL
						MOV		CL,17
						BITSHIFT	INST,VAL,EAX,CL
						MOV		CL,31
						BITSHIFT	INST,VAL,EAX,CL
						MOV		CL,63					; Should be ANDed with 31
						BITSHIFT	INST,VAL,EAX,CL
						ENDM



DEBUG_BREAK_TOWNS_VM	MACRO
						MOV		DX,2386H
						MOV		AL,0002H
						OUT		DX,AL
						ENDM



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;extern C0_BITSHIFT_R8_I8(unsigned int res[128],unsigned int v0);
C0_BITSHIFT_R8_I8	PROC
						PUSH	EBP
						MOV		EBP,ESP
						SUB		ESP,4
						PUSHAD

						MOV		EDI,[EBP+8]
						MOV		EDX,[EBP+12]

						BITSHIFT_R8_I8_X8	ROL,EDX
						BITSHIFT_R8_I8_X8	ROR,EDX
						BITSHIFT_R8_I8_X8	RCL,EDX
						BITSHIFT_R8_I8_X8	RCR,EDX
						BITSHIFT_R8_I8_X8	SAL,EDX
						BITSHIFT_R8_I8_X8	SAR,EDX
						BITSHIFT_R8_I8_X8	SHL,EDX
						BITSHIFT_R8_I8_X8	SHR,EDX

						POPAD
						ADD		ESP,4
						POP		EBP
						RET
C0_BITSHIFT_R8_I8	ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;extern C1_BITSHIFT_R8_I88(unsigned int res[16],unsigned int v0);
C1_BITSHIFT_R_I8	PROC
						PUSH	EBP
						MOV		EBP,ESP
						SUB		ESP,4
						PUSHAD

						MOV		EDI,[EBP+8]
						MOV		EDX,[EBP+12]

						BITSHIFT_R32_I8_X8	ROL,EDX
						BITSHIFT_R32_I8_X8	ROR,EDX
						BITSHIFT_R32_I8_X8	RCL,EDX
						BITSHIFT_R32_I8_X8	RCR,EDX
						BITSHIFT_R32_I8_X8	SAL,EDX
						BITSHIFT_R32_I8_X8	SAR,EDX
						BITSHIFT_R32_I8_X8	SHL,EDX
						BITSHIFT_R32_I8_X8	SHR,EDX

						POPAD
						ADD		ESP,4
						POP		EBP
						RET
C1_BITSHIFT_R_I8	ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;extern D3_BITSHIFT_R_CL(unsigned int res[16],unsigned int v0,unsigned int CL);
D3_BITSHIFT_R_CL	PROC
						PUSH	EBP
						MOV		EBP,ESP
						SUB		ESP,4
						PUSHAD

						MOV		EDI,[EBP+8]
						MOV		EDX,[EBP+12]

						BITSHIFT_R32_CL_X8	ROL,EDX
						BITSHIFT_R32_CL_X8	ROR,EDX
						BITSHIFT_R32_CL_X8	RCL,EDX
						BITSHIFT_R32_CL_X8	RCR,EDX
						BITSHIFT_R32_CL_X8	SAL,EDX
						BITSHIFT_R32_CL_X8	SAR,EDX
						BITSHIFT_R32_CL_X8	SHL,EDX
						BITSHIFT_R32_CL_X8	SHR,EDX

						POPAD
						ADD		ESP,4
						POP		EBP
						RET
D3_BITSHIFT_R_CL	ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

CODE					ENDS
						END

