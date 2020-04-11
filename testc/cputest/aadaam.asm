						.386p
						ASSUME	CS:CODE

						PUBLIC	TEST_AAD
						PUBLIC	TEST_AAM
						PUBLIC	TEST_AAS

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



; void TEST_AAD(unsigned int res[],unsigned int eax)
TEST_AAD				PROC
						PUSH	EBP				; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr,  [EIP+12]=EAX,  [EIP+16]=EDX
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]
						MOV		EAX,[EBP+12]

						AND		EAX,0FFFFH
						AAD
						PUSHFD
						POP		EBX
						AND		EBX,EFLAGS_SF+EFLAGS_ZF+EFLAGS_PF

						MOV		[EDI],EAX
						MOV		[EDI+4],EBX

						POPAD
						POP		EBP
						RET
TEST_AAD				ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



; void TEST_AAM(unsigned int res[],unsigned int eax)
TEST_AAM				PROC
						PUSH	EBP				; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr,  [EIP+12]=EAX,  [EIP+16]=EDX
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]
						MOV		EAX,[EBP+12]

						AND		EAX,0FFFFH
						AAM
						PUSHFD
						POP		EBX
						AND		EBX,EFLAGS_SF+EFLAGS_ZF+EFLAGS_PF

						MOV		[EDI],EAX
						MOV		[EDI+4],EBX

						POPAD
						POP		EBP
						RET
TEST_AAM				ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



; void TEST_AAS(unsigned int res[],unsigned int eax)
TEST_AAS				PROC
						PUSH	EBP				; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr,  [EIP+12]=EAX,  [EIP+16]=EDX
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]
						MOV		EAX,[EBP+12]

						AND		EAX,0FFFFH
						AAS
						PUSHFD
						POP		EBX
						AND		EBX,EFLAGS_CF+EFLAGS_AF

						MOV		[EDI],EAX
						MOV		[EDI+4],EBX

						POPAD
						POP		EBP
						RET
TEST_AAS				ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



CODE					ENDS
						END
