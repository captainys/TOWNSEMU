						.386p
						ASSUME	CS:CODE

						PUBLIC	TEST_DAA
						PUBLIC	TEST_DAS

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



; int TEST_DAA(unsigned int eax,unsigned int edx)
TEST_DAA				PROC
						PUSH	EBP				; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=EAX,  [EIP+12]=EDX
						MOV		EBP,ESP
						PUSHAD

						MOV		EAX,[EBP+8]
						MOV		EDX,[EBP+12]

						XOR		AH,AH

						ADD		AL,DL
						DAA

						PUSHFD
						POP		EBX
						AND		BL,EFLAGS_SF+EFLAGS_ZF+EFLAGS_PF+EFLAGS_CF+EFLAGS_AF
						MOV		AH,BL

						AND		EAX,0FFFFH
						MOV		[EBP+8],EAX
						POPAD
						MOV		EAX,[EBP+8]
						POP		EBP
						RET
TEST_DAA				ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



; int TEST_DAS(unsigned int eax,unsigned int edx)
TEST_DAS				PROC
						PUSH	EBP				; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=EAX,  [EIP+12]=EDX
						MOV		EBP,ESP
						PUSHAD

						MOV		EAX,[EBP+8]
						MOV		EDX,[EBP+12]

						XOR		AH,AH

						SUB		AL,DL
						DAS

						PUSHFD
						POP		EBX
						AND		BL,EFLAGS_SF+EFLAGS_ZF+EFLAGS_PF+EFLAGS_CF+EFLAGS_AF
						MOV		AH,BL

						AND		EAX,0FFFFH
						MOV		[EBP+8],EAX
						POPAD
						MOV		EAX,[EBP+8]
						POP		EBP
						RET
TEST_DAS				ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



CODE					ENDS
						END
