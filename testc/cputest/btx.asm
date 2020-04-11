						.386p
						ASSUME	CS:CODE

						PUBLIC	BTX_R32_I8
						PUBLIC	BTX_R32_R32

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



ONE_BTX_R_BIT			MACRO		INST,REG,BIT
						PUSH		REG
						INST		REG,BIT
						PUSHFD
						POP			EDX
						AND			EDX,EFLAGS_CF
						MOV			[EDI],REG
						MOV			[EDI+4],EDX
						LEA			EDI,[EDI+8]
						POP			REG
						ENDM



RUN_BTX_R32_I8			MACRO		INST,REG
						ONE_BTX_R_BIT	INST,REG,0
						ONE_BTX_R_BIT	INST,REG,7
						ONE_BTX_R_BIT	INST,REG,12
						ONE_BTX_R_BIT	INST,REG,15
						ONE_BTX_R_BIT	INST,REG,16
						ONE_BTX_R_BIT	INST,REG,20
						ONE_BTX_R_BIT	INST,REG,23
						ONE_BTX_R_BIT	INST,REG,31
						ENDM



BTX_R32_I8				PROC
						PUSH	EBP				; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr,  [EIP+12]=EAX,  [EIP+16]=EDX
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]

						MOV		EBX,[EBP+12]
						RUN_BTX_R32_I8	BT,EBX
						MOV		EBX,[EBP+12]
						RUN_BTX_R32_I8	BTS,EBX
						MOV		EBX,[EBP+12]
						RUN_BTX_R32_I8	BTR,EBX
						MOV		EBX,[EBP+12]
						RUN_BTX_R32_I8	BTC,EBX

						POPAD
						POP		EBP
						RET
BTX_R32_I8				ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



BTX_R32_R32				PROC
						PUSH	EBP				; [EBP]=PrevEBP,  [EBP+4]=EIP,  [EIP+8]=ResultPtr,  [EIP+12]=EAX,  [EIP+16]=EDX
						MOV		EBP,ESP
						PUSHAD

						MOV		EDI,[EBP+8]

						MOV		EBX,[EBP+12]
						MOV		ECX,[EBP+16]
						ONE_BTX_R_BIT	BT,EBX,ECX
						MOV		EBX,[EBP+12]
						MOV		ECX,[EBP+16]
						ONE_BTX_R_BIT	BTS,EBX,ECX
						MOV		EBX,[EBP+12]
						MOV		ECX,[EBP+16]
						ONE_BTX_R_BIT	BTR,EBX,ECX
						MOV		EBX,[EBP+12]
						MOV		ECX,[EBP+16]
						ONE_BTX_R_BIT	BTC,EBX,ECX

						POPAD
						POP		EBP
						RET
BTX_R32_R32				ENDP



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



CODE					ENDS
						END
