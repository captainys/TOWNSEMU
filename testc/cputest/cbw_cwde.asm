
						.386p
						ASSUME	CS:CODE

						PUBLIC	TEST_CBW
						PUBLIC	TEST_CWDE
						PUBLIC	TEST_CWD
						PUBLIC	TEST_CDQ

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



; void TEST_CBW(unsigned int *res,unsigned int eax)
TEST_CBW				PROC
						PUSH	EBP				; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr,  [EIP+12]=EAX,  [EIP+16]=EDX
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]
						MOV		EAX,[EBP+12]

						CBW

						MOV		[EDI],EAX

						POPAD
						POP		EBP
						RET
TEST_CBW				ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



; void TEST_CWDE(unsigned int *res,unsigned int eax)
TEST_CWDE				PROC
						PUSH	EBP				; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr,  [EIP+12]=EAX,  [EIP+16]=EDX
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]
						MOV		EAX,[EBP+12]

						CWDE

						MOV		[EDI],EAX

						POPAD
						POP		EBP
						RET
TEST_CWDE				ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



; void TEST_CWD(unsigned int res[2],unsigned int eax,unsigned int edx)
TEST_CWD				PROC
						PUSH	EBP				; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr,  [EIP+12]=EAX,  [EIP+16]=EDX
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]
						MOV		EAX,[EBP+12]
						MOV		EDX,[EBP+16]

						CWDE

						MOV		[EDI],EAX
						MOV		[EDI+4],EDX

						POPAD
						POP		EBP
						RET
TEST_CWD				ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



; void TEST_CDQ(unsigned int res[2],unsigned int eax,unsigned int edx)
TEST_CDQ				PROC
						PUSH	EBP				; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr,  [EIP+12]=EAX,  [EIP+16]=EDX
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]
						MOV		EAX,[EBP+12]
						MOV		EDX,[EBP+16]

						CWDE

						MOV		[EDI],EAX
						MOV		[EDI+4],EDX

						POPAD
						POP		EBP
						RET
TEST_CDQ				ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



CODE					ENDS
						END
