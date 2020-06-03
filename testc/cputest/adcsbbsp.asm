						.386p
						ASSUME	CS:CODE

						PUBLIC	TEST_ADC_SP1
						PUBLIC	TEST_ADC_SP2
						PUBLIC	TEST_ADC_SP3
						PUBLIC	TEST_ADC_SP4
						PUBLIC	TEST_SBB_SP1
						PUBLIC	TEST_SBB_SP2
						PUBLIC	TEST_SBB_SP3

; Special cases ADC and SBB

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



TEST_ADC_SP1			PROC
						PUSH	EBP		; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]

						STC
						MOV		EAX,0FFFFFFFFH
						MOV		ECX,0FFFFFFFFH
						ADC		EAX,ECX

						PUSHFD
						POP		ECX
						AND		ECX,EFLAGS_OF+EFLAGS_SF+EFLAGS_ZF+EFLAGS_AF+EFLAGS_CF+EFLAGS_PF

						MOV		[EDI],EAX
						MOV		[EDI+4],ECX

						POPAD
						POP		EBP
						RET
TEST_ADC_SP1			ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



TEST_ADC_SP2	PROC
						PUSH	EBP		; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]

						STC
						MOV		EAX,0FFFFH
						MOV		ECX,0FFFFFFFFH
						ADC		EAX,ECX

						PUSHFD
						POP		ECX
						AND		ECX,EFLAGS_OF+EFLAGS_SF+EFLAGS_ZF+EFLAGS_AF+EFLAGS_CF+EFLAGS_PF

						MOV		[EDI],EAX
						MOV		[EDI+4],ECX

						POPAD
						POP		EBP
						RET
TEST_ADC_SP2	ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



TEST_ADC_SP3	PROC
						PUSH	EBP		; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]

						STC
						MOV		EAX,0FFFFFFFFH
						MOV		ECX,0
						ADC		EAX,ECX

						PUSHFD
						POP		ECX
						AND		ECX,EFLAGS_OF+EFLAGS_SF+EFLAGS_ZF+EFLAGS_AF+EFLAGS_CF+EFLAGS_PF

						MOV		[EDI],EAX
						MOV		[EDI+4],ECX

						POPAD
						POP		EBP
						RET
TEST_ADC_SP3	ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



TEST_ADC_SP4	PROC
						PUSH	EBP		; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]

						STC
						MOV		EAX,0
						MOV		ECX,0FFFFFFFFH
						ADC		EAX,ECX

						PUSHFD
						POP		ECX
						AND		ECX,EFLAGS_OF+EFLAGS_SF+EFLAGS_ZF+EFLAGS_AF+EFLAGS_CF+EFLAGS_PF

						MOV		[EDI],EAX
						MOV		[EDI+4],ECX

						POPAD
						POP		EBP
						RET
TEST_ADC_SP4	ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



TEST_SBB_SP1	PROC
						PUSH	EBP		; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]

						STC
						MOV		EAX,0
						MOV		ECX,0FFFFFFFFH
						SBB		EAX,ECX

						PUSHFD
						POP		ECX
						AND		ECX,EFLAGS_OF+EFLAGS_SF+EFLAGS_ZF+EFLAGS_AF+EFLAGS_CF+EFLAGS_PF

						MOV		[EDI],EAX
						MOV		[EDI+4],ECX

						POPAD
						POP		EBP
						RET
TEST_SBB_SP1	ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



TEST_SBB_SP2	PROC
						PUSH	EBP		; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]

						STC
						MOV		EAX,1000
						MOV		ECX,1000
						SBB		EAX,ECX

						PUSHFD
						POP		ECX
						AND		ECX,EFLAGS_OF+EFLAGS_SF+EFLAGS_ZF+EFLAGS_AF+EFLAGS_CF+EFLAGS_PF

						MOV		[EDI],EAX
						MOV		[EDI+4],ECX

						POPAD
						POP		EBP
						RET
TEST_SBB_SP2	ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



TEST_SBB_SP3	PROC
						PUSH	EBP		; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]

						STC
						MOV		EAX,0
						MOV		ECX,0
						SBB		EAX,ECX

						PUSHFD
						POP		ECX
						AND		ECX,EFLAGS_OF+EFLAGS_SF+EFLAGS_ZF+EFLAGS_AF+EFLAGS_CF+EFLAGS_PF

						MOV		[EDI],EAX
						MOV		[EDI+4],ECX

						POPAD
						POP		EBP
						RET
TEST_SBB_SP3	ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



CODE					ENDS
						END
