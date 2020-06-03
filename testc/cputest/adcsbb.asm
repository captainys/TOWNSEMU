						.386p
						ASSUME	CS:CODE

						PUBLIC	TEST_ADC_SBB_32
						PUBLIC	TEST_ADC_SBB_16
						PUBLIC	TEST_ADC_SBB_8

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



; REG1, REG2 needs to be EAX,ECX,AX,CX,AL,CL
ADC_SBB_X				MACRO	REG1,REG2

						PUSH	EBP		; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EBP+8]=ResultPtr,  [EBP+12]=EAX,  [EBP+16]=ECX
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]
						XOR		EAX,EAX
						XOR		ECX,ECX


						STC
						MOV		REG1,[EBP+12]
						MOV		REG2,[EBP+16]
						ADC		REG1,REG2

						PUSHFD
						POP		ECX
						AND		ECX,EFLAGS_OF+EFLAGS_SF+EFLAGS_ZF+EFLAGS_AF+EFLAGS_CF+EFLAGS_PF

						MOV		[EDI   ],EAX
						MOV		[EDI+ 4],ECX



						CLC
						MOV		REG1,[EBP+12]
						MOV		REG2,[EBP+16]
						ADC		REG1,REG2

						PUSHFD
						POP		ECX
						AND		ECX,EFLAGS_OF+EFLAGS_SF+EFLAGS_ZF+EFLAGS_AF+EFLAGS_CF+EFLAGS_PF

						MOV		[EDI+ 8],EAX
						MOV		[EDI+12],ECX



						STC
						MOV		REG1,[EBP+12]
						MOV		REG2,[EBP+16]
						SBB		REG1,REG2

						PUSHFD
						POP		ECX
						AND		ECX,EFLAGS_OF+EFLAGS_SF+EFLAGS_ZF+EFLAGS_AF+EFLAGS_CF+EFLAGS_PF

						MOV		[EDI+16],EAX
						MOV		[EDI+20],ECX



						CLC
						MOV		REG1,[EBP+12]
						MOV		REG2,[EBP+16]
						SBB		REG1,REG2

						PUSHFD
						POP		ECX
						AND		ECX,EFLAGS_OF+EFLAGS_SF+EFLAGS_ZF+EFLAGS_AF+EFLAGS_CF+EFLAGS_PF

						MOV		[EDI+24],EAX
						MOV		[EDI+28],ECX



						POPAD
						POP		EBP
						RET

						ENDM



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



CODE					SEGMENT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



; void TEST_ADC_SBB_32(unsigned int res[8],unsigned int eax,unsigned int ecx);
TEST_ADC_SBB_32			PROC
						ADC_SBB_X	EAX,ECX
TEST_ADC_SBB_32			ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



; void TEST_ADC_SBB_16(unsigned int res[8],unsigned int eax,unsigned int ecx);
TEST_ADC_SBB_16			PROC
						ADC_SBB_X	AX,CX
TEST_ADC_SBB_16			ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



; void TEST_ADC_SBB_8(unsigned int res[8],unsigned int eax,unsigned int ecx);
TEST_ADC_SBB_8			PROC
						ADC_SBB_X	AL,CL
TEST_ADC_SBB_8			ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



CODE					ENDS
						END
