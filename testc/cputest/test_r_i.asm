						.386p
						ASSUME	CS:CODE

						PUBLIC	TEST_R8_I8
						PUBLIC	TEST_R32_I32

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



EFLAGS_CF   	   		EQU		00001H
EFLAGS_PF   	  		EQU		00004H
EFLAGS_AF  				EQU		00010H
EFLAGS_ZF       		EQU		00040H
EFLAGS_SF       		EQU		00080H
EFLAGS_TRAP       		EQU		00100H
EFLAGS_IF 				EQU		00200H
EFLAGS_DF  				EQU		00400H
EFLAGS_OF  	 			EQU		00800H
EFLAGS_IOPL       		EQU		03000H
EFLAGS_NF   	  		EQU		04000H
EFLAGS_RF   	  		EQU		10000H
EFLAGS_VF		  		EQU		20000H
EFLAGS_ALIGN_CHECK		EQU		40000H
TEST_EFLAGS_MASK		EQU		EFLAGS_OF+EFLAGS_CF+EFLAGS_SF+EFLAGS_ZF+EFLAGS_PF


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



CODE					SEGMENT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



TEST_REG_IMM			MACRO	R,I
						TEST	R,I
						PUSHFD
						POP		EAX
						AND		EAX,TEST_EFLAGS_MASK
						STOSD
						ENDM



; void TEST_R8_I8(unsigned int res[16],unsigned int ebx)
TEST_R8_I8				PROC
						PUSH	EBP				; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr,  [EIP+12]=EBX
						MOV		EBP,ESP
						PUSHAD
						PUSH	ES

						PUSH	DS
						POP		ES

						MOV		EDI,[EBP+8]
						MOV		EBX,[EBP+12]

						TEST_REG_IMM	BL,000H
						TEST_REG_IMM	BL,001H
						TEST_REG_IMM	BL,002H
						TEST_REG_IMM	BL,004H
						TEST_REG_IMM	BL,008H
						TEST_REG_IMM	BL,010H
						TEST_REG_IMM	BL,020H
						TEST_REG_IMM	BL,040H
						TEST_REG_IMM	BL,080H

						TEST_REG_IMM	BL,003H
						TEST_REG_IMM	BL,006H
						TEST_REG_IMM	BL,00CH
						TEST_REG_IMM	BL,018H
						TEST_REG_IMM	BL,030H
						TEST_REG_IMM	BL,060H
						TEST_REG_IMM	BL,0C0H

						POP		ES
						POPAD
						POP		EBP
						RET
TEST_R8_I8				ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



; void TEST_R32_I32(unsigned int res[16],unsigned int ebx)
TEST_R32_I32			PROC
						PUSH	EBP				; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr,  [EIP+12]=EAX,  [EIP+16]=EDX
						MOV		EBP,ESP
						PUSHAD
						PUSH	ES

						PUSH	DS
						POP		ES

						MOV		EDI,[EBP+8]
						MOV		EBX,[EBP+12]

						TEST_REG_IMM	EBX,000000000H
						TEST_REG_IMM	EBX,000000008H
						TEST_REG_IMM	EBX,000000080H
						TEST_REG_IMM	EBX,000000800H
						TEST_REG_IMM	EBX,000008000H
						TEST_REG_IMM	EBX,000080000H
						TEST_REG_IMM	EBX,000800000H
						TEST_REG_IMM	EBX,008000000H
						TEST_REG_IMM	EBX,080000000H

						TEST_REG_IMM	EBX,000000F0FH
						TEST_REG_IMM	EBX,00000F0F6H
						TEST_REG_IMM	EBX,0000F0F0CH
						TEST_REG_IMM	EBX,000F0F018H
						TEST_REG_IMM	EBX,00F0F0030H
						TEST_REG_IMM	EBX,0F0F00060H
						TEST_REG_IMM	EBX,00F0000C0H

						POP		ES
						POPAD
						POP		EBP
						RET
TEST_R32_I32			ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



CODE					ENDS
						END
