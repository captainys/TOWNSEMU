						.386p
						ASSUME	CS:CODE

						PUBLIC	TEST_SHLD_SHRD32
						PUBLIC	TEST_SHLD_SHRD16


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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

CODE					SEGMENT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; void TEST_SHLD_SHRD32(unsigned int res[6],unsigned int EAX,unsigned int EDX,unsigned int ECX);
; void TEST_SHLD_SHRD16(unsigned int res[6],unsigned int EAX,unsigned int EDX,unsigned int ECX);

TEST_SHLD_SHRD32		PROC
						PUSH	EBP				; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EBP+8]=ResultPtr,  [EBP+12]=EAX,  [EBP+16]=EDX,  [EBP+20]=ECX
						MOV		EBP,ESP
						PUSHAD


						MOV		EDI,[EBP+8]


						PUSHFD
						AND		BYTE PTR [ESP],0FFH-(EFLAGS_SF+EFLAGS_ZF+EFLAGS_PF+EFLAGS_CF)
						POPFD

						MOV		EAX,[EBP+12]
						MOV		EDX,[EBP+16]
						MOV		ECX,[EBP+20]
						SHLD	EAX,EDX,CL

						PUSHFD
						POP		EBX
						AND		EBX,EFLAGS_SF+EFLAGS_ZF+EFLAGS_PF+EFLAGS_CF

						MOV		[EDI  ],EAX
						MOV		[EDI+4],EDX
						MOV		[EDI+8],EBX


						PUSHFD
						AND		BYTE PTR [ESP],0FFH-(EFLAGS_SF+EFLAGS_ZF+EFLAGS_PF+EFLAGS_CF)
						POPFD

						MOV		EAX,[EBP+12]
						MOV		EDX,[EBP+16]
						MOV		ECX,[EBP+20]
						SHRD	EAX,EDX,CL

						PUSHFD
						POP		EBX
						AND		EBX,EFLAGS_SF+EFLAGS_ZF+EFLAGS_PF+EFLAGS_CF

						MOV		[EDI+12],EAX
						MOV		[EDI+16],EDX
						MOV		[EDI+20],EBX


						POPAD
						POP		EBP
						RET
TEST_SHLD_SHRD32		ENDP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

TEST_SHLD_SHRD16		PROC
						PUSH	EBP				; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EBP+8]=ResultPtr,  [EBP+12]=EAX,  [EBP+16]=EDX,  [EBP+20]=ECX
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]


						PUSHFD
						AND		BYTE PTR [ESP],0FFH-(EFLAGS_SF+EFLAGS_ZF+EFLAGS_PF+EFLAGS_CF)
						POPFD

						MOV		EAX,[EBP+12]
						MOV		EDX,[EBP+16]
						MOV		ECX,[EBP+20]
						SHLD	AX,DX,CL

						PUSHFD
						POP		EBX
						AND		EBX,EFLAGS_SF+EFLAGS_ZF+EFLAGS_PF+EFLAGS_CF

						MOV		[EDI  ],EAX
						MOV		[EDI+4],EDX
						MOV		[EDI+8],EBX


						PUSHFD
						AND		BYTE PTR [ESP],0FFH-(EFLAGS_SF+EFLAGS_ZF+EFLAGS_PF+EFLAGS_CF)
						POPFD

						MOV		EAX,[EBP+12]
						MOV		EDX,[EBP+16]
						MOV		ECX,[EBP+20]
						SHRD	AX,DX,CL

						PUSHFD
						POP		EBX
						AND		EBX,EFLAGS_SF+EFLAGS_ZF+EFLAGS_PF+EFLAGS_CF

						MOV		[EDI+12],EAX
						MOV		[EDI+16],EDX
						MOV		[EDI+20],EBX


						POPAD
						POP		EBP
						RET
TEST_SHLD_SHRD16		ENDP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

CODE					ENDS
						END

