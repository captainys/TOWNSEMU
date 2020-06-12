						.386p
						ASSUME	CS:CODE

						PUBLIC	TEST_F6
						PUBLIC	TEST_F7

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



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



CODE					SEGMENT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;	case I486_OPCODE_F6_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV: //=0xF6
;	case I486_OPCODE_F7_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV: //=0xF7,



; void TEST_F6(unsigned int res[],unsigned int eax,unsigned int edx)
TEST_F6					PROC
						PUSH	EBP				; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr,  [EIP+12]=EAX,  [EIP+16]=EDX
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]
						MOV		EAX,[EBP+12]
						MOV		EDX,[EBP+16]



						AND		EAX,0FFH
						AND		EDX,0FFH
						PUSH	EDX
						PUSH	EAX

						NOT		DL
						PUSHFD
						POP		EBX
						AND		EBX,0
						MOV		[EDI],EDX
						MOV		[EDI+4],EBX
						LEA		EDI,[EDI+8]

						MOV		EAX,[ESP]
						MOV		EDX,[ESP+4];

						NEG		DL
						PUSHFD
						POP		EBX
						AND		EBX,EFLAGS_CF+EFLAGS_OF+EFLAGS_SF+EFLAGS_ZF+EFLAGS_PF
						MOV		[EDI],EDX
						MOV		[EDI+4],EBX
						LEA		EDI,[EDI+8]

						MOV		EAX,[ESP]
						MOV		EDX,[ESP+4];

						MUL		DL
						PUSHFD
						POP		EBX
						AND		EBX,EFLAGS_CF+EFLAGS_OF
						MOV		[EDI],EAX
						MOV		[EDI+4],EBX
						LEA		EDI,[EDI+8]

						MOV		EAX,[ESP]
						MOV		EDX,[ESP+4];

						IMUL	DL
						PUSHFD
						POP		EBX
						AND		EBX,EFLAGS_CF+EFLAGS_OF
						MOV		[EDI],EAX
						MOV		[EDI+4],EBX
						LEA		EDI,[EDI+8]

						MOV		EAX,[ESP]
						MOV		EDX,[ESP+4];

						OR		DL,4 ; Force it to be >=4
						AND		AH,3 ; Force it to be <4
						DIV		DL
						PUSHFD
						POP		EBX
						AND		EBX,0
						MOV		[EDI],EAX
						MOV		[EDI+4],EBX
						LEA		EDI,[EDI+8]

						MOV		EAX,[ESP]
						MOV		EDX,[ESP+4];

						OR		DL,4 ; Force it to be >=4
						AND		DL,7FH
						AND		AH,3 ; Force it to be <4
						IDIV	DL
						PUSHFD
						POP		EBX
						AND		EBX,0
						MOV		[EDI],EAX
						MOV		[EDI+4],EBX
						LEA		EDI,[EDI+8]

						POP		EAX
						POP		EDX



						POPAD
						POP		EBP
						RET
TEST_F6					ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



; void TEST_F7(unsigned int res[],unsigned int eax,unsigned int edx)
TEST_F7					PROC
						PUSH	EBP				; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr,  [EIP+12]=EAX,  [EIP+16]=EDX
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]
						MOV		EAX,[EBP+12]
						MOV		EDX,[EBP+16]



						PUSH	EDX
						PUSH	EAX

						NOT		EDX
						PUSHFD
						POP		EBX
						AND		EBX,0
						MOV		[EDI],EDX
						MOV		[EDI+4],EBX
						LEA		EDI,[EDI+8]

						MOV		EAX,[ESP]
						MOV		EDX,[ESP+4];

						NEG		EDX
						PUSHFD
						POP		EBX
						AND		EBX,EFLAGS_CF+EFLAGS_OF+EFLAGS_SF+EFLAGS_ZF+EFLAGS_PF
						MOV		[EDI],EDX
						MOV		[EDI+4],EBX
						LEA		EDI,[EDI+8]

						MOV		EAX,[ESP]
						MOV		EDX,[ESP+4];

						MUL		EDX
						PUSHFD
						POP		EBX
						AND		EBX,EFLAGS_CF+EFLAGS_OF
						MOV		[EDI],EAX
						MOV		[EDI+4],EBX
						LEA		EDI,[EDI+8]

						MOV		EAX,[ESP]
						MOV		EDX,[ESP+4];

						IMUL	EDX
						PUSHFD
						POP		EBX
						AND		EBX,EFLAGS_CF+EFLAGS_OF
						MOV		[EDI],EAX
						MOV		[EDI+4],EBX
						LEA		EDI,[EDI+8]

						MOV		EAX,[ESP]
						MOV		EBX,[ESP+4];
						XOR		EDX,EDX

						OR		EBX,1 ; Force it to be non-zero
						DIV		EBX
						PUSHFD
						POP		EBX
						AND		EBX,0
						MOV		[EDI],EAX
						MOV		[EDI+4],EBX
						LEA		EDI,[EDI+8]

						MOV		EAX,[ESP]
						MOV		EBX,[ESP+4];
						XOR		EDX,EDX
						
						OR		EBX,1 ; Force it to be non-zero, no IDIV overflow for 0x80000000
						OR		EAX,1 ; No -0x80000000 to avoid overflow
						CDQ
						IDIV	EBX
						PUSHFD
						POP		EBX
						AND		EBX,0
						MOV		[EDI],EAX
						MOV		[EDI+4],EBX
						LEA		EDI,[EDI+8]

						POP		EAX
						POP		EDX



						POPAD
						POP		EBP
						RET
TEST_F7					ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



CODE					ENDS
						END
