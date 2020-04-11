						.386p
						ASSUME	CS:CODE

						PUBLIC	TEST_IMUL_R32_R32
						PUBLIC	TEST_IMUL_R32_MEM
						PUBLIC	TEST_MUL_R32_R32
						PUBLIC	TEST_MUL_R32_MEM


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
MUL_EFLAGS_MASK			EQU		EFLAGS_CF+EFLAGS_OF		; i486 Processor Programmers Reference Manual states CF and OF are set.  SF, ZF, AF, and PF are undefined.



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



CODE					SEGMENT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



TEST_IMUL_R32_R32		PROC
						PUSH	EBP				; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr,  [EIP+12]=EAX,  [EIP+16]=EDX
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]
						MOV		EAX,[EBP+12]
						MOV		EDX,[EBP+16]

						IMUL	EDX
						PUSHFD
						POP		ECX
						AND		ECX,MUL_EFLAGS_MASK

						MOV		[EDI  ],EAX
						MOV		[EDI+4],EDX
						MOV		[EDI+8],ECX

						POPAD
						POP		EBP
						RET
TEST_IMUL_R32_R32		ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



TEST_IMUL_R32_MEM		PROC
						PUSH	EBP				; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr,  [EIP+12]=EAX,  [EIP+16]=EDX
						MOV		EBP,ESP
						SUB		ESP,4
						PUSHAD

						MOV		EDI,[EBP+8]
						MOV		EAX,[EBP+12]
						MOV		EDX,[EBP+16]

						MOV		[EBP-4],EDX
						IMUL	DWORD PTR [EBP-4]
						PUSHFD
						POP		ECX
						AND		ECX,MUL_EFLAGS_MASK

						MOV		[EDI  ],EAX
						MOV		[EDI+4],EDX
						MOV		[EDI+8],ECX

						POPAD
						ADD		ESP,4
						POP		EBP
						RET
TEST_IMUL_R32_MEM		ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



TEST_MUL_R32_R32		PROC
						PUSH	EBP				; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr,  [EIP+12]=EAX,  [EIP+16]=EDX
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]
						MOV		EAX,[EBP+12]
						MOV		EDX,[EBP+16]

						MUL		EDX
						PUSHFD
						POP		ECX
						AND		ECX,MUL_EFLAGS_MASK

						MOV		[EDI  ],EAX
						MOV		[EDI+4],EDX
						MOV		[EDI+8],ECX

						POPAD
						POP		EBP
						RET
TEST_MUL_R32_R32		ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



TEST_MUL_R32_MEM		PROC
						PUSH	EBP				; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr,  [EIP+12]=EAX,  [EIP+16]=EDX
						MOV		EBP,ESP
						SUB		ESP,4
						PUSHAD

						MOV		EDI,[EBP+8]
						MOV		EAX,[EBP+12]
						MOV		EDX,[EBP+16]

						MOV		[EBP-4],EDX
						MUL		DWORD PTR [EBP-4]
						PUSHFD
						POP		ECX
						AND		ECX,MUL_EFLAGS_MASK

						MOV		[EDI  ],EAX
						MOV		[EDI+4],EDX
						MOV		[EDI+8],ECX

						POPAD
						ADD		ESP,4
						POP		EBP
						RET
TEST_MUL_R32_MEM		ENDP






CODE					ENDS
						END
