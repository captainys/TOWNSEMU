						.386p
						ASSUME	CS:CODE

						PUBLIC	TEST_IMUL_R32_R32
						PUBLIC	TEST_IMUL_R32_MEM
						PUBLIC	TEST_MUL_R32_R32
						PUBLIC	TEST_MUL_R32_MEM


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
